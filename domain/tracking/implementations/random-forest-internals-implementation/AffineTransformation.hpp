#ifndef __AFFINE_TRANSFORMATION_HPP__
#define __AFFINE_TRANSFORMATION_HPP__

#include <opencv2/core/core.hpp>

class AffineTransformation {
  public:
    AffineTransformation(
      double angle,
      cv::Point center,
      double scaleX,
      double scaleY,
      double skewFactorX,
      double skewFactorY);

    AffineTransformation(const cv::Mat& transformation, bool shouldBeInverted);

    cv::Point transformPoint(const cv::Point& point) const;

    void transformImage(const cv::Mat& inputImage, cv::Mat& transformedImage, cv::Size boundary) const;
    void transformImage(const cv::Mat& inputImage, cv::Mat* transformedImage, cv::Mat* transformMatrix);

  private:
    cv::Mat transformationMatrix;

    void initializeTransformation(
          double angle,
          cv::Point center,
          double scaleX,
          double scaleY,
          double skewFactorX,
          double skewFactorY);

    cv::Point2d multiplyMatrixWithVector(const cv::Point2d& vector) const;
    void initiateByInversedTransformMatrix(const cv::Mat& matrix);
};

#endif