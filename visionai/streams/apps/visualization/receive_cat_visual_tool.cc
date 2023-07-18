// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/visualization/receive_cat_visual_tool.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>  // NOLINT(build/c++11)
#include <tuple>
#include <utility>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/gstreamer_async_decoder.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/apps/visualization/drawable.h"
#include "visionai/streams/apps/visualization/object_detection_drawable.h"
#include "visionai/streams/apps/visualization/oc_drawable.h"
#include "visionai/streams/apps/visualization/ppe_result_drawable.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/packet_receiver.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/thread/sync_queue.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

absl::Status ReceiveCatVisualTool::Initialize() {
  PacketReceiver::Options options;
  options.cluster_selection = options_.cluster_selection;
  options.channel.stream_id = options_.stream_id;
  options.channel.event_id = options_.event_id;
  options.lessee = options_.receiver_id;
  VAI_ASSIGN_OR_RETURN(packet_receiver_, PacketReceiver::Create(options));
  return absl::OkStatus();
}

void ReceiveCatVisualTool::Run() {
  Packet p;

  GstreamerAsyncDecoder<DecoderContext> decoder(
      [](absl::StatusOr<RawImage> image, DecoderContext context) {
        if (image.ok()) {
          std::string img = std::move(*image).ReleaseBuffer();
          context.queue->Push(std::make_tuple(context.time, image->width(),
                                              image->height(), img));
        }
      });

  while (!is_cancelled_.HasBeenNotified()) {
    auto status = packet_receiver_->Receive(absl::Milliseconds(100), &p);

    // Decide if there is a timeout.
    if (absl::IsNotFound(status)) {
      continue;
    }

    // Case 1: Upstream closed. Time to quit.
    //
    // Just handle the error here.
    if (!status.ok()) {
      if (absl::IsOutOfRange(status)) {
        LOG(INFO) << absl::StrFormat("Reached the end of event \"%s\"",
                                     options_.event_id);
      } else {
        LOG(ERROR) << status;
      }
      return;
    }

    // Case 2: Got an actual packet. Process it.
    if (options_.summary_only) {
      LOG(INFO) << p.header().DebugString()
                << "payload size: " << p.payload().size() << " bytes.";
    } else {
      absl::Time time = visionai::ToAbseilTimestamp(p.header().capture_time());
      if (options_.try_decode_protobuf &&
          p.header().type().type_class() == "protobuf") {
        std::unique_ptr<renderutils::Drawable> result = Parser(p);
        if (result) {
          options_.a_queue->Push(std::make_tuple(time, std::move(result)));
        }
      } else {
        auto packetas = PacketAs<GstreamerBuffer>(std::move(p));
        if (packetas.status().ok()) {
          GstreamerBuffer gstreamer_buffer = packetas.value();
          DecoderContext context;
          context.time = time;
          context.queue = options_.v_queue;
          auto result = decoder.Feed(gstreamer_buffer, context);
        }
      }
    }
  }
}

void ReceiveCatVisualTool::Cancel() {
  if (!is_cancelled_.HasBeenNotified()) {
    is_cancelled_.Notify();
  }
}

template <class ResultType, class DrawableType,
          class... AddditionalConstructorArgs>
std::unique_ptr<renderutils::Drawable> ReceiveCatVisualTool::CreateDrawable(
    Packet p, AddditionalConstructorArgs... extra_vals) {
  // Parse the result type from the packet payload and return a pointer to the
  // drawable type
  ResultType result;
  result.ParseFromString(p.payload());
  return std::unique_ptr<renderutils::Drawable>{
      new DrawableType{result, extra_vals...}};
}

absl::flat_hash_map<absl::string_view, ReceiveCatVisualTool::ModelType>
ReceiveCatVisualTool::GetDetectionTypeMap() {
  static const auto* const map =
      new absl::flat_hash_map<absl::string_view, ModelType>{
          {"google.cloud.visionai.v1.ObjectDetectionPredictionResult",
           ReceiveCatVisualTool::ObjectDetector},
          {"google.cloud.visionai.v1."
           "PersonalProtectiveEquipmentDetectionOutput",
           ReceiveCatVisualTool::PPEDetector},
          {"google.cloud.visionai.v1.OccupancyCountingPredictionResult",
           ReceiveCatVisualTool::OccupancyAnalysis},
      };
  return *map;
}

// Implementation for the parser function
// Parser will take in the type descriptor from the packet and determine
// which drawable objects to create at the moment only PPE and General Object
std::unique_ptr<renderutils::Drawable> ReceiveCatVisualTool::Parser(
    const Packet& p) {
  // Check if the packet is coming from a valid model. It will be in a valid
  // model if it exists in our type mapping
  absl::string_view descriptor = p.header().type().type_descriptor().type();
  // If this model type is not supported, return a null Drawable
  if (GetDetectionTypeMap().find(descriptor) == GetDetectionTypeMap().end()) {
    return std::unique_ptr<renderutils::Drawable>();
  }
  // Based off the mapping, return a pointer to the model's Drawable
  switch (GetDetectionTypeMap().at(descriptor)) {
    case OccupancyAnalysis:
      return CreateDrawable<
          google::cloud::visionai::v1::OccupancyCountingPredictionResult,
          renderutils::OccupancyAnalysisDrawable>(p);
    case PPEDetector:
      return CreateDrawable<google::cloud::visionai::v1::
                                PersonalProtectiveEquipmentDetectionOutput,
                            renderutils::PPEResultDrawable>(p);
    case ObjectDetector:
      return CreateDrawable<google::cloud::visionai::v1::
                            ObjectDetectionPredictionResult,
                           renderutils::ObjectDetectionDrawable>(p);
  }
}
}  // namespace visionai
