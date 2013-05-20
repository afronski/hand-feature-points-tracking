#include <fstream>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>

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

    bool directoryExists(const std::string& input) {
      struct stat status;
      int error;

      #if defined(_WIN32)
        error = _stat(input.c_str(), &status);
      #else
        error = stat(input.c_str(), &status);
      #endif

      if (error != 0) {
        throw new std::runtime_error("Cannot perform stat on specified directory!");
      }

      return S_ISDIR(status.st_mode);
    }

    void makeDir(const std::string& input) {
      int error;

      #if defined(_WIN32)
        error = _mkdir(input.c_str());
      #else
        error = mkdir(input.c_str(), 0777);
      #endif

      if (error != 0) {
        throw new std::runtime_error("Cannot create specified directory!");
      }
    }

  }
}