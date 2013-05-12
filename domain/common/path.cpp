#include <fstream>

#include "path.hpp"

namespace common {
  namespace path {

    std::string extractFileName(const std::string& input) {
      size_t index = input.find_last_of(".");

      if (index != std::string::npos) {
        return input.substr(0, index);
      }

      return input;
    }

    bool fileExists(const std::string& input) {
      std::ifstream ifile(input.c_str());

      return ifile;
    }

  }
}