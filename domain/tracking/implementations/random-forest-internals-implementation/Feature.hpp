#ifndef __FEATURE_HPP__
#define __FEATURE_HPP__

#include <opencv2/core/core.hpp>

class Feature {
  public:
    Feature(cv::Point point);

    cv::Point getPoint() const { return featurePoint; }

  private:
    cv::Point featurePoint;
    cv::Scalar color;

    int squareOfDistance;
};

#endif