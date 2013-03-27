#ifndef __FRAME_TRANSFORMER_HPP__
#define __FRAME_TRANSFORMER_HPP__

#include <vector>

#include <opencv2/core/core.hpp>

class FrameTransformer {
  public:
    virtual ~FrameTransformer() {}
    virtual cv::Mat process(const cv::Mat& frame) = 0;
};

class PointsAwareFrameTransformer : public FrameTransformer {
  public:
    virtual ~PointsAwareFrameTransformer() {}
    virtual void fill(const std::vector<cv::Point>& points) = 0;
};

#endif