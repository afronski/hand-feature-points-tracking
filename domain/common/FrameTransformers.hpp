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

    virtual void printResults() {};

    virtual void handleFirstFrame(const cv::Mat& frame) {};
    virtual void handleMovieName(const std::string& movieName) {};
};

class ArgumentsAwareFrameTransformer : public FrameTransformer {
  public:
    virtual ~ArgumentsAwareFrameTransformer() {}
    virtual void fill(const std::vector<std::string>& arguments) = 0;
};

#endif