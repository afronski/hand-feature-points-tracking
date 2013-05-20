#ifndef __AFFINE_TRANSFORMATIONS_GENERATOR_HPP__
#define __AFFINE_TRANSFORMATIONS_GENERATOR_HPP__

#include <opencv2/core/core.hpp>

#include "AffineTransformation.hpp"

class AffineTransformationsGenerator {
  public:
    AffineTransformationsGenerator(cv::Point center);

    bool getNextTransformation(AffineTransformation* next);
};

#endif