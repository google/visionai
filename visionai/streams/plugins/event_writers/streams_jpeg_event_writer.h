// Copyright 2022 Google LLC
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_STREAMS_JPEG_EVENT_WRITER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_STREAMS_JPEG_EVENT_WRITER_H_

#include <memory>

#include "absl/status/status.h"
#include "visionai/algorithms/media/gstreamer_async_jpeg_encoder.h"
#include "visionai/streams/client/packet_sender.h"
#include "visionai/streams/framework/event_writer.h"

namespace visionai {

// The StreamsJPEGEventWriter writes jpeg packets into a specific event in the
// Vision AI Stream.
class StreamsJPEGEventWriter : public EventWriter {
 public:
  StreamsJPEGEventWriter() {}

  // Used by tests only to inject the mock.
  StreamsJPEGEventWriter(std::shared_ptr<PacketSender> sender)
      : sender_(sender) {}

  // Used by tests only to inject the mock.
  StreamsJPEGEventWriter(std::string event_id,
                         std::shared_ptr<PacketSender> sender)
      : event_id_(event_id), sender_(sender) {}

  ~StreamsJPEGEventWriter() override {}

  // Initialize the EventWriter with EventWriterInitConext.
  absl::Status Init(EventWriterInitContext* ctx) override;

  // Open the EventWriter for the new event.
  absl::Status Open(absl::string_view event_id) override;

  // Write a packet.
  absl::Status Write(Packet p) override;

  // Close the current event.
  absl::Status Close() override;

 private:
  std::string event_id_;
  std::string stream_id_;
  std::string sender_name_;
  std::string local_dir_;
  ClusterSelection cluster_selection_;
  std::shared_ptr<PacketSender> sender_;
  std::unique_ptr<GstreamerAsyncJpegEncoder<Packet>>
      gstreamer_jpeg_encoder_ = nullptr;

  absl::StatusOr<std::string> GetLocalFilePath();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_STREAMS_JPEG_EVENT_WRITER_H_
