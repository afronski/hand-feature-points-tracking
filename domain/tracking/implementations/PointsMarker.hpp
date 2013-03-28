#ifndef __POINTS_MARKER_HPP__
#define __POINTS_MARKER_HPP__

#include "../../common/FrameTransformers.hpp"

class PointsMarker : public PointsAwareFrameTransformer {
  public:
    static const std::string Name;

    virtual void process(cv::Mat& frame);
    virtual void fill(const std::vector<cv::Point>& points);

  private:
    std::vector<cv::Point> points;
};

#endif