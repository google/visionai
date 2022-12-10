// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/gstreamer_runner.h"

#include <algorithm>
#include <memory>
#include <thread>
#include <utility>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/synchronization/mutex.h"
extern "C"{
#include "third_party/ffmpeg/libavutil/motion_vector.h"
}
#include "third_party/gstreamer/subprojects/gst_libav/ext/libav/gst-motion-meta.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/app/gstappsink.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstbuffer.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/types/gstreamer_buffer.h"
#include "visionai/util/completion_signal.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {

constexpr char kAppSrcName[] = "feed";
constexpr char kAppSinkName[] = "fetch";
constexpr int kPipelineFinishTimeoutSeconds = 5;

static GstFlowReturn ReturnGstFlowReturn(const absl::Status& status) {
  return status.ok() ? GST_FLOW_OK : GST_FLOW_ERROR;
}

// RAII object that grants a *running* glib main loop.
//
// The event loop is run in a background thread. Gstreamer's Bus mechanism works
// as long as there is some glib main loop running; i.e. it needn't be in the
// main thread.
class GMainLoopManager {
 public:
  GMainLoopManager() {
    glib_main_loop_ = g_main_loop_new(NULL, FALSE);
    glib_main_loop_runner_ =
        std::thread([this]() { g_main_loop_run(glib_main_loop_); });
    while (g_main_loop_is_running(glib_main_loop_) != TRUE)
      ;
  }

  ~GMainLoopManager() {
    g_main_loop_quit(glib_main_loop_);
    glib_main_loop_runner_.join();
    g_main_loop_unref(glib_main_loop_);
  }

  GMainLoopManager(const GMainLoopManager&) = delete;
  GMainLoopManager(GMainLoopManager&&) = delete;
  GMainLoopManager& operator=(const GMainLoopManager&) = delete;

 private:
  GMainLoop* glib_main_loop_ = nullptr;
  std::thread glib_main_loop_runner_;
};

// Callback attached to observe pipeline bus messages.
gboolean gst_bus_message_callback(GstBus* bus, GstMessage* message,
                                  CompletionSignal* signal) {
  GError* err;
  gchar* debug_info;
  switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_EOS:
      signal->End();
      break;
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(message, &err, &debug_info);
      LOG(ERROR) << absl::StrFormat("Error from gstreamer element %s: %s",
                                    GST_OBJECT_NAME(message->src),
                                    err->message);
      LOG(ERROR) << absl::StrFormat("Additional debug info: %s",
                                    debug_info ? debug_info : "none");
      LOG(ERROR) << "Got gstreamer error; shutting down event loop";
      signal->End();
      break;
    default:
      break;
  }
  return TRUE;
}

namespace {

struct NewSampleSignalData {
  GstreamerRunner::ReceiverCallback receiver_callback;
  std::unique_ptr<absl::Mutex> new_sample_mutex;
};

}  // namespace

