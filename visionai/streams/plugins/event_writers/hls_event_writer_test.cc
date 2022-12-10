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

#include "visionai/streams/plugins/event_writers/hls_event_writer.h"

#include <map>
#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/status/status.h"
#include "absl/strings/substitute.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/gstreamer_registry.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/hls/playlist_m3u8.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/file_path.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

using ::google::protobuf::TextFormat;

constexpr char kTestLocalDir[] = "some-local-dir";
constexpr char kTestMaxFiles[] = "1";
constexpr char kTestTargetDurationInSec[] = "1";
constexpr char kTestStreamId[] = "some-stream-id";
constexpr char kTestConfigTemplate[] = R"pb(
  name: "HLSEventWriter"
  attr { key: "local_dir" value: "$0" }
  attr { key: "max_files" value: "$1" }
  attr { key: "target_duration_in_sec", value: "$2"}
  attr { key: "stream_id", value: "$3"}
)pb";

extern "C" {
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
}

class HLSEventWriterTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    auto status = GstRegisterPlugins();
    ASSERT_TRUE(status.ok());
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
  }

  void WritePackets(std::shared_ptr<HLSEventWriter> writer) {
    GstreamerRunner::Options options;
    options.processing_pipeline_string =
        "videotestsrc num-buffers=300 is-live=true";
    options.receiver_callback =
        [&writer](GstreamerBuffer buffer) -> absl::Status {
      VAI_ASSIGN_OR_RETURN(Packet p, MakePacket(buffer));
      return writer->Write(p);
    };
    auto runner = GstreamerRunner::Create(options).value();
    runner->WaitUntilCompleted(absl::Seconds(3));
  }

  std::vector<streams_internal::HLSSegment> GetHLSSegments(
      const std::string& path) {
    std::string playlist_content;
    GetFileContents(path, &playlist_content).IgnoreError();
    return streams_internal::ParsePlaylistSegments(playlist_content);
  }
};

TEST_F(HLSEventWriterTest, DirectoryNotExist) {
  HLSEventWriter::Options options;
  options.local_dir =
      file::JoinPath(testing::TempDir(), "hls-storage/does-not-exist");
  std::shared_ptr<HLSEventWriter> writer =
      std::make_shared<HLSEventWriter>(options);
  auto status = writer->Open("event-0");
  ASSERT_FALSE(status.ok());
  EXPECT_TRUE(absl::IsNotFound(status));
}

TEST_F(HLSEventWriterTest, DirectoryStrLengthTooLong) {
  HLSEventWriter::Options options;
  std::string long_string(256, 'a');
  options.local_dir =
      file::JoinPath(testing::TempDir(), "hls-storage/", long_string);
  std::shared_ptr<HLSEventWriter> writer =
      std::make_shared<HLSEventWriter>(options);
  auto status = writer->Open("event-0");
  ASSERT_FALSE(status.ok());
  EXPECT_TRUE(absl::IsInvalidArgument(status));
}

TEST_F(HLSEventWriterTest, DirectoryStrInvalidChar) {
  HLSEventWriter::Options options;
  std::string invali_char = "a!b";
  options.local_dir =
      file::JoinPath(testing::TempDir(), "hls-storage/", invali_char);
  std::shared_ptr<HLSEventWriter> writer =
      std::make_shared<HLSEventWriter>(options);
  auto status = writer->Open("event-0");
  ASSERT_FALSE(status.ok());
  EXPECT_TRUE(absl::IsInvalidArgument(status));
}

TEST_F(HLSEventWriterTest, Init) {
  std::shared_ptr<HLSEventWriter> writer =
      std::make_shared<HLSEventWriter>();
  EventWriterConfig config;
  EXPECT_TRUE(TextFormat::ParseFromString(
      absl::Substitute(kTestConfigTemplate, kTestLocalDir, kTestMaxFiles,
                       kTestTargetDurationInSec, kTestStreamId),
      &config));
  VAI_ASSERT_OK_AND_ASSIGN(auto context, EventWriterInitContext::Create(config));
  ASSERT_TRUE(writer->Init(context.get()).ok());
}

TEST_F(HLSEventWriterTest, Write) {
  HLSEventWriter::Options options;
  options.local_dir = file::JoinPath(testing::TempDir(), "hls-storage");
  options.target_duration_in_sec = 1;
  CreateDir(options.local_dir).IgnoreError();
  std::shared_ptr<HLSEventWriter> writer =
      std::make_shared<HLSEventWriter>(options);
  auto status = writer->Open("event-0");

  std::string playlist_path =
      file::JoinPath(options.local_dir, "playlist.m3u8");
  auto segments = GetHLSSegments(playlist_path);
  ASSERT_TRUE(segments.empty());

  WritePackets(writer);

  segments = GetHLSSegments(playlist_path);
  ASSERT_TRUE(!segments.empty());
  EXPECT_EQ(segments[0].name, "segment00000.ts");
  std::string master_playlist_path =
      file::JoinPath(options.local_dir, "master.m3u8");
  std::string segment00000_path =
      file::JoinPath(options.local_dir, "segment00000.ts");
  EXPECT_TRUE(FileExists(master_playlist_path).ok());
  EXPECT_TRUE(FileExists(segment00000_path).ok());

  ASSERT_TRUE(writer->Close().ok());
  EXPECT_FALSE(FileExists(master_playlist_path).ok());
  EXPECT_FALSE(FileExists(segment00000_path).ok());
  EXPECT_FALSE(FileExists(playlist_path).ok());
}

}  // namespace visionai
