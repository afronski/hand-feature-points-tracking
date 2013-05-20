#include "Feature.hpp"

Feature::Feature(cv::Point point):
  featurePoint(point),
  color(cv::Scalar(0, 0, 255, 125)),
  squareOfDistance(25)
{}