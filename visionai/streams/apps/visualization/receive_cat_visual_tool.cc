// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/visualization/receive_cat_visual_tool.h"

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>  // NOLINT(build/c++11)
#include <tuple>
#include <utility>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/gstreamer_async_decoder.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/packet_receiver.h"
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
        if (p.header().type().type_descriptor().type() ==
            "google.cloud.visionai.v1."
            "OccupancyCountingPredictionResult") {
          google::cloud::visionai::v1::OccupancyCountingPredictionResult
              oc_result;
          oc_result.ParseFromString(p.payload());
          options_.a_queue->Push(std::make_tuple(time, oc_result));
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

}  // namespace visionai
