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

#ifndef THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_ENCODED_STREAM_LOG_EVENT_WRITER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_ENCODED_STREAM_LOG_EVENT_WRITER_H_

#include "absl/status/status.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/streams/framework/event_writer.h"


namespace visionai {

// The EncodedStreamLogEventWriter logs the received packet.
class EncodedStreamLogEventWriter : public EventWriter {
 public:
  EncodedStreamLogEventWriter() {}

  ~EncodedStreamLogEventWriter() override {}

  absl::Status Init(EventWriterInitContext* ctx) override;

  absl::Status Open(absl::string_view event_id) override;

  absl::Status Write(Packet p) override;

  absl::Status Close() override;

 private:
  std::string event_id_;

  std::unique_ptr<GstreamerRunner> gstreamer_runner_ = nullptr;

  std::string AssembleGstreamerPipeline();
};
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_PLUGINS_EVENT_WRITERS_ENCODED_STREAM_LOG_EVENT_WRITER_H_
