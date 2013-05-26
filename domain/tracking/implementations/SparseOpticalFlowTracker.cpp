#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>

#include "SparseOpticalFlowTracker.hpp"

// Transformer constants.
const std::string SparseOpticalFlowTracker::Name = "Sparse Optical Flow";

const double SparseOpticalFlowTracker::Quality = 0.01;

const cv::Scalar SparseOpticalFlowTracker::DrawingColor = cv::Scalar(255, 255, 255);
const double SparseOpticalFlowTracker::CircleRadius = 3.0;

// Private implementation methods.
bool SparseOpticalFlowTracker::newPointsShouldBeAdded() {
  return salientPoints[0].size() <= maximumFeaturesCount;
}

void SparseOpticalFlowTracker::detectFeaturePoints() {
  cv::goodFeaturesToTrack(
    actualGrayFrame,
    features,
    maximumFeaturesCount,
    Quality,
    minimalDistanceBetweenPoints);
}

bool SparseOpticalFlowTracker::pointShouldBeAccepted(unsigned int number) {
  double xDifference = std::abs(salientPoints[0][number].x - salientPoints[1][number].x),
         yDifference = std::abs(salientPoints[0][number].y - salientPoints[1][number].y);

  return status[number] && (xDifference + yDifference > 2.0);
}

void SparseOpticalFlowTracker::drawTrackedPoints(cv::Mat& frame) {
  for (unsigned int i = 0; i < salientPoints[1].size(); ++i) {
    cv::line(frame, points[i], salientPoints[1][i], DrawingColor);
    cv::circle(frame, salientPoints[1][i], CircleRadius, DrawingColor, -1);
  }
}

// Methods.
void SparseOpticalFlowTracker::process(cv::Mat& frame) {
  cv::cvtColor(frame, actualGrayFrame, CV_BGR2GRAY);

  if (newPointsShouldBeAdded()) {
    detectFeaturePoints();

    salientPoints[0].insert(salientPoints[0].end(), features.begin(), features.end());
    points.insert(points.end(), features.begin(), features.end());
  }

  if (previousGrayFrame.empty()) {
    actualGrayFrame.copyTo(previousGrayFrame);
  }

  cv::calcOpticalFlowPyrLK(previousGrayFrame, actualGrayFrame, salientPoints[0], salientPoints[1], status, errors);

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

void SparseOpticalFlowTracker::fill(const std::vector<std::string>& arguments) {
  if (arguments.size() > 2) {
    std::stringstream forConversion(arguments[2]);
    forConversion >> minimalDistanceBetweenPoints;
  }

  if (arguments.size() > 3) {
    std::stringstream forConversion(arguments[3]);
    forConversion >> maximumFeaturesCount;
  }
}