#ifndef __VISION_HPP__
#define __VISION_HPP__

#include <vector>

#include <opencv2/core/core.hpp>

#include "CommandLineInterface.hpp"

namespace common {
  namespace vision {

    // Primitives.
    void draw_cross(cv::Mat& image, const cv::Point& center, const cv::Scalar& color, int size);

    // Parsing input to the OpenCV structures.
    std::vector<cv::Point> extract_points_from_arguments(const common::CommandLineInterface& cli,
                                                         unsigned int start = 0);

  }
}

#endif