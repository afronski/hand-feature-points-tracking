#ifndef __LUCAS_KANADE_TRACKER_HPP__
#define __LUCAS_KANADE_TRACKER_HPP__

#include "../../common/FrameTransformers.hpp"

class LucasKanadeTracker : public PointsAwareFrameTransformer {
  public:
    static const std::string Name;

    virtual void process(cv::Mat& frame);
    virtual void fill(const std::vector<cv::Point>& points);

  private:
    static const double Quality;
    static const double MinimalDistanceBetweenPoints;
    static const cv::Scalar DrawingColor;
    static const double CircleRadius;

    bool newPointsShouldBeAdded();
    void detectFeaturePoints();

    bool pointShouldBeAccepted(unsigned int number);
    void drawTrackedPoints(cv::Mat& frame);

    std::vector<cv::Point2f> points;

    std::vector<cv::Point2f> features;
    std::vector<cv::Point2f> salientPoints[2];

    std::vector<float> errors;
    std::vector<unsigned char> status;

    unsigned int maximumFeaturesCount;

    cv::Mat actualGrayFrame;
    cv::Mat previousGrayFrame;
};

#endif