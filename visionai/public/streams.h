// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// streams.h
// -----------------------------------------------------------------------------
//
// This header file contains the public C++ interface for Vertex AI Vision.
// Users should code against this header file rather than those in other
// directories as those may change without warning.
//
// This programming API is divided into several sections:
//
//  General - This section contains programming constructs that apply generally
//            throughout the entire API. For example, a data structure that
//            enables the user to configure service connection parameters is in
//            this section.
//
//  Control - This section contains programming constructs that enable users to
//            manipulate service API resources, but more generally, other
//            control plane operations.
//
//  Packet  - This section contains programming constructs that enable its users
//            to construct the elementary data unit of transfer: the `Packet`.
//            For example, it contains methods to straightforwardly marshal and
//            unmarshal C++ types to and from `Packet`s.
//
//  Basic IO - This section contains programming constructs that enable its
//             users to send and receive `Packet`s to Vertex AI Vision.
//
//  Media IO  - This section contains programming constructs that enable its
//              users to easily perform I/O between media of various kinds and
//              Vertex AI Vision; e.g. ingesting video files, rtsp into streams.
//
//  See the tutorial directory for some code samples.

#ifndef THIRD_PARTY_VISIONAI_PUBLIC_STREAMS_H_
#define THIRD_PARTY_VISIONAI_PUBLIC_STREAMS_H_

#include <memory>
#include <utility>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/client/control.h"
#include "visionai/streams/client/event_update_receiver.h"
#include "visionai/streams/client/packet_receiver.h"
#include "visionai/streams/client/packet_sender.h"
#include "visionai/streams/packet/packet.h"

namespace visionai {

/// @defgroup general General
/// @brief Constructs applicable across all API groups.

/// @addtogroup general
/// @{

// ----------------------------------------------------------------------------
// General
// ----------------------------------------------------------------------------

/// @brief ServiceConnectionOptions is a structure that contains options to
///        connect to the Vertex AI Vision service.
///
/// For example, if you have a Vision AI cluster created at
///
/// `visionai.googleapis.com/projects/my-project/locations/us-central1-a/clusters/my-cluster`
///
/// then you will set the following:
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// service_endpoint = "visionai.googleapis.com"
/// project_id = "my-project"
/// location_id = "us-central1-a"
/// cluster_id = "my-cluster"
/// ~~~~~~~~~~~~~~~
///
struct ServiceConnectionOptions {
  /// @brief The service endpoint of Vertex AI Vision.
  ///
  /// Most commonly, this is simply visionai.googleapis.com.
  ///
  std::string service_endpoint;

  /// @brief The Google Cloud project-id.
  std::string project_id;

  /// @brief The Google Cloud location-id.
  std::string location_id;

