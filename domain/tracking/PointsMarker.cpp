#include "PointsMarker.hpp"

// Transformer name.
const std::string PointsMarker::Name = "Drawing points (testing)";

// Methods.
cv::Mat PointsMarker::process(const cv::Mat& frame) {
  cv::Mat result = frame;

  for (std::vector<cv::Point>::const_iterator it = points.begin(); it != points.end(); ++it) {
    cv::circle(result, *it, 1, cv::Scalar(0, 0, 255));
  }

  return result;
}

void PointsMarker::fill(const std::vector<cv::Point>& points) {
  this->points = points;
}
