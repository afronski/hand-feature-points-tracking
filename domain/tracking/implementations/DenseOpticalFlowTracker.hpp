#ifndef __DENSE_OPTICAL_FLOW_TRACKER_HPP__
#define __DENSE_OPTICAL_FLOW_TRACKER_HPP__

#include "../../common/FrameTransformers.hpp"

class DenseOpticalFlowTracker : public ArgumentsAwareFrameTransformer {
  public:
    static const std::string Name;

    virtual void process(cv::Mat& frame);
    virtual void fill(const std::vector<cv::string>& arguments);

  private:
    static const cv::Scalar MapOverlayColor;
    static const double MapOverlayPixelSize;
    static const unsigned int MapOverlayStep;

    static const double PyramidScale;
    static const int PyramidLevels;

    static const int WindowSize;
    static const int Iterations;

    static const int Neighbourhood;

    static const double StandardDeviationForGaussian;

    void drawOpticalFlowMap(cv::Mat& frame);

    cv::Mat flow;

    cv::Mat actualGrayFrame;
    cv::Mat previousGrayFrame;
};

#endif