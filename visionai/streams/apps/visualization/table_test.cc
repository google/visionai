#include "visionai/streams/apps/visualization/table.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "visionai/streams/apps/visualization/constants.h"
#include "visionai/streams/apps/visualization/test_utils.h"

namespace visionai {
namespace {

using renderutils::kLineThick1;
using renderutils::kStatsTextFontscale;
using renderutils::kStatsTextPaddingX;
using renderutils::kStatsTextPaddingY;
using renderutils::kStatsTextThickness;
using renderutils::kTableAlpha;
using renderutils::kTextFontFamily;

const cv::Scalar kBlackColor = cv::Scalar(0, 0, 0);
const cv::Scalar kWhiteColor = cv::Scalar(255, 255, 255);
const cv::Scalar kBlueColor = cv::Scalar(255, 0, 0);

void GenerateExpectedMat(cv::Mat& expected,
                         std::vector<std::vector<std::string>>& content) {
  // Compute and store the max height of each row and max width of each column
  std::vector<int> row_maxes(content.size());
  std::vector<int> col_maxes(content.size());
  for (int i = 0; i < content.size(); i++) {
    for (int j = 0; j < content[0].size(); j++) {
      std::string& cell_content = content[i][j];
      cv::Size text_size =
          cv::getTextSize(cell_content, kTextFontFamily, kStatsTextFontscale,
                          kStatsTextThickness, 0);
      row_maxes[i] =
          std::max(row_maxes[i], text_size.height + kStatsTextPaddingY);
      col_maxes[j] =
          std::max(col_maxes[j], text_size.width + kStatsTextPaddingX);
    }
  }

  int height = 0;
  for (int i = 0; i < row_maxes.size(); i++) {
    height += row_maxes[i];
  }

  int width = 0;
  for (int i = 0; i < col_maxes.size(); i++) {
    width += col_maxes[i];
  }

  // Create the slightly transparent background
  cv::Mat overlay(expected(cv::Rect(10, 10, width, height)));
  cv::Mat background(overlay.size(), CV_8UC3, kBlackColor);

  cv::addWeighted(background, kTableAlpha, overlay, 1.0 - kTableAlpha, 0.0,
                  overlay);

  // Write all of the text onto the table
  int y_start = 0;
  for (int i = 0; i < content.size(); i++) {
    int row_size_offset = row_maxes[i];
    int line_y_pos = y_start += row_size_offset;

    int x_start = 0;
    for (int j = 0; j < content[0].size(); j++) {
      int column_size_offset = j == 0 ? 0 : col_maxes[j - 1];
      int line_x_pos = x_start += column_size_offset;

      std::string& current_str = content[i][j];
      cv::putText(overlay, current_str,
                  cv::Point(line_x_pos + kStatsTextPaddingX / 2,
                            line_y_pos - kStatsTextPaddingY / 2),
                  kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
  }

  // Draw all horizontal lines
  int column_start_y = 0;
  for (int i = 0; i <= content.size(); i++) {
    int row_size_offset = i == 0 ? 0 : row_maxes[i - 1];
    int line_y_pos = column_start_y += row_size_offset;
    cv::line(overlay, cv::Point(0, line_y_pos), cv::Point(width, line_y_pos),
             kWhiteColor, kLineThick1);
  }

  // Draw all vertical lines
  int column_start_x = 0;
  for (int i = 0; i <= content[0].size(); i++) {
    int size_offset = i == 0 ? 0 : col_maxes[i - 1];
    int line_x_pos = column_start_x += size_offset;
    cv::line(overlay, cv::Point(line_x_pos, 0), cv::Point(line_x_pos, height),
             kWhiteColor, kLineThick1);
  }

  expected(cv::Rect(10, 10, width, height)) = overlay;
}

// Simple wrapper around running a test where items are expected to draw from
// the table's overlay method. Removes duplication in the boilerplate setup code
bool RunTableTest(std::vector<std::vector<std::string>>& content) {
  std::vector<std::vector<std::string>> copy_content = content;

  renderutils::Table table(content);

  cv::Mat base_mat(renderutils::mat_cols, renderutils::mat_rows,
                   CV_8UC3, kBlueColor);

  cv::Mat expected_mat = base_mat.clone();

  table.overlay(base_mat, 10, 10);

  GenerateExpectedMat(expected_mat, copy_content);

  return testutils::CheckTwoImagesEqual(base_mat, expected_mat);
}

// If no content is provided in the table, no overlay should be generated and
// the image should be untouched
TEST(TableOverlay, TestOverlayNoTableContent) {
  std::vector<std::vector<std::string>> table_content;
  renderutils::Table table(table_content);

  cv::Mat base_mat(renderutils::mat_rows, renderutils::mat_cols
                   , CV_8UC3, kBlueColor);

  cv::Mat expected_mat = base_mat.clone();

  table.overlay(base_mat, 10, 10);

  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

// Nothing should draw if the table contains blank row vectors.
TEST(TableOverlay, TestOverlayNoTableCell) {
  std::vector<std::vector<std::string>> table_content{
      std::vector<std::string>{}};
  renderutils::Table table(table_content);

  cv::Mat base_mat(renderutils::mat_rows, renderutils::mat_cols,
                   CV_8UC3, kBlueColor);

  cv::Mat expected_mat = base_mat.clone();

  table.overlay(base_mat, 10, 10);

  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

// We want to make sure the table doesn't overlay with an irregularly shaped 2-D
// vector
TEST(TableOverlay, TestOverlayIrregularVector) {
  std::vector<std::vector<std::string>> table_content{
      std::vector<std::string>{"Two", "Cells"},
      std::vector<std::string>{"OneCell"},
  };
  renderutils::Table table(table_content);

  cv::Mat base_mat(renderutils::mat_rows, renderutils::mat_cols, 
                   CV_8UC3, kBlueColor);

  cv::Mat expected_mat = base_mat.clone();

  table.overlay(base_mat, 10, 10);

  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

// We want to make sure something is changed from the original mat when our
// table has content
TEST(TableOverlay, TestMatChangesWithContent) {
  std::vector<std::vector<std::string>> table_content{
      std::vector<std::string>{"Multiple", "Items"},
      std::vector<std::string>{"Multiple", "Rows"},
  };
  renderutils::Table table(table_content);

  cv::Mat base_mat(renderutils::mat_rows, renderutils::mat_cols, 
                   CV_8UC3, kBlueColor);

  cv::Mat expected_mat = base_mat.clone();

  table.overlay(base_mat, 10, 10);

  EXPECT_FALSE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

// Ensure the table draws correctly with a single item in its content list
TEST(TableOverlay, TestOverlaySingleItemContent) {
  std::vector<std::vector<std::string>> table_content{
      std::vector<std::string>{"Single Item"}};

  EXPECT_TRUE(RunTableTest(table_content));
}

// Ensure the table draws correctly with multiple items in its content list
TEST(TableOverlay, TestOverlayMultiItemContent) {
  std::vector<std::vector<std::string>> table_content{
      std::vector<std::string>{"Multiple", "Items"},
      std::vector<std::string>{"Multiple", "Rows"},
  };

  EXPECT_TRUE(RunTableTest(table_content));
}
}  // namespace
}  // namespace visionai