// Callback for receiving new GstSample's from appsink.
GstFlowReturn on_new_sample_from_sink(GstElement* elt,
                                      NewSampleSignalData* data) {
  // We use the mutex lock to avoid a race condition where the decoder output
  // for time t+1 is passed to the receiver callback before the output for time
  // t.
  //
  // To understand why this lock is adequate: whenever a new sample is ready, a
  // signal is emitted. Upon signal emission, this callback is called. The
  // callback is not supplied the new sample directly. Instead, we make another
  // call in this signal handler:`gst_app_sink_pull_sample' to fetch the sample.
  //
  // Since we have control over when the new sample is fetched (and are not
  // supplied it as a callback parameter), and can assume that each fetch gets
  // the oldest sample, this mutex lock should be adequate in preventing the
  // race condition.
  //
  // Here's an example: suppose t=1 data is ready, and t=2 data is ready
  // immediately after. Even if the signal that was emitted by t=2 triggers the
  // callback first and get the lock, that callback will still pull t=1 data and
  // pass it to the receiver callback, because that's the next data available in
  // the internal queue.
  absl::MutexLock lock(data->new_sample_mutex.get());
  GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(elt));

  // No-op if callbacks are not supplied.
  if (!data->receiver_callback) {
    gst_sample_unref(sample);
    return GST_FLOW_OK;
  }

  // Copy the GstSample into visionai's GstreamerBuffer type.
  GstBuffer* buffer = gst_sample_get_buffer(sample);
  GstCaps* caps = gst_sample_get_caps(sample);

  GstreamerBuffer gstreamer_buffer;

  // Get motion vector meta from GstreamerBuffer
  GstMotionMeta* meta =
      (GstMotionMeta*)gst_buffer_get_meta(buffer, GST_MOTION_META_API_TYPE);
  if (meta != nullptr) {
    // `debug-mv` enabled in the pipeline.
    if (meta->mvs != nullptr && meta->size > 0) {
      gstreamer_buffer.assign(reinterpret_cast<char*>(meta->mvs), meta->size);
    } else {
      // No motion vectors are extracted because, e.g., this is a key frame.
      gstreamer_buffer.assign(nullptr, 0);
    }

    // TODO(b/245757650): Properly set `gstreamer_buffer` with motion vectors.
    // TODO: Find a way to decide whether a buffer is the frame head.
    //       One possibility is to use GST_BUFFER_FLAG_MARKER, but this bit is
    //       not used uniformly across plugins.
    //       Turn it on uniformly for the time being as it is by far the common
    //       case.
    bool is_key_frame =
        !GST_BUFFER_FLAG_IS_SET(buffer, GST_BUFFER_FLAG_DELTA_UNIT);
    gstreamer_buffer.set_is_key_frame(is_key_frame);
  } else {
    // `debug-mv` not enabled in the pipeline.
    VAI_ASSIGN_OR_RETURN(gstreamer_buffer, ToGstreamerBuffer(caps, buffer),
                     _.LogError().With(ReturnGstFlowReturn));
    gstreamer_buffer.set_pts(GST_BUFFER_PTS(buffer));
    gstreamer_buffer.set_dts(GST_BUFFER_DTS(buffer));
    gstreamer_buffer.set_duration(GST_BUFFER_DURATION(buffer));
  }

  gst_sample_unref(sample);

  // Deliver the GstreamerBuffer using the callback.
  // When the callback method return CancelledError(), pause/halt the pipeline.
  absl::Status status = data->receiver_callback(std::move(gstreamer_buffer));
  if (absl::IsCancelled(status)) {
    return GST_FLOW_EOS;
  } else if (!status.ok()) {
    LOG(ERROR) << status.message();
    return GST_FLOW_ERROR;
  }
  return GST_FLOW_OK;
}

absl::Status ValidateOptions(const GstreamerRunner::Options& options) {
  if (options.processing_pipeline_string.empty()) {
    return absl::InvalidArgumentError(
        "Given an empty processing pipeline string");
  }
  return absl::OkStatus();
}

