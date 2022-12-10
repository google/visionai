// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include <cstdlib>
#include <string>

#include "glog/logging.h"
#include "google/cloud/visionai/v1/annotations.pb.h"
#include "google/protobuf/any.pb.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/apps/flags.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/event_update.h"
#include "visionai/streams/client/event_update_receiver.h"
#include "visionai/streams/client/packet_receiver.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/random_string.h"
#include "visionai/util/status/status_macros.h"

ABSL_FLAG(std::string, receiver_id, "",
          "A name to identify (you) the receiver.");
ABSL_FLAG(bool, summary_only, false,
          "Print just a short summary about the received packets.");
ABSL_FLAG(bool, try_decode_protobuf, true,
          "If the package is a protobuf, try decode it against known types and "
          "print.");

namespace visionai {

namespace {

constexpr absl::Duration kEventPollPeriod = absl::Seconds(5);
constexpr absl::Duration kPacketPollPeriod = absl::Milliseconds(100);
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
GET_STRING_FLAG_FUNC(GetStreamId, stream_id)

#undef GET_STRING_FLAG_FUNC

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
}  // namespace


absl::Status DEPRECATED_Run();

class ReceiveCat {
 public:
  struct Options {
    ClusterSelection cluster_selection;
    std::string stream_id;
    std::string event_id;
    std::string receiver_id;
    bool summary_only;
    bool try_decode_protobuf;
  };

  static absl::StatusOr<std::shared_ptr<ReceiveCat>> Create(
      const Options& options) {
    auto receive_cat = std::make_shared<ReceiveCat>(options);
    VAI_RETURN_IF_ERROR(receive_cat->Initialize());
    return receive_cat;
  }

  absl::Status Initialize() {
    PacketReceiver::Options options;
    options.cluster_selection = options_.cluster_selection;
    options.channel.stream_id = options_.stream_id;
    options.channel.event_id = options_.event_id;
    options.lessee = options_.receiver_id;
    options.receive_mode = "eager";
    VAI_ASSIGN_OR_RETURN(packet_receiver_, PacketReceiver::Create(options));
    return absl::OkStatus();
  }

  void Run() {
    Packet p;
    while (!is_cancelled_.HasBeenNotified()) {
      auto status = packet_receiver_->Receive(kPacketPollPeriod, &p);

      // Decide if there is a timeout.
      if (absl::IsNotFound(status)) {
        continue;
      }

      // Case 1: Upstream closed. Time to quit.
      //
      // TODO: Should propagate error back to the main thread so it may handle a
      // retry if appropriate, or just handle it here.
      if (!status.ok()) {
        if (absl::IsOutOfRange(status)) {
          LOG(INFO) << absl::StrFormat("Reached the end of event \"%s\"",
                                       options_.event_id);
        } else {
          LOG(ERROR) << status;
        }
        return;
      }

      // Case 2: Got an actual packet. Print it.
      if (options_.summary_only) {
        LOG(INFO) << p.header().DebugString()
                  << "payload size: " << p.payload().size() << " bytes.";
      } else {
        LOG(INFO) << p.DebugString();
        if (options_.try_decode_protobuf &&
            p.header().type().type_class() == "protobuf") {
          google::protobuf::Any any;
          any.set_type_url(
              absl::StrFormat("type.googleapis.com/%s",
                              p.header().type().type_descriptor().type()));
          any.set_value(p.payload());
          LOG(INFO) << any.DebugString();
        }
      }
    }
  }

  void Cancel() {
    if (!is_cancelled_.HasBeenNotified()) {
      is_cancelled_.Notify();
    }
  }

  explicit ReceiveCat(const Options& options) : options_(options) {}
  ~ReceiveCat() = default;

 private:
  Options options_;
  absl::Notification is_cancelled_;
  std::unique_ptr<PacketReceiver> packet_receiver_ = nullptr;
};

absl::Status Run() {
  VAI_ASSIGN_OR_RETURN(auto cluster_selection, ClusterSelectionFromCommandLine());
  VAI_ASSIGN_OR_RETURN(auto stream_id, GetStreamId(),
                   _ << "while getting the stream id");
  auto event_id = absl::GetFlag(FLAGS_event_id);
  auto receiver_id = absl::GetFlag(FLAGS_receiver_id);
  bool summary_only = absl::GetFlag(FLAGS_summary_only);
  bool try_decode_protobuf = absl::GetFlag(FLAGS_try_decode_protobuf);

  if (receiver_id.empty()) {
    VAI_ASSIGN_OR_RETURN(receiver_id, RandomResourceId(kReceiverNameLength));
    LOG(INFO) << absl::StrFormat(
        "Given an empty receiver-id; generated the following id for use: %s",
        receiver_id);
  }

  // If an event is explicitly specified, then receive from just that.
  if (!event_id.empty()) {
    ReceiveCat::Options options;
    options.cluster_selection = cluster_selection;
    options.stream_id = stream_id;
    options.event_id = event_id;
    options.receiver_id = receiver_id;
    options.summary_only = summary_only;
    options.try_decode_protobuf = try_decode_protobuf;
    VAI_ASSIGN_OR_RETURN(auto receive_cat, ReceiveCat::Create(options));
    receive_cat->Run();
    return absl::OkStatus();
  }

  // When the event is not specified, then use the event update receiver to
  // transition to the latest.
  EventUpdateReceiver::Options event_update_receiver_options;
  event_update_receiver_options.cluster_selection = cluster_selection;
  event_update_receiver_options.stream_id = stream_id;
  event_update_receiver_options.receiver = receiver_id;
  event_update_receiver_options.starting_logical_offset = "most-recent";
  event_update_receiver_options.fallback_starting_offset = "end";
  VAI_ASSIGN_OR_RETURN(auto event_update_receiver,
                   EventUpdateReceiver::Create(event_update_receiver_options));

  std::thread t;
  std::shared_ptr<ReceiveCat> receive_cat = nullptr;

  bool read_ok = false;
  while (true) {
    EventUpdate event_update;

    // Repeatedly poll for new events.
    // If timeout, then just try again.
    while (!event_update_receiver->Receive(kEventPollPeriod, &event_update,
                                           &read_ok))
      ;

    // Upstream closed; no more events will arrive.
    if (!read_ok) {
      break;
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
      break;
    }
    auto stream_id = GetStreamId(event_update);
    if (!stream_id.ok()) {
      LOG(ERROR) << absl::StrFormat(
          "Failed to get an stream id from a newly arrived `EventUpdate`: %s",
          stream_id.status().message());
      break;
    }
    ReceiveCat::Options options;
    options.cluster_selection = cluster_selection;
    options.stream_id = *stream_id;
    options.event_id = *event_id;
    options.receiver_id = receiver_id;
    options.summary_only = summary_only;
    options.try_decode_protobuf = try_decode_protobuf;
    VAI_ASSIGN_OR_RETURN(receive_cat, ReceiveCat::Create(options));
    t = std::thread(&ReceiveCat::Run, receive_cat);
  }

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

  return absl::OkStatus();
}

}  // namespace visionai

int main(int argc, char** argv) {
  std::string usage = R"usage(
Usage: receive_cat_app [OPTION]

Receive (raw) packets directly from a stream.

)usage";

  absl::SetProgramUsageMessage(usage);
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);
  visionai::ForceLinkAnnotationProtos();

  absl::Status return_status = visionai::Run();
  if (!return_status.ok()) {
    LOG(ERROR) << return_status;
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}
