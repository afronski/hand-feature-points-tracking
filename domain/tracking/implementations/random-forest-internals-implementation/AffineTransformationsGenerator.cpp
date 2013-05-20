#include "AffineTransformationsGenerator.hpp"

// Constructor.
AffineTransformationsGenerator::AffineTransformationsGenerator(cv::Point centerPoint):
  center(centerPoint),
  isValid(true) {
  parametersMinValue[Angle] = -30;
  parametersMinValue[ScaleX] = 0.2;

  parametersMinValue[SkewFactorX] = 0.0;
  parametersMinValue[SkewFactorY] = 0.0;

  parametersStep[Angle] = 10;
  parametersStep[ScaleX] = 0.2;

  parametersStep[SkewFactorX] = 0.1;
  parametersStep[SkewFactorY] = 0.1;

  parametersMaxValue[Angle] = 31;
  parametersMaxValue[ScaleX] = 1.1;

  parametersMaxValue[SkewFactorX] = 0.41;
  parametersMaxValue[SkewFactorY] = 0.41;

  for (int i = 0; i < AffineOperationTypeSize; ++i) {
    parameters[i] = parametersMinValue[i];
  }
}

// Public method.
bool AffineTransformationsGenerator::getNextTransformation(AffineTransformation* next) {
  next = new AffineTransformation(
                parameters[Angle],
                center,
                parameters[ScaleX],
                parameters[ScaleX],
                parameters[SkewFactorX],
                parameters[SkewFactorY]);

  isValid &= adjustNextParamsValues();
  return isValid;
}

// Private method.
bool AffineTransformationsGenerator::adjustNextParamsValues() {
  bool shouldAdjustNext = false;

  for (int i = 0; i < AffineOperationTypeSize; ++i) {
    if( parameters[i] + parametersStep[i] > parametersMaxValue[i] ) {
      parameters[i] = parametersMinValue[i];
      shouldAdjustNext = true;
    } else {
      parameters[i] += parametersStep[i];
      shouldAdjustNext = false;
      break;
    }
  }

  if (shouldAdjustNext) {
    return false;
  }

  return true;
}