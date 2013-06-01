#ifndef __POINTS_MARKER_HPP__
#define __POINTS_MARKER_HPP__

#include "../../common/FrameTransformers.hpp"

class PointsMarker : public ArgumentsAwareFrameTransformer {
  public:
    static const std::string Name;
    static const cv::Scalar DrawingColor;

    virtual void process(cv::Mat& frame);
    virtual void fill(const std::vector<std::string>& arguments);

    virtual void beforeFrame(cv::Mat& frame);
    virtual void afterFrame(cv::Mat& frame);

    virtual Dictionary getResults() const;

  private:
    std::vector<cv::Point> points;
    double radius;
};

#endif