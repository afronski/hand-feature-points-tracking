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
    FrameTransformer();
    virtual ~FrameTransformer() {};

    virtual void process(cv::Mat& frame) = 0;

    virtual void beforeFrame(cv::Mat& frame);
    virtual void afterFrame(cv::Mat& frame);

    virtual Dictionary getResults() const;

    virtual void handleFirstFrame(const cv::Mat& frame) {};
    virtual void handleMovieName(const std::string& movieName) {};

    virtual void afterInitialization() {};

    void collectAndDrawAverageTrack(
           const std::vector<cv::Point2f>& points,
           std::size_t meaningfulAmount,
           cv::Mat& frame);

    void collectAndDrawAverageTrack(
           const std::vector<cv::Point2d>& points,
           std::size_t meaningfulAmount,
           cv::Mat& frame);

  protected:
    common::Timer memoryMeasureOverheadTimer;
    common::Timer processingFrameTimer;
    common::Timer averagePointsCalculationTimer;

    double virtualMemoryAtStart;
    double residentSetAtStart;

    std::vector<double> memoryMeasureOverhead;
    std::vector<double> processedFramesTiming;

    std::vector<double> processedFramesVirtualMemoryUsage;
    std::vector<double> processedFramesResidentMemoryUsage;

    std::vector<double> averagePointsCalculationOverhead;

    std::vector<cv::Point2f> averagePoints;
    std::vector<cv::Point2d> doubleAveragePoints;
};

class ArgumentsAwareFrameTransformer : public FrameTransformer {
  public:
    virtual ~ArgumentsAwareFrameTransformer() {}
    virtual void fill(const std::vector<std::string>& arguments) = 0;
};

#endif