// Object that owns and configures a gstreamer pipeline.
//
// It supports pipelines of the form:
// gst-launch [appsrc !] main-processing-pipeline [! appsink]
//
// appsrc and appsink are added depending on whether appsrc caps and a callback
// is provided in Options.
class GstreamerPipeline {
 public:
  static absl::StatusOr<std::unique_ptr<GstreamerPipeline>> Create(
      const GstreamerRunner::Options& options) {
    auto status = ValidateOptions(options);
    if (!status.ok()) {
      LOG(ERROR) << status;
      return absl::InvalidArgumentError(
          "The given GstreamerRunner::Options has errors");
    }
    auto gstreamer_pipeline = std::make_unique<GstreamerPipeline>();

    // Create the gst_pipeline.
    std::vector<std::string> pipeline_elements;
    if (!options.appsrc_caps_string.empty()) {
      // TODO: might be useful to have an option to configure this string.
      pipeline_elements.push_back(absl::StrFormat(
          "appsrc name=%s is-live=true do-timestamp=%d format=3", kAppSrcName,
          options.appsrc_do_timestamps));
    }

    if (options.processing_pipeline_string.empty()) {
      return absl::InvalidArgumentError(
          "Given an empty processing pipeline string");
    }
    pipeline_elements.push_back(options.processing_pipeline_string);

    if (options.receiver_callback) {
      pipeline_elements.push_back(
          absl::StrFormat("appsink name=%s", kAppSinkName));
    }
    std::string pipeline_string = absl::StrJoin(pipeline_elements, " ! ");

    gstreamer_pipeline->gst_pipeline_ =
        gst_parse_launch(pipeline_string.c_str(), NULL);
    if (gstreamer_pipeline->gst_pipeline_ == nullptr) {
      return absl::InvalidArgumentError(
          absl::StrFormat("Failed to create a gstreamer pipeline using "
                          "\"%s\". Make sure you've "
                          "given a valid processing pipeline string",
                          pipeline_string));
    }

    // Configure the appsrc.
    if (!options.appsrc_caps_string.empty()) {
      gstreamer_pipeline->gst_appsrc_ = gst_bin_get_by_name(
          GST_BIN(gstreamer_pipeline->gst_pipeline_), kAppSrcName);
      if (gstreamer_pipeline->gst_appsrc_ == nullptr) {
        return absl::InternalError(
            "Failed to get a pointer to the appsrc element");
      }
      GstCaps* appsrc_caps =
          gst_caps_from_string(options.appsrc_caps_string.c_str());
      if (appsrc_caps == nullptr) {
        return absl::InvalidArgumentError(absl::StrFormat(
            "Failed to create a GstCaps from \"%s\"; make sure it is a valid "
            "cap string",
            options.appsrc_caps_string));
      }
      g_object_set(G_OBJECT(gstreamer_pipeline->gst_appsrc_), "caps",
                   appsrc_caps, NULL);
      gst_caps_unref(appsrc_caps);
    }

    // Configure the appsink.
    if (options.receiver_callback) {
      gstreamer_pipeline->gst_appsink_ = gst_bin_get_by_name(
          GST_BIN(gstreamer_pipeline->gst_pipeline_), kAppSinkName);
      if (gstreamer_pipeline->gst_appsink_ == nullptr) {
        return absl::InternalError(
            "Failed to get a pointer to the appsink element");
      }
      g_object_set(G_OBJECT(gstreamer_pipeline->gst_appsink_), "emit-signals",
                   TRUE, "sync", options.appsink_sync ? TRUE : FALSE, NULL);

      gstreamer_pipeline->new_sample_data() = {
          .receiver_callback = options.receiver_callback,
          .new_sample_mutex = std::make_unique<absl::Mutex>(),
      };
      g_signal_connect(gstreamer_pipeline->gst_appsink_, "new-sample",
                       G_CALLBACK(on_new_sample_from_sink),
                       const_cast<NewSampleSignalData*>(
                           &gstreamer_pipeline->new_sample_data()));
    }

    return std::move(gstreamer_pipeline);
  }

  GstElement* gst_pipeline() const { return gst_pipeline_; }

  GstElement* gst_appsrc() const { return gst_appsrc_; }

  NewSampleSignalData& new_sample_data() { return new_sample_data_; }

  GstreamerPipeline() = default;
  ~GstreamerPipeline() { Cleanup(); }
  GstreamerPipeline(const GstreamerPipeline&) = delete;
  GstreamerPipeline& operator=(const GstreamerPipeline&) = delete;

 private:
  void Cleanup() {
    if (gst_pipeline_ != nullptr) {
      gst_object_unref(gst_pipeline_);
    }
    if (gst_appsrc_ != nullptr) {
      gst_object_unref(gst_appsrc_);
    }
    if (gst_appsink_ != nullptr) {
      gst_object_unref(gst_appsink_);
    }
  }

  GstElement* gst_pipeline_ = nullptr;
  GstElement* gst_appsrc_ = nullptr;
  GstElement* gst_appsink_ = nullptr;
  NewSampleSignalData new_sample_data_;
};

}  // namespace

// -----------------------------------------------------------------------
// GstreamerRunnerImpl
// A class that manages a running Gstreamer pipeline.
class GstreamerRunner::GstreamerRunnerImpl {
 public:
  // Create a fully initialized and running gstreamer pipeline.
  static absl::StatusOr<std::unique_ptr<GstreamerRunnerImpl>> Create(
      const Options& options);

  // Feed a GstreamerBuffer into the running pipeline.
  absl::Status Feed(const GstreamerBuffer&);

  bool IsCompleted() {
    if (completion_signal_) {
      return completion_signal_->IsCompleted();
    } else {
      return true;
    }
  }

