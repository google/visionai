// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/filters/encoded_motion_filter.h"

#include "google/protobuf/util/time_util.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/debugging/leak_check.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/streams/filtered_element.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/file_path.h"

namespace visionai {

class EncodedMotionFilterTestPeer {
 public:
  static absl::Duration GetTotalTimeFiltered(EncodedMotionFilter &filter) {
    return filter.GetTotalTimeFiltered();
  }
};

namespace {

using google::protobuf::util::TimeUtil;

const char kTestFolder[] =
    "visionai/testing/testdata/media/encoded-frames";
// We have 6 frames in total:
// 1st frame:     key frame
// 2nd frame: non-key frame
// 3rd frame: non-key frame
// 4th frame: non-key frame
// 5th frame: non-key frame
// 6th frame: non-key frame
constexpr int kNumTestFrames = 6;

constexpr absl::string_view kH264CapsString =
    "video/x-h264, stream-format=(string)avc, alignment=(string)au, "
    "level=(string)1.3, profile=(string)high, "
    "codec_data=(buffer)"
    "0164000dffe100186764000dacd94161fb016c80000003008000001e078a14cb01000668eb"
    "e3cb22c0, width=(int)352, height=(int)240, framerate=(fraction)30/1, "
    "pixel-aspect-ratio=(fraction)1/1, chroma-format=(string)4:2:0, "
    "bit-depth-luma=(uint)8, bit-depth-chroma=(uint)8, parsed=(boolean)true";
constexpr absl::Duration kMotionDecoderTimeout = absl::Seconds(1);

extern "C" {
GST_PLUGIN_STATIC_DECLARE(app);
GST_PLUGIN_STATIC_DECLARE(videotestsrc);
GST_PLUGIN_STATIC_DECLARE(coreelements);
GST_PLUGIN_STATIC_DECLARE(playback);
GST_PLUGIN_STATIC_DECLARE(jpeg);
GST_PLUGIN_STATIC_DECLARE(videoconvert);
GST_PLUGIN_STATIC_DECLARE(libav);
GST_PLUGIN_STATIC_DECLARE(x264);
}

class EncodedMotionFilterTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    absl::LeakCheckDisabler disabler;
    auto status = GstInit();
    ASSERT_TRUE(status.ok()) << status;
    GST_PLUGIN_STATIC_REGISTER(app);
    GST_PLUGIN_STATIC_REGISTER(videotestsrc);
    GST_PLUGIN_STATIC_REGISTER(coreelements);
    GST_PLUGIN_STATIC_REGISTER(playback);
    GST_PLUGIN_STATIC_REGISTER(jpeg);
    GST_PLUGIN_STATIC_REGISTER(videoconvert);
    GST_PLUGIN_STATIC_REGISTER(libav);
    GST_PLUGIN_STATIC_REGISTER(x264);
  }
};

absl::StatusOr<GstreamerBuffer> LoadGstreamerBuffer(absl::string_view file_path,
                                                    bool is_key_frame,
                                                    int64_t dts) {
  GstreamerBuffer gstreamer_buffer;
  std::string file_contents;
  VAI_RETURN_IF_ERROR(GetFileContents(file_path, &file_contents));
  gstreamer_buffer.assign(file_contents.data(), file_contents.size());
  gstreamer_buffer.set_caps_string(kH264CapsString);
  gstreamer_buffer.set_is_key_frame(is_key_frame);
  gstreamer_buffer.set_dts(dts);
  return gstreamer_buffer;
}

// Counts Packet-type elements in the buffer.
int CountPackets(
    std::shared_ptr<ProducerConsumerQueue<FilteredElement>> buffer) {
  int cnt = 0;
  while (buffer->count() > 0) {
    FilteredElement elem;
    buffer->Pop(elem);
    if (elem.type() == FilteredElementType::kPacket) {
      cnt++;
    }
  }
  return cnt;
}

// Gets the DTS from each packet in the buffer.
std::vector<int64_t> GetPacketsDTS(
    std::shared_ptr<ProducerConsumerQueue<FilteredElement>> buffer) {
  std::vector<int64_t> dts;
  while (buffer->count() > 0) {
    FilteredElement elem;
    buffer->Pop(elem);
    if (elem.type() == FilteredElementType::kPacket) {
      std::unique_ptr<Packet> packet = std::move(elem).ReleasePacket();
      dts.push_back(
          TimeUtil::TimestampToNanoseconds(packet->header().capture_time()));
    }
  }
  return dts;
}

// A Mock Event Writer to for unit tests.
class MockEventWriter : public EventWriter {
 public:
  MockEventWriter() {}
  ~MockEventWriter() override {}

  absl::Status Init(EventWriterInitContext* ctx) override {
    return absl::OkStatus();
  }

