#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>

#include "DenseOpticalFlowTracker.hpp"

// Transformer constants.
const std::string DenseOpticalFlowTracker::Name = "Dense Optical Flow";

const cv::Scalar DenseOpticalFlowTracker::MapOverlayColor = cv::Scalar(255, 0, 0);
const double DenseOpticalFlowTracker::MapOverlayPixelSize = 2.0;

const double DenseOpticalFlowTracker::PyramidScale = 0.5;
const int DenseOpticalFlowTracker::PyramidLevels = 3;

const int DenseOpticalFlowTracker::Neighbourhood = 7;

const double DenseOpticalFlowTracker::StandardDeviationForGaussian = 1.1;

// Private methods implementation.
void DenseOpticalFlowTracker::drawOpticalFlowMap(cv::Mat& frame) {
  for(int y = 0; y < frame.rows; y += mapOverlayStep) {
    for(int x = 0; x < frame.cols; x += mapOverlayStep) {
      const cv::Point2f& flowAtXY = flow.at<cv::Point2f>(y, x);
      const cv::Point2f& point = cv::Point(x, y);

      cv::line(frame, point, cv::Point(cvRound(x + flowAtXY.x), cvRound(y + flowAtXY.y)), MapOverlayColor);
      cv::circle(frame, point, MapOverlayPixelSize, MapOverlayColor, -1);
    }
  }
}

// Methods.
void DenseOpticalFlowTracker::process(cv::Mat& frame) {
  cv::cvtColor(frame, actualGrayFrame, CV_BGR2GRAY);

  if (previousGrayFrame.empty()) {
    previousGrayFrame = actualGrayFrame.clone();
  }

  flow = cv::Mat(actualGrayFrame.size(), CV_32FC2);

  cv::calcOpticalFlowFarneback(
    previousGrayFrame,
    actualGrayFrame,
    flow,
    PyramidScale,
    PyramidLevels,
    windowSize,
    iterations,
    Neighbourhood,
    StandardDeviationForGaussian,
    cv::OPTFLOW_FARNEBACK_GAUSSIAN);

  drawOpticalFlowMap(frame);

  previousGrayFrame = actualGrayFrame.clone();
}

void DenseOpticalFlowTracker::fill(const std::vector<std::string>& arguments) {
  if (arguments.size() > 2) {
    std::stringstream forConversion(arguments[2]);
    forConversion >> mapOverlayStep;
  }

  if (arguments.size() > 3) {
    std::stringstream forConversion(arguments[3]);
    forConversion >> windowSize;
  }

  if (arguments.size() > 4) {
    std::stringstream forConversion(arguments[4]);
    forConversion >> iterations;
  }
}