  /// @brief The specific cluster-id of Vertex AI Vision.
  ///
  /// Clusters are automatically created the first time you deploy a Vertex AI
  /// Vision application.
  ///
  std::string cluster_id;
};

/// @}

/// @brief MotionFilterOptions is a structure that contains options to config
///        the motion filter.
struct MotionFilterOptions {
  std::string cool_down_period;
  std::string min_event_length;
  std::string motion_detection_sensitivity;
  std::string lookback_window;
};

/// @}

/// @defgroup control Control
/// @brief Constructs for controlling service resources.

/// @addtogroup control
/// @{

// ----------------------------------------------------------------------------
// Control
// ----------------------------------------------------------------------------

/// @brief Creates a new stream resource with id `stream_id`.
///
/// @param option Connection options.
/// @param stream_id The id of the stream to connect to.
/// @return OK on success. Otherwise the error indicating the cause.
///
absl::Status CreateStream(const ServiceConnectionOptions& options,
                          absl::string_view stream_id);

/// @brief Deletes the stream with id `stream_id`.
absl::Status DeleteStream(const ServiceConnectionOptions& options,
                          absl::string_view stream_id);
/// @}

/// @brief Add an input stream to an app platform application.
///
/// @param option Connection options.
/// @param stream_id The id of the stream to add.
/// @param application_id The id of the application to add the stream to.
/// @return OK on success. Otherwise the error indicating the cause.
///
absl::Status AddStreamToApplication(const ServiceConnectionOptions& options,
                                    absl::string_view stream_id,
                                    absl::string_view application_id);
/// @}

/// @brief Remove an input stream from an app platform application.
///
/// @param option Connection options.
/// @param stream_id The id of the stream to remove.
/// @param application_id The id of the application to remove the stream from.
/// @return OK on success. Otherwise the error indicating the cause.
///
absl::Status RemoveStreamFromApplication(
    const ServiceConnectionOptions& options,
    absl::string_view stream_id,
    absl::string_view application_id);
/// @}

/// @defgroup packet Packet
/// @brief Constructs for working with packets.

/// @addtogroup packet
/// @{

// ----------------------------------------------------------------------------
// Packet
// ----------------------------------------------------------------------------
//
// This section contains methods to manipulate `Packet`s. Of these, the most
// frequent operation is to convert C++ types to and from `Packet`s.
//
// Example - Marshalling a C++ type to a `Packet`.
//
//   // Suppose you have a C++ string.
//   std::string s = "hello!";
//
//   // Simply move that object in as an argument to ToPacket.
//   auto packet = ToPacket(std::move(s));
//
// Example - Unmarshalling a `Packet` to a C++ type.
//
//   // Suppose you have a `Packet`, perhaps obtained from the StreamReceiver.
//   // Further, you also know that this `Packet` represents a string.
//   Packet p = ...;
//
//   // You simply move the packet in and ask to extract it as a C++ string.
//   auto s = FromPacket<std::string>(std::move(p));
//   if (!s.ok()) {
//     // If the status is not ok, then it is likely that the given `Packet`
//     // does not represent a C++ string. Check the status code and message.
//   } else {
//     // `s` really does hold a C++ string extracted from p. You may now use it
//     // in your program as usual.
//   }

/// @brief ToPacket marshals an object of C++ type `T` into a `Packet`.
///
/// The set of possible C++ types that can be marshalled are listed in
/// visionai/streams/packet/packet_codecs/codec_selector.h
///
template <typename T>
absl::StatusOr<Packet> ToPacket(T&& t);

/// @brief FromPacket attempts to unmarshal a `Packet` into a desired C++ Type.
///
/// An error code will be returned if `p` cannot be extracted as the requested
/// C++ type.
///
template <typename T>
absl::StatusOr<T> FromPacket(Packet p);

/// @}

/// @defgroup basic_io Basic IO
/// @brief Constructs to perform basic Packet I/O.
///
/// @addtogroup basic_io
/// @{

// ----------------------------------------------------------------------------
// Basic IO
// ----------------------------------------------------------------------------

/// @brief `StreamSender` is the client through which users send `Packet`s
///        to a stream.
///
/// Each instance of the `StreamSender` represents a single connection to a
/// specific stream. Once created, the user may repeatedly call `Send` with new
/// `Packet`s that they want to send to the stream.
///
/// Example - Repeatedly sending `Packet`s to a specific stream.
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// // First populate the options.
/// StreamSender::Options options;
/// options.service_connection_options = ...;
/// options.stream_id = "my-stream";
///
/// // Create an instance of the `StreamSender`.
/// auto stream_sender_statusor = StreamSender::Create(options);
/// if (!stream_sender.ok()) {
/// // An error occurred during the setup of the sender.
/// // You can fix the problem and try again.
/// }
/// auto stream_sender = std::move(*stream_sender_statusor);
///
/// // Now you can repeatedly send Packets.
/// while (true) {
///   // Get a new packet from some function or generation mechanism.
///   Packet p = SomeFunctionThatGetsNewPacketsToSend();
///
///   auto status = stream_sender->Send(std::move(p));
///   if (!status.ok()) {
///     // An error occurred.
///     // To retry, you must create a new instance of the sender.
///   }
/// }
///
/// // When there are no more packets to send, remember to destroy the sender.
/// stream_sender->reset();
/// ~~~~~~~~~~~~~~~
///
class StreamSender {
 public:
  /// @brief Options to configure the `StreamSender`.
  struct Options {
    /// @brief REQUIRED: The service endpoint and cluster to connect to.
    ServiceConnectionOptions service_connection_options;

