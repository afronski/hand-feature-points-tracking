#ifndef __BACKGROUND_EXTRACTOR_HPP__
#define __BACKGROUND_EXTRACTOR_HPP__

#include <opencv2/opencv.hpp>

#include "../../common/FrameTransformers.hpp"

class BackgroundExtractor : public PointsAwareFrameTransformer {
  public:
    static const std::string Name;

    virtual void process(cv::Mat& frame);
    virtual void fill(const std::vector<cv::Point>& points);

  private:
    cv::BackgroundSubtractorMOG mixtureOfGaussian;
    cv::Mat foreground;
};

#endif