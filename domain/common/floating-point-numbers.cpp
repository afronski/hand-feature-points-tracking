#include <cmath>
#include <cfloat>

#include "floating-point-numbers.hpp"

namespace common {

  bool isGreaterOrEqualThanZero(double value) {
    if (value > 0) {
      return true;
    }

    if (std::abs(value) <= DBL_EPSILON) {
      return true;
    }

    return false;
  }

  bool isLessOrEqualThanZero(double value) {
    if (value < 0) {
      return true;
    }

    if (std::abs(value) <= DBL_EPSILON) {
      return true;
    }

    return false;
  }

  bool isEqualToZero(double value) {
    return std::abs(value) <= DBL_EPSILON;
  }

}