  bool WaitUntilCompleted(absl::Duration timeout) const {
    if (completion_signal_) {
      return completion_signal_->WaitUntilCompleted(timeout);
    } else {
      return true;
    }
  }

  void SignalEOS();

  GstreamerRunnerImpl(const Options& options) : options_(options) {}
  ~GstreamerRunnerImpl();

 private:
  absl::Status Initialize();
  absl::Status Finalize();

  Options options_;

  bool eos_signaled_ = false;

  std::unique_ptr<GstreamerPipeline> gstreamer_pipeline_ = nullptr;
  std::unique_ptr<CompletionSignal> completion_signal_ = nullptr;
  std::unique_ptr<GMainLoopManager> glib_loop_manager_ = nullptr;
};

absl::StatusOr<std::unique_ptr<GstreamerRunner::GstreamerRunnerImpl>>
GstreamerRunner::GstreamerRunnerImpl::Create(const Options& options) {
  auto runner_impl = std::make_unique<GstreamerRunnerImpl>(options);
  VAI_RETURN_IF_ERROR(runner_impl->Initialize());
  return std::move(runner_impl);
}

absl::Status GstreamerRunner::GstreamerRunnerImpl::Initialize() {
  // Create the gstreamer pipeline.
  auto gstreamer_pipeline_statusor = GstreamerPipeline::Create(options_);
  if (!gstreamer_pipeline_statusor.ok()) {
    return gstreamer_pipeline_statusor.status();
  }
  gstreamer_pipeline_ = std::move(gstreamer_pipeline_statusor).value();

  // Create the completion signal to observe the pipeline progress.
  completion_signal_ = std::make_unique<CompletionSignal>();
  GstBus* bus = gst_element_get_bus(gstreamer_pipeline_->gst_pipeline());
  gst_bus_add_watch(bus, (GstBusFunc)gst_bus_message_callback,
                    completion_signal_.get());
  gst_object_unref(bus);

  // Start the pipeline.
  completion_signal_->Start();
  glib_loop_manager_ = std::make_unique<GMainLoopManager>();
  gst_element_set_state(gstreamer_pipeline_->gst_pipeline(), GST_STATE_PLAYING);
  return absl::OkStatus();
}

void GstreamerRunner::GstreamerRunnerImpl::SignalEOS() {
  if (!eos_signaled_ && completion_signal_ &&
      !completion_signal_->IsCompleted() && gstreamer_pipeline_ &&
      gstreamer_pipeline_->gst_pipeline()) {
    if (gstreamer_pipeline_->gst_appsrc()) {
      GstFlowReturn ret;
      g_signal_emit_by_name(gstreamer_pipeline_->gst_appsrc(), "end-of-stream",
                            &ret);
    } else {
      gst_element_send_event(gstreamer_pipeline_->gst_pipeline(),
                             gst_event_new_eos());
    }
    eos_signaled_ = true;
  }
}

absl::Status GstreamerRunner::GstreamerRunnerImpl::Finalize() {
  // Allow the pipeline to complete its processing gracefully.
  // Do this by enforcing a deadline.
  if (completion_signal_ && !completion_signal_->IsCompleted() &&
      !completion_signal_->WaitUntilCompleted(
          absl::Seconds(kPipelineFinishTimeoutSeconds))) {
    LOG(WARNING) << "The gstreamer pipeline could not complete its cleanup "
                    "executions within the timeout ( "
                 << kPipelineFinishTimeoutSeconds
                 << "s). Discarding to move on; consumers might experince "
                    "dropped results";
    return absl::DeadlineExceededError("Pipeline failed to cleanup in time.");
  }
  if (gstreamer_pipeline_ && gstreamer_pipeline_->gst_pipeline())
    gst_element_set_state(gstreamer_pipeline_->gst_pipeline(), GST_STATE_NULL);

  return absl::OkStatus();
}

GstreamerRunner::GstreamerRunnerImpl::~GstreamerRunnerImpl() {
  SignalEOS();
  auto status = Finalize();
  if (!status.ok()) {
    LOG(ERROR) << status;
  }
}

