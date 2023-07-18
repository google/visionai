// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>  // NOLINT(build/c++11)
#include <tuple>
#include <utility>
#include <vector>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/highgui.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "opencv4/opencv2/videoio.hpp"
#include "absl/cleanup/cleanup.h"
#include "absl/container/flat_hash_map.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/apps/flags.h"
#include "visionai/streams/apps/visualization/drawable.h"
#include "visionai/streams/apps/visualization/receive_cat_visual_tool.h"
#include "visionai/streams/apps/visualization/render_utils.h"
#include "visionai/streams/client/event_update.h"
#include "visionai/streams/client/event_update_receiver.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/thread/sync_queue.h"
#include "visionai/util/status/status_macros.h"

ABSL_FLAG(std::string, v_stream_id, "", "ID of the video stream.");
ABSL_FLAG(std::string, a_stream_id, "", "ID of the annotation stream.");
ABSL_FLAG(std::string, receiver_id, "",
          "A name to identify (you) the receiver.");
ABSL_FLAG(bool, summary_only, false,
          "Print just a short summary about the received packets.");
ABSL_FLAG(bool, try_decode_protobuf, true,
          "If the package is a protobuf, try decode it against known types and "
          "print.");
ABSL_FLAG(bool, same_fps, false,
          "Should use same fps for both video and annotation");
ABSL_FLAG(bool, no_display, false,
          "Disable rendering to display, need to have a valid "
          "output_video_filepath.");
ABSL_FLAG(std::string, output_video_filepath, "",
          "The full path and filename of the annotated video, supports only "
          ".avi format. If it is empty, no video file will be generated. ");