    /// @brief REQUIRED: The resource id of the specific stream to send to.
    std::string stream_id;

    /// @brief OPTIONAL: The specific event id to send to.
    ///
    /// Leave empty for this to be automatically generated.
    ///
    std::string event_id;

    /// @brief OPTIONAL: A name to identify the sender.
    ///
    /// Leave empty for this to be automatically generated.
    ///
    std::string sender_id;
  };

  /// @brief Create a readily usable instance of a `StreamSender`.
  static absl::StatusOr<std::unique_ptr<StreamSender>> Create(const Options&);

  /// @brief Send the given `packet` to a stream.
  ///
  /// Returns OK on success. Otherwise, returns an error status.
  ///
  /// To retry on a case of failure, you must create a new instance of the
  /// `StreamSender` and `Send` through that.
  ///
  /// The first overload blocks until the status of the `Send` is known.
  /// The second overload blocks up to `timeout` before returning CANCELLED.
  ///
  absl::Status Send(Packet packet);

  /// @brief Send the given `packet` to a stream with a `timeout`.
  ///
  /// Returns OK on success. Otherwise, returns an error status.
  ///
  /// To retry on a case of failure, you must create a new instance of the
  /// `StreamSender` and `Send` through that.
  ///
  /// The first overload blocks until the status of the `Send` is known.
  /// The second overload blocks up to `timeout` before returning CANCELLED.
  ///
  absl::Status Send(Packet packet, absl::Duration timeout);

  // Copy-control members.
  //
  // Do not use the constructors directly. Use `Create` instead.
  StreamSender() = default;
  ~StreamSender() = default;
  StreamSender(const StreamSender&) = delete;
  StreamSender& operator=(const StreamSender&) = delete;

 private:
  std::unique_ptr<PacketSender> packet_sender_ = nullptr;
};

/// @brief `StreamReceiver` is the client through which users receive `Packet`s
///        from a stream.
///
/// Each instance of the `StreamReceiver` represents a single connection to a
/// specific stream. Once created, the user may repeatedly call `Receive`.
///
/// Example - Repeatedly receive `Packet`s from a specific stream.
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// // First populate the options.
/// StreamReceiver::Options options;
/// options.service_connection_options = ...;
/// options.stream_id = "my-stream";
///
/// // Create an instance of the `StreamReceiver`.
/// auto stream_receiver_statusor = StreamReceiver::Create(options);
/// if (!stream_receiver.ok()) {
///   // An error occurred during the setup of the receiver.
///   // You can fix the problem and try again.
/// }
/// auto stream_receiver = std::move(*stream_receiver_statusor);
///
/// // Now you can repeatedly receive Packets.
/// while (true) {
///   Packet p;
///   auto status = stream_receiver->Receive(&p);
///   if (!status.ok()) {
///     // An error occurred. If the error is transient, then you can retry
///     // with the same receiver. Otherwise, create a new instance to retry.
///   }
/// }
///
/// // When no more packets are desired, destroy the receiver.
/// stream_receiver->reset();
/// ~~~~~~~~~~~~~~~
///
class StreamReceiver {
 public:
  /// @brief Options to configure the `StreamReceiver`.
  struct Options {
    /// @brief REQUIRED: This specifies the service endpoint and cluster
    ///                  to connect to.
    ServiceConnectionOptions service_connection_options;

    /// @brief REQUIRED: This specifies the specific stream to receive from.
    std::string stream_id;

    /// @brief OPTIONAL: This specifies a specific event to receive from.
    ///
    /// Leave empty to receive from the latest event.
    ///
    std::string event_id;

    /// @brief OPTIONAL: A name to identify the receiver.
    ///
    /// Leave empty to have it be automatically generatead.
    std::string receiver_id;
  };

  /// @brief Create a readily usable instance of a `StreamReceiver`.
  static absl::StatusOr<std::unique_ptr<StreamReceiver>> Create(const Options&);

