// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/media/util/register_plugins_for_sdk.h"

#include "absl/debugging/leak_check.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
extern "C" {
GST_PLUGIN_STATIC_DECLARE(app);
GST_PLUGIN_STATIC_DECLARE(coreelements);
GST_PLUGIN_STATIC_DECLARE(gio);
GST_PLUGIN_STATIC_DECLARE(isomp4);
GST_PLUGIN_STATIC_DECLARE(libav);
GST_PLUGIN_STATIC_DECLARE(matroska);
GST_PLUGIN_STATIC_DECLARE(mpegtsmux);
GST_PLUGIN_STATIC_DECLARE(multifile);
GST_PLUGIN_STATIC_DECLARE(playback);
GST_PLUGIN_STATIC_DECLARE(rawparse);
GST_PLUGIN_STATIC_DECLARE(rtp);
GST_PLUGIN_STATIC_DECLARE(rtpmanager);
GST_PLUGIN_STATIC_DECLARE(rtsp);
GST_PLUGIN_STATIC_DECLARE(typefindfunctions);
GST_PLUGIN_STATIC_DECLARE(videoconvert);
GST_PLUGIN_STATIC_DECLARE(videoparsersbad);
GST_PLUGIN_STATIC_DECLARE(videorate);
}
}  // namespace

// Excluding x264.
absl::Status RegisterGstPluginsForSDK() {
  absl::LeakCheckDisabler disabler;
  VAI_RETURN_IF_ERROR(GstInit());
  GST_PLUGIN_STATIC_REGISTER(app);
  GST_PLUGIN_STATIC_REGISTER(coreelements);
  GST_PLUGIN_STATIC_REGISTER(gio);
  GST_PLUGIN_STATIC_REGISTER(isomp4);
  GST_PLUGIN_STATIC_REGISTER(libav);
  GST_PLUGIN_STATIC_REGISTER(matroska);
  GST_PLUGIN_STATIC_REGISTER(mpegtsmux);
  GST_PLUGIN_STATIC_REGISTER(multifile);
  GST_PLUGIN_STATIC_REGISTER(playback);
  GST_PLUGIN_STATIC_REGISTER(rawparse);
  GST_PLUGIN_STATIC_REGISTER(rtp);
  GST_PLUGIN_STATIC_REGISTER(rtpmanager);
  GST_PLUGIN_STATIC_REGISTER(rtsp);
  GST_PLUGIN_STATIC_REGISTER(typefindfunctions);
  GST_PLUGIN_STATIC_REGISTER(videoconvert);
  GST_PLUGIN_STATIC_REGISTER(videoparsersbad);
  GST_PLUGIN_STATIC_REGISTER(videorate);

  return absl::OkStatus();
}
}  // namespace visionai
