#ifndef __SPARSE_OPTICAL_FLOW_TRACKER_HPP__
#define __SPARSE_OPTICAL_FLOW_TRACKER_HPP__

#include "../../common/Timer.hpp"

#include "../../common/FrameTransformers.hpp"

class SparseOpticalFlowTracker : public ArgumentsAwareFrameTransformer {
  public:
    static const std::string Name;

    SparseOpticalFlowTracker():
      minimalDistanceBetweenPoints(10.0), maximumFeaturesCount(500), meaningfulAmountOfPoints(10)
    {}

    virtual void process(cv::Mat& frame);
    virtual void fill(const std::vector<std::string>& arguments);

    virtual void beforeFrame(cv::Mat& frame);
    virtual void afterFrame(cv::Mat& frame);

    virtual Dictionary getResults() const;

  private:
    double minimalDistanceBetweenPoints;
    unsigned int maximumFeaturesCount;

    static const double Quality;
    static const cv::Scalar DrawingColor;
    static const double CircleRadius;

    bool newPointsShouldBeAdded();
    void detectFeaturePoints();

    bool pointShouldBeAccepted(unsigned int number);
    void drawTrackedPoints(cv::Mat& frame);

    common::Timer timerForDrawing;
    std::vector<double> drawingTimeOverhead;

    std::vector<cv::Point2f> points;
    std::vector<cv::Point2f> features;
    std::vector<cv::Point2f> salientPoints[2];

    std::vector<float> errors;
    std::vector<unsigned char> status;

    std::size_t meaningfulAmountOfPoints;

    cv::Mat actualGrayFrame;
    cv::Mat previousGrayFrame;
};

#endif