# This is the BUILD for gst-plugins-base.

load("//third_party/gstreamer:tools.bzl", "gen_config_file", "gnome_mkenums_simple", "specialize_file_template")

package(
    default_visibility = ["//visibility:public"],
    features = ["-layering_check"],
)

licenses(["restricted"])

gen_config_file(
    name = "gen_config",
    out = "config.h",
    defs = {
        "AUDIORESAMPLE_FORMAT_AUTO": "",
        "DEFAULT_AUDIOSINK": '"autoaudiosink"',
        "DEFAULT_VIDEOSINK": '"autovideosink"',
        "ENABLE_NLS": "1",
        "GETTEXT_PACKAGE": '"gst-plugins-base-1.0"',
        "GIO_LIBDIR": '"/usr/lib/x86_64-linux-gnu"',
        "GIO_MODULE_DIR": '"/usr/lib/x86_64-linux-gnu/gio/modules"',
        "GIO_PREFIX": '"/usr"',
        "GST_API_EXPORT": 'extern __attribute__ ((visibility ("default")))',
        "GST_API_VERSION": '"1.0"',
        "GST_DATADIR": '"/usr/local/share"',
        "GST_INSTALL_PLUGINS_HELPER": '"/usr/local/libexec/gst-install-plugins-helper"',
        "GST_LICENSE": '"LGPL"',
        "GST_PACKAGE_NAME": '"GStreamer Base Plug-ins source release"',
        "GST_PACKAGE_ORIGIN": '"Unknown package origin"',
        "GST_PACKAGE_RELEASE_DATETIME": '"2021-11-03"',
        "HAVE_CPU_X86_64": "1",
        "HAVE_DCGETTEXT": "1",
        "HAVE_DLFCN_H": "1",
        "HAVE_EMMINTRIN_H": "1",
        "HAVE_GIO_UNIX_2_0": "",
        "HAVE_GMTIME_R": "1",
        "HAVE_GRAPHENE": "",
        "HAVE_INTTYPES_H": "1",
        "HAVE_ISO_CODES": "",
        "HAVE_JPEG": "",
        "HAVE_LINUX_DMA_BUF_H": "1",
        "HAVE_LINUX_TCP_INFO": "",
        "HAVE_LOCALTIME_R": "1",
        "HAVE_LOG2": "1",
        "HAVE_LRINTF": "1",
        "HAVE_MEMORY_H": "1",
        "HAVE_MMAP": "1",
        "HAVE_NETINET_IN_H": "1",
        "HAVE_NETINET_TCP_H": "1",
        "HAVE_OGG": "",
        #"HAVE_ORC": "1", b/226934292
        "HAVE_PNG": "",
        "HAVE_SMMINTRIN_H": "1",
        "HAVE_STDINT_H": "1",
        "HAVE_STRINGS_H": "1",
        "HAVE_STRING_H": "1",
        "HAVE_SYS_SOCKET_H": "1",
        "HAVE_SYS_STAT_H": "1",
        "HAVE_SYS_TYPES_H": "1",
        "HAVE_SYS_WAIT_H": "1",
        "HAVE_UNISTD_H": "1",
        "HAVE_X11": "",
        "HAVE_XMMINTRIN_H": "1",
        "HAVE_XSHM": "",
        "HAVE_ZLIB": "",
        "ISO_CODES_PREFIX": '"/usr"',
        "ISO_CODES_VERSION": '"4.9.0"',
        "LOCALEDIR": '"/usr/local/share/locale"',
        "PACKAGE": '"gst-plugins-base"',
        "PACKAGE_VERSION": '"1.19.3"',
        "SIZEOF_CHAR": "1",
        "SIZEOF_INT": "4",
        "SIZEOF_LONG": "8",
        "SIZEOF_SHORT": "2",
        "SIZEOF_VOIDP": "8",
        "TARGET_CPU": '"x86_64"',
        "VERSION": '"1.19.3"',
    },
)

DEFAULT_COPTS = [
    "-w",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "config",
    hdrs = ["config.h"],
)

