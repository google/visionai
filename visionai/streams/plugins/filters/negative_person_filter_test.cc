/*
 * Copyright 2022 Google LLC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "visionai/streams/plugins/filters/negative_person_filter.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/imgcodecs.hpp"
#include "absl/strings/string_view.h"
#include "visionai/streams/filtered_element.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/event_writer.h"
#include "visionai/streams/framework/event_writer_def_registry.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/file_path.h"
#include "util/task/contrib/status_macros/ret_check.h"

namespace visionai {

namespace {
using absl::StatusOr;

constexpr absl::string_view kImageFolder =
    "visionai/testing/testdata/media/person";
constexpr absl::string_view kGraphPath =
    "visionai/testing/testdata/models/person/"
    "frozen_inference_graph_rcnn_inception_resnet.pb";
constexpr absl::string_view kLabelMapPath =
    "visionai/testing/testdata/models/person/"
    "person_only_label_map_rcnn_inception_resnet.pbtxt";

StatusOr<RawImage> CvMatToRawImage(cv::Mat cv_mat) {
  RET_CHECK_EQ(cv_mat.channels(), 3)
      << "cv_mat should have 3 channels instead of " << cv_mat.channels()
      << ".";
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
  return raw_image;
}

StatusOr<RawImage> LoadRawImage(const std::string& image_path) {
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
    event_id_ = event_id;
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

TEST(NegativePersonFilterTest, TestWorkingEndWithNoInputsError) {
  // TODO: Use a mock detector to test negative person filter.
  NegativePersonFilter filter;

  FilterInitContext::InitData init_data;
  init_data.attrs["graph_path"].set_s(kGraphPath);
  init_data.attrs["label_map_path"].set_s(kLabelMapPath);
  init_data.attrs["input_layer_name"].set_s("image_tensor");
  init_data.attrs["output_layer_names"].set_s(
      "detection_boxes,detection_scores,detection_classes,num_detections");
  init_data.attrs["max_detections_to_output"].set_i(500);
  init_data.attrs["min_score_thresh_to_output"].set_f(0.1);
  init_data.attrs["time_out_ms"].set_f(1.0);
  FilterInitContext init_context(init_data);
  EXPECT_TRUE(filter.Init(&init_context).ok());

  const int kCapacity = 10;
  std::shared_ptr<RingBuffer<Packet>> input_buffer =
      std::make_shared<RingBuffer<Packet>>(kCapacity);
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer =
      std::make_shared<ProducerConsumerQueue<FilteredElement>>(kCapacity);
  FilterRunContext::RunData run_data;
  run_data.input_buffer = input_buffer;
  run_data.output_buffer = output_buffer;
  EventManager::Options event_manager_options;
  event_manager_options.config.set_name("MockEventWriter");
  run_data.event_manager =
      std::make_unique<EventManager>(event_manager_options);

  StatusOr<RawImage> raw_image_wo_person =
      LoadRawImage(file::JoinPath(kImageFolder, "image_wo_person.jpg"));
  ASSERT_TRUE(raw_image_wo_person) << raw_image_wo_person.status(.ok());
  StatusOr<Packet> packet_wo_person =
      MakePacket(std::move(*raw_image_wo_person));
  ASSERT_TRUE(packet_wo_person) << packet_wo_person.status(.ok());
  run_data.input_buffer->EmplaceFront(std::move(*packet_wo_person));

  StatusOr<RawImage> raw_image_w_person =
      LoadRawImage(file::JoinPath(kImageFolder, "image_w_person.jpg"));
  ASSERT_TRUE(raw_image_w_person) << raw_image_w_person.status(.ok());
  StatusOr<Packet> packet_w_person =
      MakePacket(std::move(*raw_image_w_person));
  ASSERT_TRUE(packet_w_person) << packet_w_person.status(.ok());
  run_data.input_buffer->EmplaceFront(std::move(*packet_w_person));

  FilterRunContext run_context(std::move(run_data));
  // Expects error caused by polling timeout.
  EXPECT_THAT(filter.Run(&run_context).code(), absl::StatusCode::kUnavailable);
  // One image with a person detected is filtered from 2 images in total, so in
  // the end only one image is in output_buffer.
  EXPECT_EQ(CountPackets(output_buffer), 1);
  EXPECT_EQ(input_buffer->count(), 0);
}

TEST(NegativePersonFilterTest, TestWorkingInTimeout) {
  NegativePersonFilter filter;

  FilterInitContext::InitData init_data;
  init_data.attrs["graph_path"].set_s(kGraphPath);
  init_data.attrs["label_map_path"].set_s(kLabelMapPath);
  init_data.attrs["input_layer_name"].set_s("image_tensor");
  init_data.attrs["output_layer_names"].set_s(
      "detection_boxes,detection_scores,detection_classes,num_detections");
  init_data.attrs["max_detections_to_output"].set_i(500);
  init_data.attrs["min_score_thresh_to_output"].set_f(0.1);
  init_data.attrs["time_out_ms"].set_f(1.0);
  FilterInitContext init_context(init_data);
  EXPECT_TRUE(filter.Init(&init_context).ok());

  const int kCapacity = 10;
  std::shared_ptr<RingBuffer<Packet>> input_buffer =
      std::make_shared<RingBuffer<Packet>>(kCapacity);
  std::shared_ptr<ProducerConsumerQueue<FilteredElement>> output_buffer =
      std::make_shared<ProducerConsumerQueue<FilteredElement>>(kCapacity);
  FilterRunContext::RunData run_data;
  run_data.input_buffer = input_buffer;
  run_data.output_buffer = output_buffer;
  EventManager::Options event_manager_options;
  event_manager_options.config.set_name("MockEventWriter");
  run_data.event_manager =
      std::make_unique<EventManager>(event_manager_options);

  StatusOr<RawImage> raw_image_wo_person =
      LoadRawImage(file::JoinPath(kImageFolder, "image_wo_person.jpg"));
  ASSERT_TRUE(raw_image_wo_person) << raw_image_wo_person.status(.ok());
  StatusOr<Packet> packet_wo_person =
      MakePacket(std::move(*raw_image_wo_person));
  ASSERT_TRUE(packet_wo_person) << packet_wo_person.status(.ok());
  run_data.input_buffer->EmplaceFront(std::move(*packet_wo_person));

  StatusOr<RawImage> raw_image_w_person =
      LoadRawImage(file::JoinPath(kImageFolder, "image_w_person.jpg"));
  ASSERT_TRUE(raw_image_w_person) << raw_image_w_person.status(.ok());
  StatusOr<Packet> packet_w_person =
      MakePacket(std::move(*raw_image_w_person));
  ASSERT_TRUE(packet_w_person) << packet_w_person.status(.ok());
  run_data.input_buffer->EmplaceFront(std::move(*packet_w_person));

  FilterRunContext run_context(std::move(run_data));

  EXPECT_TRUE(filter.Cancel().ok());
  EXPECT_TRUE(filter.Run(&run_context).ok());
  // One image with a person detected is filtered from 2 images in total, so in
  // the end only one image is in output_buffer.
  EXPECT_EQ(CountPackets(output_buffer), 1);
  EXPECT_EQ(input_buffer->count(), 0);
}

}  // namespace visionai
