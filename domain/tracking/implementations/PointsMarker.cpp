#include <fstream>
#include <iostream>

#include "../../common/vision.hpp"
#include "../../common/path.hpp"

#include "PointsMarker.hpp"

// Constants.
const std::string PointsMarker::Name = "Drawing points (testing)";
const cv::Scalar PointsMarker::DrawingColor = cv::Scalar(0, 0, 255);

// Methods.
void PointsMarker::process(cv::Mat& frame) {
  if (!points.empty()) {
    std::vector<cv::Point>::iterator previous = points.end();

    for (std::vector<cv::Point>::iterator it = points.begin(); it != points.end(); ++it) {
      common::vision::draw_cross(frame, *it, DrawingColor, 3);

      if (previous != points.end()) {
        cv::line(frame, *previous, *it, DrawingColor);
      }

      cv::circle(frame, *it, radius, DrawingColor, 1);

      previous = it;
    }
  }
}

void PointsMarker::fill(const std::vector<std::string>& arguments) {
  std::string keypointsFilename = common::path::extractFileName(arguments[0]) + ".keypoints";

  if (common::path::fileExists(keypointsFilename)) {
    std::ifstream input(keypointsFilename.c_str());

    unsigned int n = 0;
    double x = 0.0, y = 0.0;

    input >> radius;
    input >> n;

    while (n > 0) {
      input >> x >> y;
      points.push_back(cv::Point(x, y));

      --n;
    }

    input.close();
  }
}
