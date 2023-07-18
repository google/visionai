#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_TABLE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_TABLE_H_

#include <string>
#include <vector>

#include "opencv4/opencv2/core/mat.hpp"

namespace visionai {
namespace renderutils {
// Table is used by the Anaheim Visualization Tool.
// Table uses a 2-D rectangular vector containing text in each index to render
// the contents in a table-form in OpenCV. The intended use-cases for this class
// are as follows:
// 1. To overlay a table in the Anaheim Visualization Tool based on information
// formatted in a 2-D vector created from a model's prediction results. A good
// example of this is the Occupancy Analysis Model's Visualization showing a
// table on the top corner with the current count of People and Vehicles
// detected.
class Table {
 public:
  // Constructs a new instance of table using the given source 2-D vector. This
  // operation will move the vector, so its contents will no longer be available
  // in the initial declaration.
  Table(std::vector<std::vector<std::string>>& table_contents)
      : table_contents_(std::move(table_contents)),
        row_sizes_(std::vector<int>(table_contents_.size())),
        column_sizes_(std::vector<int>(
            !table_contents_.empty() ? table_contents_[0].size() : 0)) {}

  // Overlay the 2-D vector's contents formatted as a table with m rows and n
  // columns where m corresponds to the size of the vector and n corresponds to
  // the size of entries within the first row (and all rows) of the vector.
  // Draws the table onto the provided image at the pixel positions given by
  // x_start_pos and y_start_pos, which will default to 0 if not provided.
  void overlay(cv::Mat& image, int x_start_pos = 0, int y_start_pos = 0);

 private:
  // The 2-D vector that contains the textual content to render in each cell.
  // All sizing and validation calculations are based around this backing
  // vector.
  std::vector<std::vector<std::string>> table_contents_;

  // A vector to cache the size of rows so their size isn't recalculated
  // multiple times within the same draw
  std::vector<int> row_sizes_;

  // A vector to cache the size of columns so their size isn't recalculated
  // multiple times within the same draw
  std::vector<int> column_sizes_;

  // Make sure the table is rectangularly shaped (all rows have the same amount
  // of columns)
  bool isValid();

  // Get the table's total height in pixels by adding up the calculated heights
  // of each row
  int getWidth();

  // Get the table's total width in pixels by adding up the calculated widths of
  // each column
  int getHeight();

  // Calculates the height of a row in pixels based on the tallest piece of text
  // that will appear within the row as a table's row height must be higher than
  // all text within it
  int getColumnSize(int column);

  // Calculates the width of a single column in pixels based on the longest
  // piece of text that will appear within it as the table's column width must
  // be longer than all text within it.
  int getRowSize(int row);

  // A convenience inline wrapper around OpenCV's getTextSize method as we
  // currently use the same font information per-call.
  cv::Size getTextSize(std::string& text);
};
}  // namespace renderutils
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_TABLE_H_
