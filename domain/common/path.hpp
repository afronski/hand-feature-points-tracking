#ifndef __PATH_HPP__
#define __PATH_HPP__

#include <string>

namespace common {
  namespace path {

    std::string extractFileName(const std::string& input);

    std::string extractBaseName(const std::string& input);

    bool fileExists(const std::string& input);

    bool directoryExists(const std::string& input);

    void makeDir(const std::string& input);
  }
}

#endif