  /// @brief Receive a `Packet` from a stream.
  ///
  /// Returns OK on success. Otherwise, returns the following possible errors:
  ///
  /// NOT_FOUND: This is only possible with the second overload. It is
  ///            returned when there are no `Packet`s in the server before
  ///            `timeout` has run out. You may immediately retry with the
  ///            same receiver instance.
  ///
  /// OUT_OF_RANGE: This indicates that the last packet in the event has been
  ///               delivered. You can create a new `StreamReceiver` to read
  ///               from a different event.
  ///
  /// Other error codes: To retry, create a new instance of the
  ///                    `StreamReceiver` and `Receive` through that.
  ///
  /// The first overload blocks until either there is a new `Packet` or until a
  /// non-transient error has occurred.
  ///
  /// The second overload blocks up to `timeout` before returning NOT_FOUND.
  /// Otherwise, either yields a `Packet` or returns a non-transient error.
  ///
  absl::Status Receive(Packet* packet);
  absl::Status Receive(absl::Duration timeout, Packet* packet);

  // Copy-control members.
  //
  // Do not use the constructors directly. Use `Create` instead.
  StreamReceiver() = default;
  ~StreamReceiver() = default;
  StreamReceiver(const StreamReceiver&) = delete;
  StreamReceiver& operator=(const StreamReceiver&) = delete;

 private:
  ClusterSelection cluster_selection_;
  std::string stream_id_;
  std::string receiver_id_;
  std::string event_id_;

  std::unique_ptr<EventUpdateReceiver> event_update_receiver_ = nullptr;
  std::unique_ptr<PacketReceiver> packet_receiver_ = nullptr;

  absl::Status GetFirstEvent(absl::Duration timeout);
};

/// @}

/// @defgroup media_io Media IO
/// @brief Constructs to perform media IO.
///
/// @addtogroup media_io
/// @{

// ----------------------------------------------------------------------------
// Media IO
// ----------------------------------------------------------------------------

/// @brief Ingests an MP4 video file on the local file system named `file_name`
///        into a stream of id `stream_id`.
///
/// This call blocks until the file is fully ingested.
///
/// The file is ingested at the rate of its playback. For example, if the
/// playback duration of the video is 1 minute, then this function will take
/// 1 minute to complete.
///
/// Returns OK on success; otherwise, an error message telling why the ingestion
/// could not complete successfully.
///
absl::Status IngestMp4(const ServiceConnectionOptions& options,
                       absl::string_view stream_id,
                       absl::string_view file_name);

/// @brief Ingests an RTSP camera endpoint `rtsp_url` into a stream of id
///        `stream_id`.
///
/// This call blocks and ingests the RTSP feed indefinitely. It may complete if
/// the RTSP feed reaches the end, or is otherwise terminated for any reason.
///
/// Returns OK on success; otherwise, an error message telling why the ingestion
/// could not complete successfully.
///
absl::Status IngestRtsp(const ServiceConnectionOptions& options,
                        absl::string_view stream_id,
                        absl::string_view rtsp_url);

/// @}

/// @brief Ingests an MP4 video file on the local file system named `file_name`
///        into a stream of id `stream_id`, with motion filter, in loop mode.
///
/// Returns OK on success; otherwise, an error message telling why the ingestion
/// could not complete successfully.
///
absl::Status IngestMotion(const ServiceConnectionOptions& options,
                       absl::string_view stream_id,
                       absl::string_view event_id,
                       absl::string_view file_name,
                       const MotionFilterOptions& motion_options);

/// @}

//------------------------------------------------------------------------------
// End of public interfaces.
// Implementation details follow.
//------------------------------------------------------------------------------

template <typename T>
absl::StatusOr<Packet> ToPacket(T&& t) {
  return MakePacket(std::forward<T>(t));
}

template <typename T>
absl::StatusOr<T> FromPacket(Packet p) {
  PacketAs<T> packet_as(std::move(p));
  if (packet_as.ok()) {
    return *std::move(packet_as);
  } else {
    return packet_as.status();
  }
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_PUBLIC_STREAMS_H_
