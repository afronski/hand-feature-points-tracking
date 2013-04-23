#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "BackgroundExtractor.hpp"

// Transformer constants.
const std::string BackgroundExtractor::Name = "Background Extractor";

// Abstract class implementation.
void BackgroundExtractor::process(cv::Mat& frame) {
  mixtureOfGaussian(frame, foreground, 0.01);

  cv::threshold(foreground, frame, 128, 255, cv::THRESH_BINARY_INV);

  cv::imshow("Foreground", foreground);
  cv::imshow("Subtracted from frame", frame);

  cv::waitKey(100);
}

void BackgroundExtractor::fill(const std::vector<cv::Point>& points) {
  cv::namedWindow("Foreground");
  cv::namedWindow("Subtracted from frame");
}