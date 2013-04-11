#ifndef __FRAME_TRANSFORMER_HPP__
#define __FRAME_TRANSFORMER_HPP__

#include <vector>

#include <opencv2/core/core.hpp>

class FrameTransformer {
  public:
    virtual ~FrameTransformer() {}
    virtual void process(cv::Mat& frame) = 0;

    virtual void beforeFrame(cv::Mat& frame) {};
    virtual void afterFrame(cv::Mat& frame) {};
};

class PointsAwareFrameTransformer : public FrameTransformer {
  public:
    virtual ~PointsAwareFrameTransformer() {}
    virtual void fill(const std::vector<cv::Point>& points) = 0;
};

#endif