// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/streams/apps/visualization/table.h"

#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "visionai/streams/apps/visualization/constants.h"

namespace visionai {
namespace renderutils {

const cv::Scalar kBlackColor = cv::Scalar(0, 0, 0);
const cv::Scalar kBlueColor(255, 0, 0);
const cv::Scalar kWhiteColor = cv::Scalar(255, 255, 255);
const cv::Scalar kRedColor = cv::Scalar(0, 0, 255);
const cv::Scalar kGreenColor(0, 255, 0);

void Table::overlay(cv::Mat& image, int x_start_pos, int y_start_pos) {
  if (!isValid()) return;
  int table_height = getHeight();
  int table_width = getWidth();

  // Extract the portion of the image the table needs into a new mat so we
  // don't clone the entire image
  cv::Rect table_rect(x_start_pos, y_start_pos, table_width, table_height);
  cv::Mat overlay = image(table_rect);

  // Draw the black background and set it slightly transparent
  cv::Mat color(overlay.size(), CV_8UC3, kBlackColor);
  cv::addWeighted(color, kTableAlpha, overlay, 1.0 - kTableAlpha, 0.0, overlay);

  // Draw all horizontal lines
  int column_start_y = 0;
  for (int i = 0; i <= table_contents_.size(); i++) {
    int row_size_offset = i == 0 ? 0 : getRowSize(i - 1);
    int line_y_pos = column_start_y += row_size_offset;
    cv::line(overlay, cv::Point(0, line_y_pos),
             cv::Point(table_width, line_y_pos), kWhiteColor, kLineThick1);
  }

  // Draw all vertical lines
  int column_start_x = 0;
  for (int i = 0; i <= table_contents_[0].size(); i++) {
    int size_offset = i == 0 ? 0 : getColumnSize(i - 1);
    int line_x_pos = column_start_x += size_offset;
    cv::line(overlay, cv::Point(line_x_pos, 0),
             cv::Point(line_x_pos, table_height), kWhiteColor, kLineThick1);
  }

  // Draw all text in the backing 2D array. We've already validated this table,
  // so we know it's rectangular (m x n) allowing us to just use the first row's
  // cell count for the nested loop.
  column_start_y = 0;
  for (int i = 0; i < table_contents_.size(); i++) {
    int row_size_offset = getRowSize(i);
    int line_y_pos = column_start_y += row_size_offset;

    column_start_x = 0;
    for (int j = 0; j < table_contents_[0].size(); j++) {
      int column_size_offset = j == 0 ? 0 : getColumnSize(j - 1);
      int line_x_pos = column_start_x += column_size_offset;

      std::string& current_str = table_contents_[i][j];
      cv::putText(overlay, current_str,
                  cv::Point(line_x_pos + kStatsTextPaddingX / 2,
                            line_y_pos - kStatsTextPaddingY / 2),
                  kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
  }

  image(table_rect) = overlay;
}

bool Table::isValid() {
  // All tables must have content within it, so we return invalid if either
  // there are no rows or the content within said row is empty
  if (table_contents_.empty() || table_contents_[0].empty()) return false;

  int table_rows = table_contents_.size();
  int table_cols = table_contents_[0].size();

  // Check if each row's cell count matches the cell count of the first row,
  // ensuring that this table is m by n and not irregularly shaped
  for (int i = 1; i < table_rows; i++) {
    if (table_contents_[i].size() != table_cols) return false;
  }
  return true;
}

int Table::getHeight() {
  if (table_contents_.empty()) return 0;
  int height = 0;
  for (int i = 0; i < table_contents_.size(); i++) {
    height += getRowSize(i);
  }
  return height;
}

int Table::getWidth() {
  if (table_contents_.empty()) return 0;
  int width = 0;
  for (int i = 0; i < table_contents_[0].size(); i++) {
    width += getColumnSize(i);
  }
  return width;
}

int Table::getRowSize(int row) {
  if (row_sizes_[row]) return row_sizes_[row];
  int max_height = 0;
  for (int i = 0; i < table_contents_[row].size(); i++) {
    max_height =
        std::max(max_height, getTextSize(table_contents_[row][i]).height);
  }
  row_sizes_[row] = max_height + kStatsTextPaddingY;
  return row_sizes_[row];
}

int Table::getColumnSize(int column) {
  if (column_sizes_[column]) return column_sizes_[column];
  int max_width = 0;
  for (int i = 0; i < table_contents_.size(); i++) {
    max_width =
        std::max(max_width, getTextSize(table_contents_[i][column]).width);
  }
  column_sizes_[column] = max_width + kStatsTextPaddingX;
  return column_sizes_[column];
}

inline cv::Size Table::getTextSize(std::string& text) {
  return cv::getTextSize(text, kTextFontFamily, kStatsTextFontscale,
                         kStatsTextThickness, 0);
}

}  // namespace renderutils
}  // namespace visionai
