#ifndef __FEATURE_HPP__
#define __FEATURE_HPP__

#include <string>

#include <opencv2/core/core.hpp>

#include "AffineTransformation.hpp"

class Feature {
  public:
    Feature(const cv::Point& point);
    Feature(const std::string& path);

    Feature& operator=(const Feature& rhs);
    bool operator==(const Feature& rhs);

    cv::Point getPoint() const { return featurePoint; }

    void save(const std::string& path) const;

    void draw(cv::Mat& frame) const;
    void draw(cv::Mat& frame, const AffineTransformation& transformation) const;

    bool correctPointByBoundary(const cv::Size& boundary);

  private:
    cv::Point featurePoint;
    cv::Scalar color;

    int squareOfDistance;
};

#endif