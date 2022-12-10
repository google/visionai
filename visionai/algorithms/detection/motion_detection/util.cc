// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/detection/motion_detection/util.h"

#include <cmath>
#include <cstring>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "visionai/util/array/array2d.h"
#include "visionai/util/array/array3d.h"
#include "visionai/util/gtl/circularbuffer.h"

constexpr float kEpsilon = 1e-5;
constexpr int kBinNumber = 50;
constexpr int kNumFeatures = 14;  // Feature dimension.

namespace visionai {
namespace motion_detection {

float EstimateEntropy(const std::vector<float>& numbers, float mean,
                      float standard_deviation) {
  if (numbers.empty() || std::abs(standard_deviation) < kEpsilon) {
    return 0;
  }

  float entropy = 0;
  float upper_bound = mean + 5 * standard_deviation;
  float bin_size = upper_bound / kBinNumber;
  std::vector<float> frequencies(kBinNumber + 1, 0);

  for (const float& number : numbers) {
    int bin = std::min(kBinNumber, static_cast<int>(floor(number / bin_size)));
    frequencies[bin] += 1.0;
  }

  for (int i = 0; i < kBinNumber + 1; ++i) {
    if (frequencies[i] > 0) {
      frequencies[i] /= numbers.size();
      entropy -= frequencies[i] * std::log(frequencies[i]);
    }
  }
  return entropy;
}

float VectorAverage(const std::vector<float>& vec) {
  return std::accumulate(vec.begin(), vec.end(), 0.0) /
         static_cast<float>(vec.size());
}

absl::Status ComputeMotionVectorFeaturesWithSpatialGrid(
    const MotionVectors& motion_vectors,
    gtl::CircularBuffer<Array3D<float>>& grids_features_buffer, int num_grid,
    int frame_height, int frame_width, std::vector<float>& mv_features) {
  if (num_grid <= 0 || frame_height <= 0 || frame_width <= 0) {
    return absl::InvalidArgumentError(
        "One of grid number, frame height or width is less than or equal to "
        "zero");
  }

  int grid_height = static_cast<int>(std::ceil(frame_height / num_grid));
  int grid_width = static_cast<int>(std::ceil(frame_width / num_grid));
  int mv_features_size = num_grid * num_grid * kNumFeatures * 2;

  if (mv_features.size() != mv_features_size) {
    return absl::InvalidArgumentError(
        absl::StrFormat("mv feature size should be %i, got %i",
                        mv_features_size, mv_features.size()));
  }

  std::fill(mv_features.begin(), mv_features.end(), 0.0);

  Array3D<float> feature_average(SHARE_WITH_FOREIGN_INSTANCE, num_grid,
                                 num_grid, kNumFeatures, mv_features.data());
  Array3D<float> feature_standard_deviation(
      SHARE_WITH_FOREIGN_INSTANCE, num_grid, num_grid, kNumFeatures,
      mv_features.data() + num_grid * num_grid * kNumFeatures);

  Array3D<float> features_from_all_grids(num_grid, num_grid, kNumFeatures, 0.0);

  // Finds the motion vector indexes for each grid in array `motion_vectors`.
  Array2D<std::vector<int>> grid_mv_idx(num_grid, num_grid);
  for (int i = 0; i < motion_vectors.size(); ++i) {
    const MotionVector& mv = motion_vectors[i];
    int grid_x = mv.dst_x / grid_width;
    int grid_y = mv.dst_y / grid_height;
    if (grid_x >= num_grid || grid_y >= num_grid)
      continue;
    grid_mv_idx(grid_y, grid_x).push_back(i);
  }

  for (int h = 0; h < num_grid; ++h) {
    for (int w = 0; w < num_grid; ++w) {
      // Store the motion vectors in this particular grid.
      std::vector<MotionVector> grid_motion_vectors;
      grid_motion_vectors.reserve(grid_mv_idx(h, w).size());
      for (int i = 0; i < grid_mv_idx(h, w).size(); ++i) {
        grid_motion_vectors.push_back(motion_vectors[grid_mv_idx(h, w)[i]]);
      }

      // Compute the features for a particular grid.
      std::vector<float> features_from_one_grid = ComputeMotionVectorFeatures(
          grid_motion_vectors, num_grid, frame_height, frame_width);

      for (int k = 0; k < kNumFeatures; ++k) {
        features_from_all_grids(h, w, k) = features_from_one_grid[k];
      }
    }
  }

  // Update the temportal grid feature buffer except for the I frame.
  if (!motion_vectors.empty()) {
    grids_features_buffer.push_back(features_from_all_grids);
  }

  for (int h = 0; h < num_grid; ++h) {
    for (int w = 0; w < num_grid; ++w) {
      for (int k = 0; k < kNumFeatures; ++k) {
        // Update the running average.
        float average_sum = 0;
        for (int j = 0; j < grids_features_buffer.size(); ++j) {
          average_sum += grids_features_buffer.at(j)(h, w, k);
        }

        feature_average(h, w, k) = average_sum / grids_features_buffer.size();

        // Update the running standard_deviation.
        float standard_deviation_sum = 0;

        for (int j = 0; j < grids_features_buffer.size(); ++j) {
          standard_deviation_sum += std::pow(
              grids_features_buffer.at(j)(h, w, k) - feature_average(h, w, k),
              2.0);
        }

        feature_standard_deviation(h, w, k) =
            std::sqrt(standard_deviation_sum / grids_features_buffer.size());
      }
    }
  }
  return absl::OkStatus();
}

std::vector<float> ComputeMotionVectorFeatures(
    const MotionVectors& motion_vectors, int num_grid,
    int frame_height, int frame_width) {
  // MV number
  int input_mv_number = motion_vectors.size();
  float weighted_mv_number = 0;
  int nonzero_mv_number = 0;

  // Motion magnitudes
  std::vector<float> motion_magnitudes(input_mv_number, 0);
  std::vector<float> motion_x(input_mv_number, 0);
  std::vector<float> motion_y(input_mv_number, 0);

  // First and higher order spread of magnitudes
  float magnitude_mean_absolute_deviation = 0;
  float magnitude_standard_deviation = 0;

  // Maximum motion magnitude
  float max_motion_magnitude = 0;
  float max_motion_x = 0;
  float max_motion_y = 0;

  // Macroblock sizes
  float total_block_size = 0;
  float nonzero_block_size = 0;

  // First order spread of motion vector wrt to mean motion vector
  float mean_absolute_deviation = 0;

  // Directional information
  float mean_angle = 0;
  float mean_angle_wrt_max = 0;

  for (int i = 0; i < input_mv_number; ++i) {
    const MotionVector& mv = motion_vectors[i];
    float mv_x =
        static_cast<float>(mv.motion_x) / mv.motion_scale / frame_width;
    float mv_y =
        static_cast<float>(mv.motion_y) / mv.motion_scale / frame_height;
    total_block_size += std::sqrt(mv.w * mv.h);

    float motion_magnitude =
        std::sqrt(std::pow(mv_x, 2.0) + std::pow(mv_y, 2.0));
    motion_magnitudes[i] = motion_magnitude;
    motion_x[i] = mv_x;
    motion_y[i] = mv_y;

    if (motion_magnitude > max_motion_magnitude) {
      max_motion_magnitude = motion_magnitude;
      max_motion_x = mv_x;
      max_motion_y = mv_y;
    }

    if (std::abs(mv_x) > kEpsilon || std::abs(mv_y) > kEpsilon) {
      nonzero_mv_number += 1;

      CHECK(mv.w > 0 && mv.h > 0);
      weighted_mv_number += 16.0 / std::sqrt(mv.w * mv.h);
      nonzero_block_size += std::sqrt(mv.w * mv.h);
    }
  }

  float mean_motion_magnitude = VectorAverage(motion_magnitudes);
  float mean_motion_x = VectorAverage(motion_x);
  float mean_motion_y = VectorAverage(motion_y);
  float mean_motion_norm =
      std::sqrt(mean_motion_x * mean_motion_x + mean_motion_y * mean_motion_y);

  for (int i = 0; i < input_mv_number; ++i) {
    mean_absolute_deviation +=
        std::sqrt(std::pow(motion_x[i] - mean_motion_x, 2.0) +
                  std::pow(motion_y[i] - mean_motion_y, 2.0));

    if (mean_motion_norm > kEpsilon && motion_magnitudes[i] > kEpsilon) {
      mean_angle += std::acos(
          (motion_x[i] * mean_motion_x + motion_y[i] * mean_motion_y) /
          (motion_magnitudes[i] * mean_motion_norm));
      mean_angle_wrt_max +=
          std::acos((motion_x[i] * max_motion_x + motion_y[i] * max_motion_y) /
                    (motion_magnitudes[i] * max_motion_magnitude));
    }
  }

  mean_absolute_deviation /= input_mv_number;
  mean_angle /= input_mv_number;
  mean_angle_wrt_max /= input_mv_number;

  for (int i = 0; i < input_mv_number; ++i) {
    magnitude_mean_absolute_deviation +=
        std::abs(motion_magnitudes[i] - mean_motion_magnitude);
    magnitude_standard_deviation +=
        std::pow(motion_magnitudes[i] - mean_motion_magnitude, 2.0);
  }

  magnitude_mean_absolute_deviation /= input_mv_number;
  magnitude_standard_deviation =
      std::sqrt(magnitude_standard_deviation / input_mv_number);

  float mean_nonzero_block_size = nonzero_block_size / 16.0 / nonzero_mv_number;
  float mean_block_size = total_block_size / 16.0 / input_mv_number;
  float magnitude_entropy = EstimateEntropy(
      motion_magnitudes, mean_motion_magnitude, magnitude_standard_deviation);
  float mv_density = static_cast<float>(input_mv_number) * 256 * num_grid *
                     num_grid / (frame_height * frame_width);
  float weighted_mv_density = weighted_mv_number * 256 * num_grid * num_grid /
                              (frame_height * frame_width);
  float nonzero_mv_density = nonzero_mv_number * 256 * num_grid * num_grid /
                             (frame_height * frame_width);
  std::vector<float> features = {max_motion_magnitude,
                                 mean_motion_magnitude,
                                 mean_motion_norm,
                                 mean_absolute_deviation,
                                 mean_angle,
                                 mean_angle_wrt_max,
                                 mean_block_size,
                                 mean_nonzero_block_size,
                                 magnitude_mean_absolute_deviation,
                                 magnitude_standard_deviation,
                                 magnitude_entropy,
                                 mv_density,
                                 weighted_mv_density,
                                 nonzero_mv_density};
  return features;
}

absl::Status ComputeMaxMagnitudeEntropyWithSpatialGrid(
    const MotionVectors& motion_vectors,
    gtl::CircularBuffer<Array3D<float>>& grids_features_buffer, int num_grid,
    int frame_height, int frame_width, std::vector<float>& mv_features) {
  if (num_grid <= 0 || frame_height <= 0 || frame_width <= 0) {
    return absl::InvalidArgumentError(
        "One of grid number, frame height or width is less than or equal to "
        "zero");
  }

  int featureNum = 1;

  int grid_height = static_cast<int>(std::ceil(frame_height / num_grid));
  int grid_width = static_cast<int>(std::ceil(frame_width / num_grid));
  int mv_features_size = num_grid * num_grid * featureNum;

  if (mv_features.size() != mv_features_size) {
    return absl::InvalidArgumentError(
        absl::StrFormat("mv feature size should be %i, got %i",
                        mv_features_size, mv_features.size()));
  }
  std::fill(mv_features.begin(), mv_features.end(), 0.0);

  Array3D<float> feature_average(SHARE_WITH_FOREIGN_INSTANCE, num_grid,
                                 num_grid, featureNum, mv_features.data());

  Array3D<float> features_from_all_grids(num_grid, num_grid, featureNum, 0.0);

  // Finds the motion vector indexes for each grid in array `motion_vectors`.
  Array2D<std::vector<int>> grid_mv_idx(num_grid, num_grid);
  for (int i = 0; i < motion_vectors.size(); ++i) {
    const MotionVector& mv = motion_vectors[i];
    int grid_x = mv.dst_x / grid_width;
    int grid_y = mv.dst_y / grid_height;
    if (grid_x >= num_grid || grid_y >= num_grid) continue;
    grid_mv_idx(grid_y, grid_x).push_back(i);
  }

  for (int h = 0; h < num_grid; ++h) {
    for (int w = 0; w < num_grid; ++w) {
      // Store the motion vectors in this particular grid.
      std::vector<MotionVector> grid_motion_vectors;
      grid_motion_vectors.reserve(grid_mv_idx(h, w).size());
      for (int i = 0; i < grid_mv_idx(h, w).size(); ++i) {
        grid_motion_vectors.push_back(motion_vectors[grid_mv_idx(h, w)[i]]);
      }

      // Compute the features for a particular grid.
      std::vector<float> features_from_one_grid =
          ComputeMaxMagnitudeEntropyFeatures(grid_motion_vectors, num_grid,
                                             frame_height, frame_width);

      for (int k = 0; k < featureNum; ++k) {
        features_from_all_grids(h, w, k) = features_from_one_grid[k];
      }
    }
  }

  // Update the temportal grid feature buffer except for the I frame.
  if (!motion_vectors.empty()) {
    grids_features_buffer.push_back(features_from_all_grids);
  }

  for (int h = 0; h < num_grid; ++h) {
    for (int w = 0; w < num_grid; ++w) {
      for (int k = 0; k < featureNum; ++k) {
        // Update the running average.
        float average_sum = 0;
        for (int j = 0; j < grids_features_buffer.size(); ++j) {
          average_sum += grids_features_buffer.at(j)(h, w, k);
        }

        feature_average(h, w, k) = average_sum / grids_features_buffer.size();
      }
    }
  }

  return absl::OkStatus();
}

std::vector<float> ComputeMaxMagnitudeEntropyFeatures(
    const MotionVectors& motion_vectors, int num_grid, int frame_height,
    int frame_width) {
  // MV number
  int input_mv_number = motion_vectors.size();
  // Motion magnitudes
  std::vector<float> motion_magnitudes(input_mv_number, 0);

  // First and higher order spread of magnitudes
  float magnitude_standard_deviation = 0;

  for (int i = 0; i < input_mv_number; ++i) {
    const MotionVector& mv = motion_vectors[i];
    float mv_x =
        static_cast<float>(mv.motion_x) / mv.motion_scale / frame_width;
    float mv_y =
        static_cast<float>(mv.motion_y) / mv.motion_scale / frame_height;

    float motion_magnitude =
        std::sqrt(std::pow(mv_x, 2.0) + std::pow(mv_y, 2.0));
    motion_magnitudes[i] = motion_magnitude;
  }

  float mean_motion_magnitude = VectorAverage(motion_magnitudes);

  for (int i = 0; i < input_mv_number; ++i) {
    magnitude_standard_deviation +=
        std::pow(motion_magnitudes[i] - mean_motion_magnitude, 2.0);
  }

  magnitude_standard_deviation =
      std::sqrt(magnitude_standard_deviation / input_mv_number);

  float magnitude_entropy = EstimateEntropy(
      motion_magnitudes, mean_motion_magnitude, magnitude_standard_deviation);

  return {magnitude_entropy};
}

}  // namespace motion_detection
}  // namespace visionai