  absl::Status Open(absl::string_view event_id) override {
    event_id_ = std::string(event_id);
    return absl::OkStatus();
  }

  absl::Status Write(Packet p) override { return absl::OkStatus(); }
  absl::Status Close() override { return absl::OkStatus(); }

 private:
  std::string event_id_;
  std::string catch_phrase_;
};

REGISTER_EVENT_WRITER_INTERFACE("MockEventWriter")
    .Doc(R"doc(MockEventWriter)doc");
REGISTER_EVENT_WRITER_IMPLEMENTATION("MockEventWriter", MockEventWriter);

// TODO(yukunma): Add more test cases.
TEST_F(EncodedMotionFilterTest, TestRunNoMotion) {
  EncodedMotionFilter filter;
  EncodedMotionFilterTestPeer filter_peer;

  FilterInitContext::InitData init_data;
  init_data.attrs["spatial_grid_number"].set_i(3);
  init_data.attrs["temporal_buffer_frames"].set_i(1);
  init_data.attrs["motion_detection_sensitivity"].set_s("medium");
  // Sets `min_frames_trigger_motion` to 100 to avoid starting new events.
  init_data.attrs["min_frames_trigger_motion"].set_i(100);
  init_data.attrs["min_event_length_in_seconds"].set_i(1);
  init_data.attrs["cool_down_period_in_seconds"].set_i(0);
  init_data.attrs["lookback_window_in_seconds"].set_i(3);
  init_data.attrs["time_out_in_ms"].set_i(10);
  FilterInitContext init_context(init_data);
  EXPECT_TRUE(filter.Init(&init_context).ok());

  std::shared_ptr<RingBuffer<Packet>> input_buffer =
      std::make_shared<RingBuffer<Packet>>(kNumTestFrames);
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer =
      std::make_shared<ProducerConsumerQueue<FilteredElement>>(kNumTestFrames);
  FilterRunContext::RunData run_data;
  run_data.input_buffer = input_buffer;
  run_data.output_buffer = output_buffer;
  EventManager::Options event_manager_options;
  event_manager_options.config.set_name("MockEventWriter");
  run_data.event_manager =
      std::make_unique<EventManager>(event_manager_options);

  for (int i = 1; i < kNumTestFrames; ++i) {
    std::string filename = absl::StrFormat("shadow-cubicle-frame-%d.264", i);
    // Only the first frame is a key frame;
    bool is_key_frame = i == 1;
    absl::StatusOr<GstreamerBuffer> gstreamer_buffer = LoadGstreamerBuffer(
        file::JoinPath(kTestFolder, filename), is_key_frame, /*dts=*/i);
    ASSERT_TRUE(gstreamer_buffer.ok());
    absl::StatusOr<Packet> p = MakePacket(std::move(*gstreamer_buffer));
    ASSERT_TRUE(p.ok());
    run_data.input_buffer->EmplaceFront(std::move(*p));
  }

  FilterRunContext run_context(std::move(run_data));
  // Expects error caused by polling timeout.
  ASSERT_FALSE(filter.Run(&run_context).ok());
  ASSERT_TRUE(filter.Cancel().ok());
  ASSERT_TRUE(filter.WaitUntilCompleted(kMotionDecoderTimeout));
  // No new events are created because `min_frames_trigger_motion` is set to
  // 100, so all images are filtered out.
  EXPECT_EQ(CountPackets(output_buffer), 0);
  EXPECT_EQ(input_buffer->count(), 0);

  // The total time filtered is 0 rather than 6 since the logging works by
  // counting the number of frames popped from the bugger. These 6 frames
  // are not popped when the filter closes.
  EXPECT_EQ(filter_peer.GetTotalTimeFiltered(filter), absl::Nanoseconds(0));
}

