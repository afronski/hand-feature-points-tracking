#include <fstream>

#include "Feature.hpp"

const std::string FileNamePostFix = "feature-point-parameters.txt";

// Internal methods.
static int square(int a) {
  return a * a;
}

// Constructors.
Feature::Feature(const cv::Point& point):
  featurePoint(point),
  color(cv::Scalar(0, 0, 255, 125)),
  squareOfDistance(25)
{}

Feature::Feature(const std::string& path):
  featurePoint(cv::Point(-1, -1)),
  color(cv::Scalar(0, 0, 255, 125)),
  squareOfDistance(25) {
  std::ifstream file((path + FileNamePostFix).c_str());

  file >> featurePoint.x >> featurePoint.y;
  file.close();
}

// Public methods.
void Feature::save(const std::string& path) const {
  std::ofstream file((path + FileNamePostFix).c_str());

  file << featurePoint.x << " " << featurePoint.y;
  file.close();
}

bool Feature::operator==(const Feature& rhs) {
  return square(rhs.featurePoint.x - featurePoint.x) + square(rhs.featurePoint.y - featurePoint.y) <= squareOfDistance;
}

Feature& Feature::operator=(const Feature& rhs) {
  featurePoint = rhs.featurePoint;
  return *this;
}

void Feature::draw(cv::Mat& frame, const AffineTransformation& transformation) const {
  cv::Point transformedPoint = transformation.transformPoint(featurePoint);
  cv::Size ellipseSize;

  ellipseSize.height = frame.cols / 100;
  ellipseSize.width = frame.cols / 100;

  cv::ellipse(frame, transformedPoint, ellipseSize, 0, 0, 360, color);
  cv::circle(frame, featurePoint, 2.0, color, -1);
}

void Feature::draw(cv::Mat& frame) const {
  cv::Size ellipseSize;

  ellipseSize.height = frame.cols / 100;
  ellipseSize.width = frame.cols / 100;

  cv::ellipse(frame, featurePoint, ellipseSize, 0, 0, 360, color);
  cv::circle(frame, featurePoint, 2.0, color, -1);
}

bool Feature::correctPointByBoundary(const cv::Size& boundary) {
  if (featurePoint.x <= boundary.width && featurePoint.x >= 0 &&
      featurePoint.y <= boundary.height && featurePoint.y >= 0) {
    return true;
  }

  if (featurePoint.x > boundary.width) {
    if (square(boundary.width - featurePoint.x) > squareOfDistance) {
      return false;
    } else {
      featurePoint.x = boundary.width;
    }
  }

  if (featurePoint.x < 0) {
    if (square(featurePoint.x) > squareOfDistance) {
      return false;
    } else {
      featurePoint.x = 0;
    }
  }

  if (featurePoint.y > boundary.height) {
    if (square(boundary.height - featurePoint.y) > squareOfDistance) {
      return false;
    } else {
      featurePoint.y = boundary.height;
    }
  }

  if (featurePoint.y < 0) {
    if (square(featurePoint.y) > squareOfDistance) {
      return false;
    } else {
      featurePoint.y = 0;
    }
  }

  return true;
}