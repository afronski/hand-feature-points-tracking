#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>

#include "HornSchunckTracker.hpp"

// Transformer constants.
const std::string HornSchunckTracker::Name = "Optical Flow (Horn-Schunck)";

const double HornSchunckTracker::Quality = 0.01;
const double HornSchunckTracker::MinimalDistanceBetweenPoints = 10.0;

const cv::Scalar HornSchunckTracker::DrawingColor = cv::Scalar(255, 255, 255);
const double HornSchunckTracker::CircleRadius = 3.0;

// Private implementation methods.
bool HornSchunckTracker::newPointsShouldBeAdded() {
  return salientPoints[0].size() <= 500;
}

void HornSchunckTracker::detectFeaturePoints() {
  cv::goodFeaturesToTrack(actualGrayFrame, features, 500, Quality, MinimalDistanceBetweenPoints);
}

bool HornSchunckTracker::pointShouldBeAccepted(unsigned int number) {
  double xDifference = std::abs(salientPoints[0][number].x - salientPoints[1][number].x),
         yDifference = std::abs(salientPoints[0][number].y - salientPoints[1][number].y);

  return status[number] && (xDifference + yDifference > 2.0);
}

void HornSchunckTracker::drawTrackedPoints(cv::Mat& frame) {
  for (unsigned int i = 0; i < salientPoints[1].size(); ++i) {
    cv::line(frame, points[i], salientPoints[1][i], DrawingColor);
    cv::circle(frame, salientPoints[1][i], CircleRadius, DrawingColor, -1);
  }
}

// Methods.
void HornSchunckTracker::process(cv::Mat& frame) {
  cv::cvtColor(frame, actualGrayFrame, CV_BGR2GRAY);

  if (newPointsShouldBeAdded()) {
    detectFeaturePoints();

    salientPoints[0].insert(salientPoints[0].end(), features.begin(), features.end());
    points.insert(points.end(), features.begin(), features.end());
  }

  if (previousGrayFrame.empty()) {
    actualGrayFrame.copyTo(previousGrayFrame);
  }

  cv::calcOpticalFlowHS(previousGrayFrame, actualGrayFrame, , salientPoints[0], salientPoints[1]);

  unsigned int k = 0;

  for (unsigned int i = 0; i < salientPoints[1].size(); ++i) {
    if (pointShouldBeAccepted(i)) {
      points[k] = points[i];
      salientPoints[1][k++] = salientPoints[1][i];
    }
  }

  salientPoints[1].resize(k);
  points.resize(k);

  drawTrackedPoints(frame);

  std::swap(salientPoints[1], salientPoints[0]);
  cv::swap(previousGrayFrame, actualGrayFrame);
}

void HornSchunckTracker::fill(const std::vector<cv::Point>& points) {
  for (std::vector<cv::Point>::const_iterator point = points.begin(); point != points.end(); ++point) {
    this->points.push_back(cv::Point2f(point->x, point->y));
  }

  this->maximumFeaturesCount = this->points.size();
}