TEST_F(EncodedMotionFilterTest, TestRunMotion) {
  EncodedMotionFilter filter;
  EncodedMotionFilterTestPeer filter_peer;

  FilterInitContext::InitData init_data;
  init_data.attrs["spatial_grid_number"].set_i(3);
  init_data.attrs["temporal_buffer_frames"].set_i(1);
  init_data.attrs["motion_detection_sensitivity"].set_s("high");
  init_data.attrs["min_frames_trigger_motion"].set_i(1);
  init_data.attrs["min_event_length_in_seconds"].set_i(1);
  init_data.attrs["cool_down_period_in_seconds"].set_i(0);
  init_data.attrs["lookback_window_in_seconds"].set_i(3);
  init_data.attrs["time_out_in_ms"].set_i(10);
  FilterInitContext init_context(init_data);
  EXPECT_TRUE(filter.Init(&init_context).ok());

  std::shared_ptr<RingBuffer<Packet>> input_buffer =
      std::make_shared<RingBuffer<Packet>>(kNumTestFrames);
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer =
      std::make_shared<ProducerConsumerQueue<FilteredElement>>(kNumTestFrames +
                                                               1);
  FilterRunContext::RunData run_data;
  run_data.input_buffer = input_buffer;
  run_data.output_buffer = output_buffer;
  EventManager::Options event_manager_options;
  event_manager_options.config.set_name("MockEventWriter");
  run_data.event_manager =
      std::make_unique<EventManager>(event_manager_options);

  for (int i = 1; i <= kNumTestFrames; ++i) {
    std::string filename = absl::StrFormat("shadow-cubicle-frame-%d.264", i);
    // Only the first frame is a key frame;
    bool is_key_frame = i == 1;
    absl::StatusOr<GstreamerBuffer> gstreamer_buffer = LoadGstreamerBuffer(
        file::JoinPath(kTestFolder, filename), is_key_frame, /*dts=*/i);
    ASSERT_TRUE(gstreamer_buffer.ok());
    absl::StatusOr<Packet> p = MakePacket(std::move(*gstreamer_buffer));
    ASSERT_TRUE(p.ok());
    run_data.input_buffer->EmplaceFront(std::move(*p));
  }

  FilterRunContext run_context(std::move(run_data));
  // Expects error caused by polling timeout.
  ASSERT_FALSE(filter.Run(&run_context).ok());
  ASSERT_TRUE(filter.Cancel().ok());
  ASSERT_TRUE(filter.WaitUntilCompleted(kMotionDecoderTimeout));
  // Motion detected because `min_frames_trigger_motion` is set to 1, so this
  // GOB will be pushed into the output buffer.
  EXPECT_EQ(CountPackets(output_buffer), kNumTestFrames);
  EXPECT_EQ(input_buffer->count(), 0);
  EXPECT_EQ(filter_peer.GetTotalTimeFiltered(filter), absl::Nanoseconds(0));
}

TEST_F(EncodedMotionFilterTest, TestNotChangeCaptureTime) {
  EncodedMotionFilter filter;

  FilterInitContext::InitData init_data;
  init_data.attrs["spatial_grid_number"].set_i(3);
  init_data.attrs["temporal_buffer_frames"].set_i(1);
  init_data.attrs["motion_detection_sensitivity"].set_s("high");
  init_data.attrs["min_frames_trigger_motion"].set_i(1);
  init_data.attrs["min_event_length_in_seconds"].set_i(1);
  init_data.attrs["cool_down_period_in_seconds"].set_i(0);
  init_data.attrs["lookback_window_in_seconds"].set_i(3);
  init_data.attrs["time_out_in_ms"].set_i(10);
  FilterInitContext init_context(init_data);
  EXPECT_TRUE(filter.Init(&init_context).ok());

  std::shared_ptr<RingBuffer<Packet>> input_buffer =
      std::make_shared<RingBuffer<Packet>>(kNumTestFrames);
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer =
      std::make_shared<ProducerConsumerQueue<FilteredElement>>(kNumTestFrames +
                                                               1);
  FilterRunContext::RunData run_data;
  run_data.input_buffer = input_buffer;
  run_data.output_buffer = output_buffer;
  EventManager::Options event_manager_options;
  event_manager_options.config.set_name("MockEventWriter");
  run_data.event_manager =
      std::make_unique<EventManager>(event_manager_options);

  std::vector<int64_t> expected_dts;
  for (int i = 1; i <= kNumTestFrames; ++i) {
    std::string filename = absl::StrFormat("shadow-cubicle-frame-%d.264", i);
    // Only the first frame is a key frame;
    bool is_key_frame = i == 1;
    absl::StatusOr<GstreamerBuffer> gstreamer_buffer = LoadGstreamerBuffer(
        file::JoinPath(kTestFolder, filename), is_key_frame, /*dts=*/i);
    ASSERT_TRUE(gstreamer_buffer.ok());
    absl::StatusOr<Packet> p = MakePacket(std::move(*gstreamer_buffer));
    ASSERT_TRUE(p.ok());
    expected_dts.push_back(
        TimeUtil::TimestampToNanoseconds(p->header().capture_time()));
    run_data.input_buffer->EmplaceFront(std::move(*p));
  }

  FilterRunContext run_context(std::move(run_data));
  // Expects error caused by polling timeout.
  ASSERT_FALSE(filter.Run(&run_context).ok());
  ASSERT_TRUE(filter.Cancel().ok());
  ASSERT_TRUE(filter.WaitUntilCompleted(kMotionDecoderTimeout));

  std::vector<int64_t> dts = GetPacketsDTS(output_buffer);
  ASSERT_EQ(dts.size(), kNumTestFrames);
  ASSERT_EQ(dts.size(), expected_dts.size());
  for (int i = 0; i < kNumTestFrames; ++i) {
    EXPECT_EQ(dts[i], expected_dts[i]);
  }
}

}  // namespace
}  // namespace visionai