absl::Status GstreamerRunner::GstreamerRunnerImpl::Feed(
    const GstreamerBuffer& gstreamer_buffer) {
  if (IsCompleted() || eos_signaled_) {
    return absl::FailedPreconditionError(
        "The runner has already completed. Please Create() it again and retry");
  }
  // Check that the pipeline is configured for Feeding.
  if (gstreamer_pipeline_->gst_appsrc() == nullptr) {
    return absl::InvalidArgumentError(
        "This runner is not configured for Feeding");
  }

  // Check that the given caps agree with those of the pipeline.
  if (gstreamer_buffer.caps_string() != options_.appsrc_caps_string) {
    return absl::InvalidArgumentError(absl::StrFormat(
        "Feeding the runner with caps \"%s\" when \"%s\" is expected",
        gstreamer_buffer.caps_string(), options_.appsrc_caps_string));
  }

  // Create a new GstBuffer by copying.
  GstBuffer* buffer = gst_buffer_new_and_alloc(gstreamer_buffer.size());
  GstMapInfo map;
  gst_buffer_map(buffer, &map, GST_MAP_READWRITE);
  std::copy(gstreamer_buffer.data(),
            gstreamer_buffer.data() + gstreamer_buffer.size(), (char*)map.data);
  gst_buffer_unmap(buffer, &map);
  if (!options_.appsrc_do_timestamps) {
    GST_BUFFER_PTS(buffer) = gstreamer_buffer.get_pts();
    GST_BUFFER_DTS(buffer) = gstreamer_buffer.get_dts();
    GST_BUFFER_DURATION(buffer) = gstreamer_buffer.get_duration();
  }
  // Feed the buffer.
  GstFlowReturn ret;
  g_signal_emit_by_name(gstreamer_pipeline_->gst_appsrc(), "push-buffer",
                        buffer, &ret);
  gst_buffer_unref(buffer);
  if (ret != GST_FLOW_OK) {
    return absl::InternalError("Failed to push a GstBuffer");
  }
  return absl::OkStatus();
}

// -----------------------------------------------------------------------
// GstreamerRunner

GstreamerRunner::GstreamerRunner() = default;

GstreamerRunner::~GstreamerRunner() = default;

absl::StatusOr<std::unique_ptr<GstreamerRunner>> GstreamerRunner::Create(
    const Options& options) {
  auto gstreamer_runner = std::make_unique<GstreamerRunner>();

  // Initializes gstreamer if it isn't already.
  absl::Status status = GstInit();
  if (!status.ok()) {
    LOG(ERROR) << status;
    return absl::InternalError("Could not initialize GStreamer");
  }

  // Create a GstreamerRunnerImpl.
  auto gstreamer_runner_impl_statusor = GstreamerRunnerImpl::Create(options);
  if (!gstreamer_runner_impl_statusor.ok()) {
    LOG(ERROR) << gstreamer_runner_impl_statusor.status();
    return absl::UnknownError("Failed to create a gstreamer runner");
  }
  gstreamer_runner->gstreamer_runner_impl_ =
      std::move(gstreamer_runner_impl_statusor).value();
  return std::move(gstreamer_runner);
}

absl::Status GstreamerRunner::Feed(
    const GstreamerBuffer& gstreamer_buffer) const {
  absl::Status status;
  if (gstreamer_runner_impl_) {
    status = gstreamer_runner_impl_->Feed(gstreamer_buffer);
  } else {
    return absl::InternalError("Runner not initialized.");
  }
  if (!status.ok()) {
    LOG(ERROR) << status;
    return absl::UnknownError("Failed to Feed the GstreamerRunner");
  }
  return absl::OkStatus();
}

bool GstreamerRunner::IsCompleted() const {
  if (gstreamer_runner_impl_) {
    return gstreamer_runner_impl_->IsCompleted();
  } else {
    return true;
  }
}

bool GstreamerRunner::WaitUntilCompleted(absl::Duration timeout) const {
  if (gstreamer_runner_impl_) {
    return gstreamer_runner_impl_->WaitUntilCompleted(timeout);
  } else {
    return true;
  }
}

void GstreamerRunner::SignalEOS() {
  if (gstreamer_runner_impl_) {
    gstreamer_runner_impl_->SignalEOS();
  }
}

}  // namespace visionai
