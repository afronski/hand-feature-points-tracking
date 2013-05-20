#ifndef __AFFINE_TRANSFORMATIONS_GENERATOR_HPP__
#define __AFFINE_TRANSFORMATIONS_GENERATOR_HPP__

#include <opencv2/core/core.hpp>

#include "AffineTransformation.hpp"

class AffineTransformationsGenerator {
  public:
    AffineTransformationsGenerator(cv::Point center);

    bool getNextTransformation(AffineTransformation* next);

  private:
    bool adjustNextParamsValues();

    enum AffineOperationType {
      Angle = 0,
      ScaleX,

      SkewFactorX,
      SkewFactorY,

      AffineOperationTypeSize
    };

    cv::Point center;

    double parameters[AffineOperationTypeSize];
    double parametersStep[AffineOperationTypeSize];
    double parametersMaxValue[AffineOperationTypeSize];
    double parametersMinValue[AffineOperationTypeSize];

    bool isValid;
};

#endif