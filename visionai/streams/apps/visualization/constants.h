#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_CONSTANTS_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_CONSTANTS_H_

#include "opencv4/opencv2/imgproc.hpp"
namespace visionai {
namespace renderutils {

inline constexpr int kTextFontFamily = cv::FONT_HERSHEY_DUPLEX;

// Font constants for statistics and table text. These are shared across all
// renderutils helpers
inline constexpr double kStatsTextThickness = 1;
inline constexpr double kStatsTextFontscale = 1;

inline constexpr double kTrackTextFontscale = 0.5;

// Tabel draw padding offset constants
inline constexpr int kStatsTextPaddingX = 40;
inline constexpr int kStatsTextPaddingY = 20;
inline constexpr double kTableAlpha = 0.5;

// Text Drawing padding for bounding box labels
inline constexpr int kLabelTextPaddingY = 10;

// render_utils table & text drawing constants
// TODO: as we migrate more functionality out of render_utils, remove the
// unneeded constants
inline constexpr int kRowHeight = 40;
inline constexpr int kXInitOffset = 10;
inline constexpr int kYInitOffset = 20;
inline constexpr int kLineThick1 = 1;
inline constexpr int kLineThick2 = 2;
inline constexpr int kFirstColumnWidth = 160;
inline constexpr int kItemColumnWidth = 90;
inline constexpr int kItemColumnWidthLineCrossing = 160;
inline constexpr double kStatsTextLineCrossingFontscale = 0.6;
inline constexpr int kXTextOffset = 20;
inline constexpr int kYTextOffset = -10;

// Drawing constants for number of rows and columns in Mat 2D array
inline constexpr int mat_cols = 600;
inline constexpr int mat_rows = 800;


}  // namespace renderutils
}  // namespace visionai
#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_CONSTANTS_H_