namespace visionai {

namespace {

constexpr absl::Duration kEventPollPeriod = absl::Seconds(5);
constexpr int kReceiverNameLength = 8;

#define GET_STRING_FLAG_FUNC(FuncName, flag)                              \
  absl::StatusOr<std::string> FuncName() {                                \
    std::string flag_value = absl::GetFlag(FLAGS_##flag);                 \
    if (flag_value.empty()) {                                             \
      return absl::InvalidArgumentError(absl::StrFormat(                  \
          "Given an empty string for command line flag \"%s\".", #flag)); \
    }                                                                     \
    return flag_value;                                                    \
  }

GET_STRING_FLAG_FUNC(GetServiceEndpoint, service_endpoint)
GET_STRING_FLAG_FUNC(GetProjectId, project_id)
GET_STRING_FLAG_FUNC(GetLocationId, location_id)
GET_STRING_FLAG_FUNC(GetClusterId, cluster_id)
GET_STRING_FLAG_FUNC(GetVStreamId, v_stream_id)
GET_STRING_FLAG_FUNC(GetAStreamId, a_stream_id)

#undef GET_STRING_FLAG_FUNC

// Gets cluster selection information from command-line flags.
absl::StatusOr<ClusterSelection> ClusterSelectionFromCommandLine() {
  VAI_ASSIGN_OR_RETURN(auto service_endpoint, GetServiceEndpoint(),
                   _ << "while getting the service endpoint");
  VAI_ASSIGN_OR_RETURN(auto project_id, GetProjectId(),
                   _ << "while getting the project id");
  VAI_ASSIGN_OR_RETURN(auto location_id, GetLocationId(),
                   _ << "while getting the location id");
  VAI_ASSIGN_OR_RETURN(auto cluster_id, GetClusterId(),
                   _ << "while getting the location id");
  ClusterSelection cluster_selection;
  cluster_selection.set_service_endpoint(service_endpoint);
  cluster_selection.set_project_id(project_id);
  cluster_selection.set_location_id(location_id);
  cluster_selection.set_cluster_id(cluster_id);
  return cluster_selection;
}

// This function is here to force reference the annotation protos, so that they
// are linked in the binary for google::protobuf::Any to parse.
void ForceLinkAnnotationProtos() {
  google::cloud::visionai::v1::ImageObjectDetectionPredictionResult
      image_object_detection_instance;
  google::cloud::visionai::v1::ClassificationPredictionResult
      image_classification_instance;
  google::cloud::visionai::v1::ImageSegmentationPredictionResult
      image_segmentation_instance;
  google::cloud::visionai::v1::VideoActionRecognitionPredictionResult
      video_action_recognition_instance;
  google::cloud::visionai::v1::VideoObjectTrackingPredictionResult
      video_object_tracking_instance;
  google::cloud::visionai::v1::VideoClassificationPredictionResult
      video_classification_instance;
  google::cloud::visionai::v1::OccupancyCountingPredictionResult
      occupancy_count_instance;
}

// Closes event update receiver and clean related resources.
void CloseEventCleanResource(
    bool read_ok,
    std::unique_ptr<visionai::EventUpdateReceiver> event_update_receiver,
    std::thread& t, std::shared_ptr<ReceiveCatVisualTool> receive_cat) {
  // Close event update receiver and get its status.
  if (read_ok) {
    event_update_receiver->Cancel();
  } else {
    event_update_receiver->CommitsDone();
  }
  auto event_update_receiver_rpc_status = event_update_receiver->Finish();
  if (!event_update_receiver_rpc_status.ok()) {
    if (!absl::IsOutOfRange(event_update_receiver_rpc_status)) {
      LOG(ERROR)
          << "Got an unexpected RPC status for the event update receiver: "
          << event_update_receiver_rpc_status;
    }
  } else {
    LOG(ERROR) << absl::InternalError(
        "Got an OK status from `EventUpdateReceiver::Finish`, which is "
        "unexpected");
  }

  // Let an existing packet receiver complete if the event update receiver
  // reaches out of range; otherwise, just cancel it.
  if (receive_cat != nullptr &&
      !absl::IsOutOfRange(event_update_receiver_rpc_status)) {
    receive_cat->Cancel();
  }
  if (t.joinable()) {
    t.join();
  }
}

// Gets the tuple of event_id and stream_id from an event update receiver.
absl::StatusOr<std::tuple<std::string, std::string>> GetEventIdStreamId(
    bool& read_ok, EventUpdateReceiver* event_update_receiver,
    std::shared_ptr<ReceiveCatVisualTool> receive_cat, std::thread& t) {
  EventUpdate event_update;

  // Repeatedly poll for new events.
  // If timeout, then just try again.
  while (!event_update_receiver->Receive(kEventPollPeriod, &event_update,
                                         &read_ok)) {
  }
  // Upstream closed; no more events will arrive.
  if (!read_ok) {
    return absl::InternalError("Upstream closed");
  }
  // New event arrival. Cancel the old and read from the new.
  if (receive_cat != nullptr) {
    receive_cat->Cancel();
    t.detach();
  }
  auto event_id = GetEventId(event_update);
  if (!event_id.ok()) {
    LOG(ERROR) << absl::StrFormat(
        "Failed to get an event id from a newly arrived `EventUpdate`: %s",
        event_id.status().message());
    return absl::InternalError("Failed to get an event id");
  }
  auto stream_id = GetStreamId(event_update);
  if (!stream_id.ok()) {
    LOG(ERROR) << absl::StrFormat(
        "Failed to get an stream id from a newly arrived `EventUpdate`: %s",
        stream_id.status().message());
    return absl::InternalError("Failed to get a stream id");
  }
  return std::make_tuple(*event_id, *stream_id);
}

// Runs the Anaheim Visualization Tool.
// The visualization tool uses ReceiveCatVisualTool to get image frames from
// video stream output and annotation protobufs from annotation stream output,
// synchronize the annotation stream with the video stream according to their
// capture time, and visualize the annotations on the video frames using OpenCV.
absl::Status Run() {
  VAI_ASSIGN_OR_RETURN(auto cluster_selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto v_stream_id, GetVStreamId(),
                   _ << "while getting the video stream id");
  VAI_ASSIGN_OR_RETURN(auto a_stream_id, GetAStreamId(),
                   _ << "while getting the annotation stream id");
  auto receiver_id = absl::GetFlag(FLAGS_receiver_id);
  bool summary_only = absl::GetFlag(FLAGS_summary_only);
  bool try_decode_protobuf = absl::GetFlag(FLAGS_try_decode_protobuf);
  bool same_fps = absl::GetFlag(FLAGS_same_fps);
  bool no_display = absl::GetFlag(FLAGS_no_display);
  std::string output_video_filepath =
      absl::GetFlag(FLAGS_output_video_filepath);

  if (receiver_id.empty()) {
    VAI_ASSIGN_OR_RETURN(receiver_id, RandomResourceId(kReceiverNameLength));
    LOG(INFO) << absl::StrFormat(
        "Given an empty receiver-id; generated the following id for use: %s",
        receiver_id);
  }

  // Invalid case if no output file specificed & display disabled, exit.
  if (no_display && output_video_filepath.empty()) {
    LOG(ERROR) << "Display disabled & no output_video_filepath set, exit.";
    return absl::OkStatus();
  }

  // Use the event update receiver to transition to the latest.
  EventUpdateReceiver::Options event_update_receiver_options;
  event_update_receiver_options.cluster_selection = cluster_selection;
  event_update_receiver_options.stream_id = v_stream_id;
  event_update_receiver_options.receiver = receiver_id;
  event_update_receiver_options.starting_logical_offset = "most-recent";
  event_update_receiver_options.fallback_starting_offset = "end";
  VAI_ASSIGN_OR_RETURN(auto v_event_update_receiver,
                   EventUpdateReceiver::Create(event_update_receiver_options));

  event_update_receiver_options.stream_id = a_stream_id;
  VAI_ASSIGN_OR_RETURN(auto a_event_update_receiver,
                   EventUpdateReceiver::Create(event_update_receiver_options));

  std::thread v_t;
  std::thread a_t;
  std::shared_ptr<ReceiveCatVisualTool> v_receive_cat = nullptr;
  std::shared_ptr<ReceiveCatVisualTool> a_receive_cat = nullptr;
  SyncQueue<std::tuple<absl::Time, int, int, std::string>> v_queue;
  SyncQueue<std::tuple<absl::Time, std::unique_ptr<renderutils::Drawable>>>
      a_queue;
  const std::string window_name = "Anaheim Visualization Tool";
  if (!no_display) {
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  }
  const auto cleanup = absl::MakeCleanup([]() { cv::destroyAllWindows(); });

  bool v_read_ok = false;
  bool a_read_ok = false;
  while (true) {
    auto v_event_stream_tuple = GetEventIdStreamId(
        v_read_ok, v_event_update_receiver.get(), v_receive_cat, v_t);
    if (!v_event_stream_tuple.ok()) {
      break;
    }
    std::string v_event_id = std::get<0>(*v_event_stream_tuple);
    std::string v_stream_id = std::get<1>(*v_event_stream_tuple);

    auto a_event_stream_tuple = GetEventIdStreamId(
        a_read_ok, a_event_update_receiver.get(), a_receive_cat, a_t);
    if (!a_event_stream_tuple.ok()) {
      break;
    }
    std::string a_event_id = std::get<0>(*a_event_stream_tuple);
    std::string a_stream_id = std::get<1>(*a_event_stream_tuple);

    ReceiveCatVisualTool::Options options;
    options.cluster_selection = cluster_selection;
    options.receiver_id = receiver_id;
    options.summary_only = summary_only;
    options.try_decode_protobuf = try_decode_protobuf;
    options.v_queue = &v_queue;
    options.a_queue = &a_queue;

    options.stream_id = v_stream_id;
    options.event_id = v_event_id;
    VAI_ASSIGN_OR_RETURN(v_receive_cat, ReceiveCatVisualTool::Create(options));
    v_t = std::thread(&ReceiveCatVisualTool::Run, v_receive_cat);

    options.stream_id = a_stream_id;
    options.event_id = a_event_id;
    options.is_annotation_stream = true;
    VAI_ASSIGN_OR_RETURN(a_receive_cat, ReceiveCatVisualTool::Create(options));
    a_t = std::thread(&ReceiveCatVisualTool::Run, a_receive_cat);

    absl::Time latest_anno_time = absl::InfinitePast();
    std::unique_ptr<renderutils::Drawable> latest_drawable;
    absl::Duration annotation_buffer_time = absl::ZeroDuration();

    // Initiate track_id_map and dwell_stats_map for dwell time feature.
    // Since it is unable to decide whether the dwell time feature is turned on
    // or not from a single annotation, we will think as if the dwell time
    // feature is always turned on.

    // track_id_map stores "track_id -> latest_update_time" mapping.
    absl::flat_hash_map<std::string, absl::Time> track_id_map;

    // dwell_stats_map stores "track_id -> a vector of DwellTimeInfo" mapping
    absl::flat_hash_map<std::string, std::vector<renderutils::DwellTimeInfo>>
        dwell_stats_map;

    // show_score is a flag to switch on/off confidence score display near
    // bounding boxes.
    bool show_score = false;

    // show_fps is used to calculate fps of annotation output stream.
    int fps_counter = -1;
    absl::Time prev_time;
    double annotation_fps = 0.0;

    // If output_video_filepath is not empty, write annotated video to this
    // designated full filename.
    bool write_video = !output_video_filepath.empty();

    // Prepare VideoWriter
    cv::VideoWriter video_writer;

    while (true) {
      while (!v_queue.Empty()) {
        std::tuple<absl::Time, int, int, std::string> image;
        image = v_queue.Pop().value();
        int width = std::get<1>(image);
        int height = std::get<2>(image);
        char* data = (char*)std::get<3>(image).data();
        cv::Mat img = cv::Mat(height, width, CV_8UC3, data).clone();
        cv::cvtColor(img, img, cv::COLOR_RGB2BGR);

        absl::Time img_time = std::get<0>(image);
        while (absl::Now() - img_time < annotation_buffer_time) {
          absl::SleepFor(absl::Milliseconds(50));
        }
        // We want our annotation time to be as close to the video frame's
        // timestamp as possible. Ideally equivalent or just slightly above
        if (!a_queue.Empty() && latest_anno_time < img_time) {
          std::tuple<absl::Time, std::unique_ptr<renderutils::Drawable>> anno;
          while (!a_queue.Empty() && std::get<0>(anno) < img_time) {
            anno = a_queue.Pop().value();
            ++fps_counter;
            if (fps_counter == 0) {
              // Get the first annotation time.
              prev_time = std::get<0>(anno);
            } else if (fps_counter == 5) {
              // Calculate fps every 5 annotations.
              absl::Duration time_gap = std::get<0>(anno) - prev_time;
              annotation_fps =
                  5.0 / absl::FDivDuration(time_gap, absl::Seconds(1));
              prev_time = std::get<0>(anno);
              fps_counter = 0;
            }
          }
          latest_anno_time = std::get<0>(anno);
          latest_drawable = std::move(std::get<1>(anno));
          if (absl::Now() - latest_anno_time > annotation_buffer_time) {
            annotation_buffer_time = absl::Now() - latest_anno_time;
            LOG(INFO) << "Annotation buffer time: " << annotation_buffer_time;
          }
        }
        if (same_fps && img_time != latest_anno_time) {
          continue;
        }
        if ((img_time > latest_anno_time
                 ? img_time - latest_anno_time
                 : latest_anno_time - img_time) < absl::Milliseconds(1000)) {
          latest_drawable->draw(img, width, height);
          // Show fps of annotation output stream.
          renderutils::ShowAnnotationFps(img, annotation_fps);
        }

        if (!no_display) {
          cv::imshow(window_name, img);
        }

        if (write_video) {
          if (!video_writer.isOpened()) {
            LOG(INFO) << "Opening output video file: " << output_video_filepath;
            bool is_opened =
                video_writer.open(output_video_filepath,
                                  cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                                  25.0, cv::Size(width, height), true);
            if (!is_opened) {
              LOG(ERROR) << "Cannot open output video file: "
                         << output_video_filepath;
              write_video = false;
            }
          }
          video_writer.write(img);
        }
        const int pressed_key = cv::waitKey(5);
        if (pressed_key == /*Enter*/ 13 || pressed_key == /*s*/ 115) {
          // Change show_score when "Enter" or "s" is pressed.
          show_score = !show_score;
        } else if (pressed_key == /*Esc*/ 27) {
          cv::destroyAllWindows();
          CloseEventCleanResource(v_read_ok, std::move(v_event_update_receiver),
                                  v_t, v_receive_cat);
          CloseEventCleanResource(a_read_ok, std::move(a_event_update_receiver),
                                  a_t, a_receive_cat);
          return absl::OkStatus();
        }
      }
      cv::waitKey(1);
    }
  }

  CloseEventCleanResource(v_read_ok, std::move(v_event_update_receiver), v_t,
                          v_receive_cat);
  CloseEventCleanResource(a_read_ok, std::move(a_event_update_receiver), a_t,
                          a_receive_cat);

  return absl::OkStatus();
}

}  // namespace
}  // namespace visionai

int main(int argc, char** argv) {
  std::string usage = R"usage(
Ahaheim Visualization Tool

Visualize model results on video streams.

)usage";

  absl::SetProgramUsageMessage(usage);
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);

  visionai::ForceLinkAnnotationProtos();
  auto result = visionai::GstRegisterPlugins();

  auto status = visionai::Run();
  if (!status.ok()) {
    LOG(ERROR) << status;
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}
