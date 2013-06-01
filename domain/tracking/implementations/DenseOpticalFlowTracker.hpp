#ifndef __DENSE_OPTICAL_FLOW_TRACKER_HPP__
#define __DENSE_OPTICAL_FLOW_TRACKER_HPP__

#include "../../common/Timer.hpp"

#include "../../common/FrameTransformers.hpp"

class DenseOpticalFlowTracker : public ArgumentsAwareFrameTransformer {
  public:
    DenseOpticalFlowTracker(): mapOverlayStep(30), windowSize(5), iterations(5) {}

    static const std::string Name;

    virtual void process(cv::Mat& frame);
    virtual void fill(const std::vector<cv::string>& arguments);

    virtual void beforeFrame(cv::Mat& frame);
    virtual void afterFrame(cv::Mat& frame);

    virtual Dictionary getResults() const;

  private:
    common::Timer timerForDrawing;
    std::vector<double> drawingTimeOverhead;

    unsigned int mapOverlayStep;
    int windowSize;
    int iterations;

    static const cv::Scalar MapOverlayColor;
    static const double MapOverlayPixelSize;

    static const double PyramidScale;
    static const int PyramidLevels;

    static const int Neighbourhood;

    static const double StandardDeviationForGaussian;

    void drawOpticalFlowMap(cv::Mat& frame);

    cv::Mat flow;

    cv::Mat actualGrayFrame;
    cv::Mat previousGrayFrame;
};

#endif