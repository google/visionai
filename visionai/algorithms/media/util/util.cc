// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/util.h"

#include <string>
#include <thread>

#include "glog/logging.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

namespace visionai {

namespace {
std::string print_gerror(GError *gerr) {
  if (gerr == nullptr) {
    return "";
  }
  return absl::StrFormat("%s (%d): %s", g_quark_to_string(gerr->domain),
                         gerr->code, gerr->message);
}
}  // namespace

absl::Status GstInit() {
  GError *gerr = NULL;
  if (gst_init_check(nullptr, nullptr, &gerr) == FALSE) {
    auto s = absl::InternalError(print_gerror(gerr));
    g_clear_error(&gerr);
    return s;
  }
  return absl::OkStatus();
}

absl::Status GstLaunchPipeline(const std::string &gst_pipeline,
                               int play_duration_in_seconds) {
  // Initialize gstreamer if requested.
  auto status = GstInit();
  if (!status.ok()) {
    LOG(ERROR) << status;
    return absl::InternalError("Could not initialize Gstreamer");
  }

  // Build the pipeline.
  GError *gerr = nullptr;
  GstElement *pipeline = gst_parse_launch(gst_pipeline.c_str(), &gerr);
  if (gerr != nullptr) {
    auto s = absl::InternalError(print_gerror(gerr));
    g_clear_error(&gerr);
    return s;
  }

  // Play the pipeline.
  if (gst_element_set_state(pipeline, GST_STATE_PLAYING) ==
      GST_STATE_CHANGE_FAILURE) {
    return absl::InternalError("Failed to start playing the pipeline");
  }

  // Run a background thread to send an EOS when the duration bound expires.
  // TODO: consider adding a SIGINT handler to trigger EOS.
  if (play_duration_in_seconds >= 0) {
    std::thread stopper([pipeline, play_duration_in_seconds]() {
      absl::SleepFor(absl::Seconds(play_duration_in_seconds));
      gst_element_send_event(pipeline, gst_event_new_eos());
    });
    stopper.detach();
  }

  // Observe the message bus for when GST_MESSAGE_ERROR or GST_MESSAGE_EOS
  // shows up so that we can stop the pipeline.
  GstBus *bus = gst_element_get_bus(pipeline);
  if (bus == nullptr) {
    return absl::InternalError("Failed to get the message bus");
  }
  GstMessage *msg = gst_bus_timed_pop_filtered(
      bus, GST_CLOCK_TIME_NONE,
      static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  // Stop the pipeline and clean up.
  if (msg != nullptr) {
    gst_message_unref(msg);
  }
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  return absl::OkStatus();
}

absl::Status GstLaunchPipeline(const std::string &gst_pipeline) {
  return GstLaunchPipeline(gst_pipeline, -1);
}

}  // namespace visionai
