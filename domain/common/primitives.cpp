#include <opencv2/core/core_c.h>

#include "primitives.hpp"

void drawCross(cv::Mat& image, const cv::Point& center, const cv::Scalar& color, int size) {
  cv::line(image, cv::Point(center.x - size, center.y - size),
                  cv::Point(center.x + size, center.y + size), color, 2, CV_AA, 0);
  cv::line(image, cv::Point(center.x + size, center.y - size),
                  cv::Point(center.x - size, center.y + size), color, 2, CV_AA, 0);
}