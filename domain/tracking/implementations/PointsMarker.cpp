#include "../../common/vision.hpp"

#include "PointsMarker.hpp"

// Transformer name.
const std::string PointsMarker::Name = "Drawing points (testing)";

// Methods.
void PointsMarker::process(cv::Mat& frame) {
  for (std::vector<cv::Point>::const_iterator it = points.begin(); it != points.end(); ++it) {
    common::vision::draw_cross(frame, *it, cv::Scalar(0, 0, 255), 3);
  }
}

void PointsMarker::fill(const std::vector<cv::Point>& points) {
  this->points = points;
}