cc_library(
    name = "gst_libs_common",
    srcs = [
        "gst_libs/gst/allocators/gstdmabuf.c",
        "gst_libs/gst/allocators/gstfdmemory.c",
        "gst_libs/gst/allocators/gstphysmemory.c",
    ],
    hdrs = [
        "gst_libs/gst/allocators/allocators.h",
        "gst_libs/gst/allocators/allocators-prelude.h",
        "gst_libs/gst/allocators/gstdmabuf.h",
        "gst_libs/gst/allocators/gstfdmemory.h",
        "gst_libs/gst/allocators/gstphysmemory.h",
    ],
    copts = DEFAULT_COPTS,
    textual_hdrs = [],
    deps = [
        ":config",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

specialize_file_template(
    name = "gen_gstpluginsbaseversion",
    src = "gst_libs/gst/pbutils/gstpluginsbaseversion.h.in",
    out = "gst_libs/gst/pbutils/gstpluginsbaseversion.h",
    replacements = {
        "@PACKAGE_VERSION_MAJOR@": "1",
        "@PACKAGE_VERSION_MINOR@": "19",
        "@PACKAGE_VERSION_MICRO@": "3",
        "@PACKAGE_VERSION_NANO@": "1",
    },
)

gnome_mkenums_simple(
    name = "pbutils-enumtypes",
    body_prefix = '#ifdef HAVE_CONFIG_H\n#include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"\n#endif\n',
    decorator = "GST_PBUTILS_API",
    hdr_out = "gst_libs/gst/pbutils/pbutils-enumtypes.h",
    header_prefix = '#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/pbutils/pbutils-prelude.h"',
    sources = [
        "gst_libs/gst/pbutils/pbutils.h",
        "gst_libs/gst/pbutils/pbutils-prelude.h",
        "gst_libs/gst/pbutils/codec-utils.h",
        "gst_libs/gst/pbutils/descriptions.h",
        "gst_libs/gst/pbutils/encoding-profile.h",
        "gst_libs/gst/pbutils/encoding-target.h",
        "gst_libs/gst/pbutils/install-plugins.h",
        "gst_libs/gst/pbutils/missing-plugins.h",
        "gst_libs/gst/pbutils/gstdiscoverer.h",
        "gst_libs/gst/pbutils/gstaudiovisualizer.h",
    ],
    src_out = "gst_libs/gst/pbutils/pbutils-enumtypes.c",
)

cc_library(
    name = "gst_libs_pbutils",
    srcs = [
        "gst_libs/gst/pbutils/codec-utils.c",
        "gst_libs/gst/pbutils/descriptions.c",
        "gst_libs/gst/pbutils/encoding-profile.c",
        "gst_libs/gst/pbutils/encoding-target.c",
        "gst_libs/gst/pbutils/gstaudiovisualizer.c",
        "gst_libs/gst/pbutils/gstdiscoverer.c",
        "gst_libs/gst/pbutils/gstdiscoverer-types.c",
        "gst_libs/gst/pbutils/gstpluginsbaseversion.c",
        "gst_libs/gst/pbutils/install-plugins.c",
        "gst_libs/gst/pbutils/missing-plugins.c",
        "gst_libs/gst/pbutils/pbutils.c",
        "gst_libs/gst/pbutils/pbutils-enumtypes.c",
    ],
    hdrs = [
        "gst_libs/gst/pbutils/codec-utils.h",
        "gst_libs/gst/pbutils/descriptions.h",
        "gst_libs/gst/pbutils/encoding-profile.h",
        "gst_libs/gst/pbutils/encoding-target.h",
        "gst_libs/gst/pbutils/gstaudiovisualizer.h",
        "gst_libs/gst/pbutils/gstdiscoverer.h",
        "gst_libs/gst/pbutils/gstpluginsbaseversion.h",
        "gst_libs/gst/pbutils/install-plugins.h",
        "gst_libs/gst/pbutils/missing-plugins.h",
        "gst_libs/gst/pbutils/pbutils.h",
        "gst_libs/gst/pbutils/pbutils-enumtypes.h",
        "gst_libs/gst/pbutils/pbutils-prelude.h",
        "gst_libs/gst/pbutils/pbutils-private.h",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        ":gst_libs_audio",
        ":gst_libs_common",
        ":gst_libs_tag",
        ":gst_libs_video",
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

gnome_mkenums_simple(
    name = "video-enumtypes",
    body_prefix = '#ifdef HAVE_CONFIG_H\n#include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"\n#endif\n',
    decorator = "GST_VIDEO_API",
    hdr_out = "gst_libs/gst/video/video-enumtypes.h",
    header_prefix = '#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video-prelude.h"',
    sources = [
        "gst_libs/gst/video/video-anc.h",
        "gst_libs/gst/video/video-format.h",
        "gst_libs/gst/video/video-frame.h",
        "gst_libs/gst/video/video-chroma.h",
        "gst_libs/gst/video/video-color.h",
        "gst_libs/gst/video/video-converter.h",
        "gst_libs/gst/video/video-dither.h",
        "gst_libs/gst/video/video-info.h",
        "gst_libs/gst/video/video-overlay-composition.h",
        "gst_libs/gst/video/video-resampler.h",
        "gst_libs/gst/video/video-scaler.h",
        "gst_libs/gst/video/video-tile.h",
        "gst_libs/gst/video/video.h",
        "gst_libs/gst/video/gstvideometa.h",
        "gst_libs/gst/video/gstvideotimecode.h",
        "gst_libs/gst/video/gstvideoutils.h",
        "gst_libs/gst/video/gstvideoencoder.h",
        "gst_libs/gst/video/gstvideodecoder.h",
        "gst_libs/gst/video/colorbalance.h",
        "gst_libs/gst/video/navigation.h",
    ],
    src_out = "gst_libs/gst/video/video-enumtypes.c",
)

genrule(
    name = "gen_video_orc_h",
    srcs = ["gst_libs/gst/video/video-orc-dist.h"],
    outs = ["gst_libs/gst/video/video-orc.h"],
    cmd = "cat $< > $@",
)

genrule(
    name = "gen_video_orc_c",
    srcs = ["gst_libs/gst/video/video-orc-dist.c"],
    outs = ["gst_libs/gst/video/video-orc.c"],
    cmd = "cat $< > $@",
)

cc_library(
    name = "gst_libs_video",
    srcs = [
        "gst_libs/gst/video/colorbalance.c",
        "gst_libs/gst/video/colorbalancechannel.c",
        "gst_libs/gst/video/convertframe.c",
        "gst_libs/gst/video/gstvideoaffinetransformationmeta.c",
        "gst_libs/gst/video/gstvideoaggregator.c",
        "gst_libs/gst/video/gstvideocodecalphameta.c",
        "gst_libs/gst/video/gstvideodecoder.c",
        "gst_libs/gst/video/gstvideoencoder.c",
        "gst_libs/gst/video/gstvideofilter.c",
        "gst_libs/gst/video/gstvideometa.c",
        "gst_libs/gst/video/gstvideopool.c",
        "gst_libs/gst/video/gstvideosink.c",
        "gst_libs/gst/video/gstvideotimecode.c",
        "gst_libs/gst/video/gstvideoutils.c",
        "gst_libs/gst/video/gstvideoutilsprivate.c",
        "gst_libs/gst/video/navigation.c",
        "gst_libs/gst/video/video.c",
        "gst_libs/gst/video/video-anc.c",
        "gst_libs/gst/video/video-blend.c",
        "gst_libs/gst/video/video-chroma.c",
        "gst_libs/gst/video/video-color.c",
        "gst_libs/gst/video/video-converter.c",
        "gst_libs/gst/video/video-dither.c",
        "gst_libs/gst/video/video-enumtypes.c",
        "gst_libs/gst/video/video-event.c",
        "gst_libs/gst/video/video-format.c",
        "gst_libs/gst/video/video-frame.c",
        "gst_libs/gst/video/video-hdr.c",
        "gst_libs/gst/video/video-info.c",
        "gst_libs/gst/video/video-multiview.c",
        "gst_libs/gst/video/video-orc.c",
        "gst_libs/gst/video/video-overlay-composition.c",
        "gst_libs/gst/video/video-resampler.c",
        "gst_libs/gst/video/video-scaler.c",
        "gst_libs/gst/video/video-tile.c",
        "gst_libs/gst/video/videodirection.c",
        "gst_libs/gst/video/videoorientation.c",
        "gst_libs/gst/video/videooverlay.c",
    ],
    hdrs = [
        "gst_libs/gst/video/colorbalance.h",
        "gst_libs/gst/video/colorbalancechannel.h",
        "gst_libs/gst/video/gstvideoaffinetransformationmeta.h",
        "gst_libs/gst/video/gstvideoaggregator.h",
        "gst_libs/gst/video/gstvideocodecalphameta.h",
        "gst_libs/gst/video/gstvideodecoder.h",
        "gst_libs/gst/video/gstvideoencoder.h",
        "gst_libs/gst/video/gstvideofilter.h",
        "gst_libs/gst/video/gstvideometa.h",
        "gst_libs/gst/video/gstvideopool.h",
        "gst_libs/gst/video/gstvideosink.h",
        "gst_libs/gst/video/gstvideotimecode.h",
        "gst_libs/gst/video/gstvideoutils.h",
        "gst_libs/gst/video/gstvideoutilsprivate.h",
        "gst_libs/gst/video/navigation.h",
        "gst_libs/gst/video/video.h",
        "gst_libs/gst/video/video-anc.h",
        "gst_libs/gst/video/video-blend.h",
        "gst_libs/gst/video/video-chroma.h",
        "gst_libs/gst/video/video-color.h",
        "gst_libs/gst/video/video-converter.h",
        "gst_libs/gst/video/video-dither.h",
        "gst_libs/gst/video/video-enumtypes.h",
        "gst_libs/gst/video/video-event.h",
        "gst_libs/gst/video/video-format.h",
        "gst_libs/gst/video/video-frame.h",
        "gst_libs/gst/video/video-hdr.h",
        "gst_libs/gst/video/video-info.h",
        "gst_libs/gst/video/video-multiview.h",
        "gst_libs/gst/video/video-orc.h",
        "gst_libs/gst/video/video-overlay-composition.h",
        "gst_libs/gst/video/video-prelude.h",
        "gst_libs/gst/video/video-resampler.h",
        "gst_libs/gst/video/video-scaler.h",
        "gst_libs/gst/video/video-tile.h",
        "gst_libs/gst/video/videodirection.h",
        "gst_libs/gst/video/videoorientation.h",
        "gst_libs/gst/video/videooverlay.h",
    ],
    copts = DEFAULT_COPTS + [
        "-DDISABLE_ORC",
    ],
    deps = [
        ":config",
        ":gst_libs_common",
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

gnome_mkenums_simple(
    name = "audio-enumtypes",
    body_prefix = '#ifdef HAVE_CONFIG_H\n#include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"\n#endif\n',
    decorator = "GST_AUDIO_API",
    hdr_out = "gst_libs/gst/audio/audio-enumtypes.h",
    header_prefix = '#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/audio-prelude.h"',
    sources = [
        "gst_libs/gst/audio/audio.h",
        "gst_libs/gst/audio/audio-format.h",
        "gst_libs/gst/audio/audio-channels.h",
        "gst_libs/gst/audio/audio-channel-mixer.h",
        "gst_libs/gst/audio/audio-converter.h",
        "gst_libs/gst/audio/audio-info.h",
        "gst_libs/gst/audio/audio-quantize.h",
        "gst_libs/gst/audio/audio-resampler.h",
        "gst_libs/gst/audio/gstaudioaggregator.h",
        "gst_libs/gst/audio/gstaudioringbuffer.h",
        "gst_libs/gst/audio/gstaudiobasesrc.h",
        "gst_libs/gst/audio/gstaudiocdsrc.h",
        "gst_libs/gst/audio/gstaudiobasesink.h",
        "gst_libs/gst/audio/gstaudiostreamalign.h",
    ],
    src_out = "gst_libs/gst/audio/audio-enumtypes.c",
)

cc_library(
    name = "gst_libs_audio",
    srcs = [
        "gst_libs/gst/audio/audio.c",
        "gst_libs/gst/audio/audio-buffer.c",
        "gst_libs/gst/audio/audio-channel-mixer.c",
        "gst_libs/gst/audio/audio-channels.c",
        "gst_libs/gst/audio/audio-converter.c",
        "gst_libs/gst/audio/audio-enumtypes.c",
        "gst_libs/gst/audio/audio-format.c",
        "gst_libs/gst/audio/audio-info.c",
        "gst_libs/gst/audio/audio-quantize.c",
        "gst_libs/gst/audio/audio-resampler.c",
        "gst_libs/gst/audio/gstaudioaggregator.c",
        "gst_libs/gst/audio/gstaudiobasesink.c",
        "gst_libs/gst/audio/gstaudiobasesrc.c",
        "gst_libs/gst/audio/gstaudiocdsrc.c",
        "gst_libs/gst/audio/gstaudioclock.c",
        "gst_libs/gst/audio/gstaudiodecoder.c",
        "gst_libs/gst/audio/gstaudioencoder.c",
        "gst_libs/gst/audio/gstaudiofilter.c",
        "gst_libs/gst/audio/gstaudioiec61937.c",
        "gst_libs/gst/audio/gstaudiometa.c",
        "gst_libs/gst/audio/gstaudiopack.c",
        "gst_libs/gst/audio/gstaudioringbuffer.c",
        "gst_libs/gst/audio/gstaudiosink.c",
        "gst_libs/gst/audio/gstaudiosrc.c",
        "gst_libs/gst/audio/gstaudiostreamalign.c",
        "gst_libs/gst/audio/gstaudioutilsprivate.c",
        "gst_libs/gst/audio/streamvolume.c",
    ],
    hdrs = [
        "gst_libs/gst/audio/audio.h",
        "gst_libs/gst/audio/audio-buffer.h",
        "gst_libs/gst/audio/audio-channel-mixer.h",
        "gst_libs/gst/audio/audio-channels.h",
        "gst_libs/gst/audio/audio-converter.h",
        "gst_libs/gst/audio/audio-enumtypes.h",
        "gst_libs/gst/audio/audio-format.h",
        "gst_libs/gst/audio/audio-info.h",
        "gst_libs/gst/audio/audio-prelude.h",
        "gst_libs/gst/audio/audio-quantize.h",
        "gst_libs/gst/audio/audio-resampler.h",
        "gst_libs/gst/audio/audio-resampler-macros.h",
        "gst_libs/gst/audio/audio-resampler-private.h",
        "gst_libs/gst/audio/gstaudioaggregator.h",
        "gst_libs/gst/audio/gstaudiobasesink.h",
        "gst_libs/gst/audio/gstaudiobasesrc.h",
        "gst_libs/gst/audio/gstaudiocdsrc.h",
        "gst_libs/gst/audio/gstaudioclock.h",
        "gst_libs/gst/audio/gstaudiodecoder.h",
        "gst_libs/gst/audio/gstaudioencoder.h",
        "gst_libs/gst/audio/gstaudiofilter.h",
        "gst_libs/gst/audio/gstaudioiec61937.h",
        "gst_libs/gst/audio/gstaudiometa.h",
        "gst_libs/gst/audio/gstaudiopack.h",
        "gst_libs/gst/audio/gstaudioringbuffer.h",
        "gst_libs/gst/audio/gstaudiosink.h",
        "gst_libs/gst/audio/gstaudiosrc.h",
        "gst_libs/gst/audio/gstaudiostreamalign.h",
        "gst_libs/gst/audio/gstaudioutilsprivate.h",
        "gst_libs/gst/audio/streamvolume.h",
        "gst_libs/gst/pbutils/descriptions.h",
        "gst_libs/gst/pbutils/pbutils-prelude.h",
    ],
    copts = DEFAULT_COPTS + [
        "-DDISABLE_ORC",
    ],
    textual_hdrs = [
        "gst_libs/gst/audio/dbesi0.c",
    ],
    deps = [
        ":config",
        ":gst_libs_common",
        ":gst_libs_tag",
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

genrule(
    name = "gen_gstaudiopack_h",
    srcs = ["gst_libs/gst/audio/gstaudiopack-dist.h"],
    outs = ["gst_libs/gst/audio/gstaudiopack.h"],
    cmd = "cat $< > $@",
)

genrule(
    name = "gen_gstaudiopack_c",
    srcs = ["gst_libs/gst/audio/gstaudiopack-dist.c"],
    outs = ["gst_libs/gst/audio/gstaudiopack.c"],
    cmd = "cat $< > $@",
)

genrule(
    name = "gen_gstvideotestsrcorc_h",
    srcs = ["gst/videotestsrc/gstvideotestsrcorc-dist.h"],
    outs = ["gst/videotestsrc/gstvideotestsrcorc.h"],
    cmd = "cat $< > $@",
)

genrule(
    name = "gen_gstvideotestsrcorc_c",
    srcs = ["gst/videotestsrc/gstvideotestsrcorc-dist.c"],
    outs = ["gst/videotestsrc/gstvideotestsrcorc.c"],
    cmd = "cat $< > $@",
)

cc_library(
    name = "plugin_videotestsrc",
    srcs = [
        "gst/videotestsrc/gstvideotestsrc.c",
        "gst/videotestsrc/gstvideotestsrc.h",
        "gst/videotestsrc/gstvideotestsrcorc.c",
        "gst/videotestsrc/gstvideotestsrcorc.h",
        "gst/videotestsrc/videotestsrc.c",
        "gst/videotestsrc/videotestsrc.h",
    ],
    copts = DEFAULT_COPTS + [
        "-DDISABLE_ORC",
    ],
    deps = [
        ":config",
        ":gst_libs_common",
        ":gst_libs_video",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "plugin_audiotestsrc",
    srcs = [
        "gst/audiotestsrc/gstaudiotestsrc.c",
        "gst/audiotestsrc/gstaudiotestsrc.h",
    ],
    copts = DEFAULT_COPTS + [
        "-DDISABLE_ORC",
    ],
    deps = [
        ":config",
        ":gst_libs_audio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

gnome_mkenums_simple(
    name = "app-enumtypes",
    body_prefix = '#ifdef HAVE_CONFIG_H\n#include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"\n#endif\n',
    decorator = "GST_APP_API",
    hdr_out = "gst_libs/gst/app/app-enumtypes.h",
    header_prefix = '#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/app/app-prelude.h"',
    sources = [
        "gst_libs/gst/app/gstappsrc.h",
    ],
    src_out = "gst_libs/gst/app/app-enumtypes.c",
)

cc_library(
    name = "gst_libs_app",
    srcs = [
        "gst_libs/gst/app/app-enumtypes.c",
        "gst_libs/gst/app/app-enumtypes.h",
        "gst_libs/gst/app/app-prelude.h",
        "gst_libs/gst/app/gstappsink.c",
        "gst_libs/gst/app/gstappsrc.c",
    ],
    hdrs = [
        "gst_libs/gst/app/app.h",
        "gst_libs/gst/app/gstappsink.h",
        "gst_libs/gst/app/gstappsrc.h",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        ":gst_libs_common",
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "plugin_app",
    srcs = [
        "gst/app/gstapp.c",
        "gst/app/gstappelements.h",
        "gst/app/gstappsink.c",
        "gst/app/gstappsrc.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        ":gst_libs_app",
        ":gst_libs_common",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

gnome_mkenums_simple(
    name = "tag-enumtypes",
    body_prefix = '#ifdef HAVE_CONFIG_H\n#include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"\n#endif\n',
    decorator = "GST_TAG_API",
    hdr_out = "gst_libs/gst/tag/tag-enumtypes.h",
    header_prefix = '#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/tag/tag-prelude.h"',
    sources = [
        "gst_libs/gst/tag/tag.h",
    ],
    src_out = "gst_libs/gst/tag/tag-enumtypes.c",
)

cc_library(
    name = "plugin_playback",
    srcs = [
        "gst/playback/gstdecodebin2.c",
        "gst/playback/gstdecodebin3.c",
        "gst/playback/gstparsebin.c",
        "gst/playback/gstplay-enum.c",
        "gst/playback/gstplaybackelement.c",
        "gst/playback/gstplaybackplugin.c",
        "gst/playback/gstplaybackutils.c",
        "gst/playback/gstplaybin2.c",
        "gst/playback/gstplaybin3.c",
        "gst/playback/gstplaysink.c",
        "gst/playback/gstplaysinkaudioconvert.c",
        "gst/playback/gstplaysinkconvertbin.c",
        "gst/playback/gstplaysinkvideoconvert.c",
        "gst/playback/gststreamsynchronizer.c",
        "gst/playback/gstsubtitleoverlay.c",
        "gst/playback/gsturidecodebin.c",
        "gst/playback/gsturidecodebin3.c",
        "gst/playback/gsturisourcebin.c",
    ],
    hdrs = [
        "gst/playback/gstdecodebin3-parse.c",
        "gst/playback/gstplay-enum.h",
        "gst/playback/gstplaybackelements.h",
        "gst/playback/gstplaybackutils.h",
        "gst/playback/gstplaysink.h",
        "gst/playback/gstplaysinkaudioconvert.h",
        "gst/playback/gstplaysinkconvertbin.h",
        "gst/playback/gstplaysinkvideoconvert.h",
        "gst/playback/gstrawcaps.h",
        "gst/playback/gststreamsynchronizer.h",
        "gst/playback/gstsubtitleoverlay.h",
    ],
    copts = DEFAULT_COPTS + ["-DCOLORSPACE=\\\"videoconvert\\\""],
    deps = [
        ":config",
        ":gst_libs_audio",
        ":gst_libs_common",
        ":gst_libs_pbutils",
        ":gst_libs_tag",
        ":gst_libs_video",
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "gst_libs_tag",
    srcs = [
        "gst_libs/gst/tag/gstexiftag.c",
        "gst_libs/gst/tag/gstid3tag.c",
        "gst_libs/gst/tag/gsttagdemux.c",
        "gst_libs/gst/tag/gsttageditingprivate.c",
        "gst_libs/gst/tag/gsttagmux.c",
        "gst_libs/gst/tag/gstvorbistag.c",
        "gst_libs/gst/tag/gstxmptag.c",
        "gst_libs/gst/tag/id3v2.c",
        "gst_libs/gst/tag/id3v2frames.c",
        "gst_libs/gst/tag/lang.c",
        "gst_libs/gst/tag/tag-enumtypes.c",
        "gst_libs/gst/tag/tags.c",
        "gst_libs/gst/tag/xmpwriter.c",
    ],
    hdrs = [
        "gst_libs/gst/gettext.h",
        "gst_libs/gst/gst-i18n-plugin.h",
        "gst_libs/gst/tag/gsttagdemux.h",
        "gst_libs/gst/tag/gsttageditingprivate.h",
        "gst_libs/gst/tag/gsttagmux.h",
        "gst_libs/gst/tag/id3v2.h",
        "gst_libs/gst/tag/tag.h",
        "gst_libs/gst/tag/tag-enumtypes.h",
        "gst_libs/gst/tag/tag-prelude.h",
        "gst_libs/gst/tag/xmpwriter.h",
    ],
    copts = DEFAULT_COPTS,
    textual_hdrs = [
        "gst_libs/gst/tag/lang-tables.dat",
    ],
    deps = [
        ":config",
        ":gst_libs_common",
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "@zlib",
    ],
)

gnome_mkenums_simple(
    name = "gstrtp-enumtypes",
    body_prefix = '#ifdef HAVE_CONFIG_H\n#include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"\n#endif\n',
    decorator = "GST_RTP_API",
    hdr_out = "gst_libs/gst/rtp/gstrtp-enumtypes.h",
    header_prefix = '#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/rtp-prelude.h"',
    sources = glob(["gst_libs/gst/rtp/*.h"]),
    src_out = "gst_libs/gst/rtp/gstrtp-enumtypes.c",
)

cc_library(
    name = "gst_libs_rtp",
    srcs = glob(["gst_libs/gst/rtp/*.c"]) + [
        "gst_libs/gst/rtp/gstrtp-enumtypes.c",
    ],
    copts = DEFAULT_COPTS + [
        "-DBUILDING_GST_RTP",
        "-DG_LOG_DOMAIN=\\\"GStreamer-RTP\\\"",
    ],
    textual_hdrs = glob(["gst_libs/gst/rtp/*.h"]) + [
        "gst_libs/gst/rtp/gstrtp-enumtypes.h",
    ],
    deps = [
        ":config",
        ":gst_libs_audio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "gst_libs_sdp",
    srcs = glob(["gst_libs/gst/sdp/*.c"]),
    copts = DEFAULT_COPTS + [
        "-DBUILDING_GST_SDP",
        "-DG_LOG_DOMAIN=\\\"GStreamer-SDP\\\"",
    ],
    textual_hdrs = glob(["gst_libs/gst/sdp/*.h"]),
    deps = [
        ":config",
        ":gst_libs_pbutils",
        ":gst_libs_rtp",
        "//third_party/glib/gio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

gnome_mkenums_simple(
    name = "gstrtsp-enumtypes",
    body_prefix = '#ifdef HAVE_CONFIG_H\n#include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"\n#endif\n',
    decorator = "GST_RTSP_API",
    hdr_out = "gst_libs/gst/rtsp/gstrtsp-enumtypes.h",
    header_prefix = '#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtsp/rtsp-prelude.h"',
    sources = glob(["gst_libs/gst/rtsp/*.h"]),
    src_out = "gst_libs/gst/rtsp/gstrtsp-enumtypes.c",
)

cc_library(
    name = "gst_libs_rtsp",
    srcs = glob(["gst_libs/gst/rtsp/*.c"]) + [
        "gst_libs/gst/rtsp/gstrtsp-enumtypes.c",
    ],
    copts = DEFAULT_COPTS + [
        "-DBUILDING_GST_RTSP",
        "-DG_LOG_DOMAIN=\\\"GStreamer-RTSP\\\"",
    ],
    textual_hdrs = glob(["gst_libs/gst/rtsp/*.h"]) + [
        "gst_libs/gst/rtsp/gstrtsp-enumtypes.h",
    ],
    deps = [
        ":config",
        ":gst_libs_audio",
        ":gst_libs_sdp",
        "//third_party/glib/gio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "gst_libs_riff",
    srcs = glob(["gst_libs/gst/riff/*.c"]),
    copts = DEFAULT_COPTS + [
        "-DBUILDING_GST_RIFF",
    ],
    textual_hdrs = glob(["gst_libs/gst/riff/*.h"]),
    deps = [
        ":config",
        ":gst_libs_audio",
        ":gst_libs_tag",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "plugin_videorate",
    srcs = glob(["gst/videorate/*.c"]),
    hdrs = glob(["gst/videorate/*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        ":gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "plugin_videoscale",
    srcs = glob(["gst/videoscale/*.c"]),
    hdrs = glob(["gst/videoscale/*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        ":gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "plugin_gio",
    srcs = glob(["gst/gio/*.c"]),
    hdrs = glob(["gst/gio/*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        "//third_party/glib/gio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "plugin_rawparse",
    srcs = glob(["gst/rawparse/*.c"]),
    hdrs = glob(["gst/rawparse/*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        ":gst_libs_audio",
        ":gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
