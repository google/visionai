// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_RECEIVE_CAT_VISUAL_TOOL_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_RECEIVE_CAT_VISUAL_TOOL_H_
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>  // NOLINT(build/c++11)
#include <tuple>
#include <utility>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "absl/base/attributes.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/apps/visualization/drawable.h"
#include "visionai/streams/client/packet_receiver.h"
#include "visionai/util/thread/sync_queue.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
// ReceiveCatVisualTool is used by Anaheim Visualization Tool.
// ReceiveCatVisualTool uses Anaheim SDK to read packets from Vertex AI Vision
// projects' output streams. There are 2 types of use cases:
// 1. Repeatedly reads packets of an event from a video stream, then decodes
// those packets using GstreamerAsyncDecoder, then put decoded raw images in a
// SyncQueue.
// 2. Repeatedly reads packets of an event from an annotation stream, then put
// the annotation protobufs in another SyncQueue.
// Which one to use is decided by the client, setting
// ReceiveCatVisualTool::Options.is_annotation_stream.
class ReceiveCatVisualTool {
 public:
  // Options used to create a ReceiveCatVisualTool instance.
  struct Options {
    ClusterSelection cluster_selection;
    std::string stream_id;
    std::string event_id;
    std::string receiver_id;
    bool summary_only;
    bool try_decode_protobuf;
    SyncQueue<std::tuple<absl::Time, int, int, std::string>>* v_queue;
    SyncQueue<std::tuple<absl::Time, std::unique_ptr<renderutils::Drawable>>>*
        a_queue;
    bool is_annotation_stream = false;
  };

  // DecoderContext provides information for the GstreamerAsyncDecoder's
  // callback function.
  struct DecoderContext {
    absl::Time time;
    SyncQueue<std::tuple<absl::Time, int, int, std::string>>* queue;
  };

  // Creates a ReceiveCatVisualTool shared pointer or return absl::Status.
  static absl::StatusOr<std::shared_ptr<ReceiveCatVisualTool>> Create(
      const Options& options) {
    auto receive_cat = std::make_shared<ReceiveCatVisualTool>(options);
    VAI_RETURN_IF_ERROR(receive_cat->Initialize());
    return receive_cat;
  }

  // The current list of supported models and the enumeration to use within the
  // switch statement in the parser.
  enum ModelType { OccupancyAnalysis, PPEDetector, ObjectDetector };

  // Initializes the PacketReceiver.
  absl::Status Initialize();

  // Starts reading from a video/annotation stream.
  void Run();

  // Cancels current job.
  void Cancel();

  explicit ReceiveCatVisualTool(const Options& options) : options_(options) {}
  ~ReceiveCatVisualTool() = default;

  // The mapping definition from a string type descriptor for a proto packet to
  // its appropriate model type
  static absl::flat_hash_map<absl::string_view, ModelType>
  GetDetectionTypeMap();

  // Parser function to grab packets and create drawable functions
  // The parser will check the type of model and create a drawable based on that
  // placeholder for functions that might be used by child implementations
  std::unique_ptr<renderutils::Drawable> Parser(const Packet& p);

 private:
  Options options_;
  absl::Notification is_cancelled_;
  std::unique_ptr<PacketReceiver> packet_receiver_ = nullptr;

  // Create a drawable object with the Prediction 'ResultType' and
  // 'DrawableType' provided as template parameters. This needs to be a template
  // method as we need the type of both the prediction result and the drawable
  // subclass to create it. If a Drawable Type's constructor has additional
  // arguments, they can be provided as parameters to this method (as
  // 'AddditionalConstructorArgs' is a variadic template type) and they will be
  // passed along to the constructor of the Drawable
  template <class DrawableType, class ResultType,
            class... AddditionalConstructorArgs>
  std::unique_ptr<renderutils::Drawable> CreateDrawable(
      Packet p, AddditionalConstructorArgs... extra_vals);
};
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_RECEIVE_CAT_VISUAL_TOOL_H_
