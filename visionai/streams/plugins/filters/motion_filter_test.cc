// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/filters/motion_filter.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "absl/strings/string_view.h"
#include "visionai/streams/filtered_element.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/file_path.h"

namespace visionai {

namespace {

const char kTestFolder[] = "visionai/testing/testdata/media/motion";
constexpr int kNumTestImages = 20;

absl::StatusOr<RawImage> CvMatToRawImage(cv::Mat cv_mat) {
  if (cv_mat.channels() != 3) {
    LOG(ERROR) << "cv_mat should have 3 channels instead of "
               << cv_mat.channels() << ".";
  }
  RawImage raw_image(cv_mat.rows, cv_mat.cols, RawImage::Format::kSRGB);
  uint8_t* p_raw_image = raw_image.data();
  for (int r = 0; r < cv_mat.rows; ++r) {
    const uint8_t* p_cv_mat =
        cv_mat.ptr<uchar>(r);  // point to first pixel in row.
    for (int c = 0; c < cv_mat.cols; ++c) {
      *p_raw_image++ = p_cv_mat[2];
      *p_raw_image++ = p_cv_mat[1];
      *p_raw_image++ = p_cv_mat[0];
      p_cv_mat += 3;
    }
  }
  return std::move(raw_image);
}

absl::StatusOr<RawImage> LoadRawImage(const std::string& image_path) {
  RawImage raw_image;
  cv::Mat cv_mat = cv::imread(image_path);
  return CvMatToRawImage(cv_mat);
}

// Count Packet-type elements in the buffer.
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

}  // namespace

TEST(MotionFilterTest, TestRunNoMotion) {
  MotionFilter filter;

  FilterInitContext::InitData init_data;
  init_data.attrs["background_history_frame_length"].set_i(5);
  init_data.attrs["variance_threshold_num_pix"].set_f(16);
  init_data.attrs["shadow_detection"].set_b(false);
  init_data.attrs["scale"].set_f(0.1);
  init_data.attrs["motion_foreground_pixel_threshold"].set_i(120);
  init_data.attrs["motion_area_threshold"].set_f(1.0);
  init_data.attrs["time_out_in_ms"].set_i(1);
  init_data.attrs["min_event_length_in_seconds"].set_i(1);
  init_data.attrs["lookback_window_in_seconds"].set_i(1);
  init_data.attrs["cool_down_period_in_seconds"].set_i(0);
  FilterInitContext init_context(init_data);
  EXPECT_TRUE(filter.Init(&init_context).ok());

  std::shared_ptr<RingBuffer<Packet>> input_buffer =
      std::make_shared<RingBuffer<Packet>>(kNumTestImages);
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer =
      std::make_shared<ProducerConsumerQueue<FilteredElement>>(kNumTestImages);
  FilterRunContext::RunData run_data;
  run_data.input_buffer = input_buffer;
  run_data.output_buffer = output_buffer;
  EventManager::Options event_manager_options;
  event_manager_options.config.set_name("MockEventWriter");
  run_data.event_manager =
      std::make_unique<EventManager>(event_manager_options);

  for (int i = 0; i < kNumTestImages; ++i) {
    std::string filename =
        absl::StrCat("frame01", absl::StrFormat("%02d", i), ".jpg");
    absl::StatusOr<RawImage> raw_image =
        LoadRawImage(file::JoinPath(kTestFolder, filename));
    ASSERT_TRUE(raw_image.ok());
    absl::StatusOr<Packet> p = MakePacket(std::move(*raw_image));
    ASSERT_TRUE(p.ok());
    run_data.input_buffer->EmplaceFront(std::move(*p));
  }

  FilterRunContext run_context(std::move(run_data));
  // Expects error caused by polling timeout.
  EXPECT_FALSE(filter.Run(&run_context).ok());
  // No motion is detected because motion_area_threshold = 1.0, so all images
  // are filtered out.
  EXPECT_EQ(CountPackets(output_buffer), 0);
  EXPECT_EQ(input_buffer->count(), 0);
}

TEST(MotionFilterTest, TestRunMotion) {
  MotionFilter filter;

  FilterInitContext::InitData init_data;
  init_data.attrs["background_history_frame_length"].set_i(5);
  init_data.attrs["variance_threshold_num_pix"].set_f(16);
  init_data.attrs["shadow_detection"].set_b(false);
  init_data.attrs["scale"].set_f(0.1);
  init_data.attrs["motion_foreground_pixel_threshold"].set_i(120);
  init_data.attrs["motion_area_threshold"].set_f(0);
  init_data.attrs["time_out_in_ms"].set_i(1);
  init_data.attrs["min_event_length_in_seconds"].set_i(1);
  init_data.attrs["lookback_window_in_seconds"].set_i(1);
  init_data.attrs["cool_down_cool_down_period_in_secondsperiod"].set_i(0);
  FilterInitContext init_context(init_data);
  EXPECT_TRUE(filter.Init(&init_context).ok());

  std::shared_ptr<RingBuffer<Packet>> input_buffer =
      std::make_shared<RingBuffer<Packet>>(kNumTestImages);
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer =
      std::make_shared<ProducerConsumerQueue<FilteredElement>>(kNumTestImages +
                                                               1);
  FilterRunContext::RunData run_data;
  run_data.input_buffer = input_buffer;
  run_data.output_buffer = output_buffer;
  EventManager::Options event_manager_options;
  event_manager_options.config.set_name("MockEventWriter");
  run_data.event_manager =
      std::make_unique<EventManager>(event_manager_options);

  for (int i = 0; i < kNumTestImages; ++i) {
    std::string filename =
        absl::StrCat("frame01", absl::StrFormat("%02d", i), ".jpg");
    absl::StatusOr<RawImage> raw_image =
        LoadRawImage(file::JoinPath(kTestFolder, filename));
    ASSERT_TRUE(raw_image.ok());
    absl::StatusOr<Packet> p = MakePacket(std::move(*raw_image));
    ASSERT_TRUE(p.ok());
    run_data.input_buffer->EmplaceFront(std::move(*p));
  }

  FilterRunContext run_context(std::move(run_data));
  // Expects error caused by polling timeout.
  EXPECT_FALSE(filter.Run(&run_context).ok());
  // Motion detected because motion_area_threshold = 0.0, so all images are
  // passed through.
  EXPECT_EQ(CountPackets(output_buffer), kNumTestImages);
  EXPECT_EQ(input_buffer->count(), 0);
}

}  // namespace visionai
