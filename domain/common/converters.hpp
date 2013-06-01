#ifndef __CONVERTERS_HPP__
#define __CONVERTERS_HPP__

#include <sstream>

namespace common {

  template<typename T>
    std::string toString(const T& value) {
      std::ostringstream result;
      result << value;

      return result.str();
    }

}

#endif