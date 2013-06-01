#ifndef __FRAME_TRANSFORMER_HPP__
#define __FRAME_TRANSFORMER_HPP__

#include <map>
#include <vector>
#include <string>

#include <opencv2/core/core.hpp>

#include "converters.hpp"
#include "Timer.hpp"

typedef std::map<std::string, std::string> Dictionary;

class FrameTransformer {
  public:
    virtual ~FrameTransformer() {}
    virtual void process(cv::Mat& frame) = 0;

    virtual void beforeFrame(cv::Mat& frame);
    virtual void afterFrame(cv::Mat& frame);

    virtual Dictionary getResults() const;

    virtual void handleFirstFrame(const cv::Mat& frame) {};
    virtual void handleMovieName(const std::string& movieName) {};

    virtual void afterInitialization() {};

  protected:
    common::Timer processingFrameTimer;
    std::vector<double> processedFramesTiming;
};

class ArgumentsAwareFrameTransformer : public FrameTransformer {
  public:
    virtual ~ArgumentsAwareFrameTransformer() {}
    virtual void fill(const std::vector<std::string>& arguments) = 0;
};

#endif