#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

namespace common {
  namespace debug {

    extern bool printEnabled;

    void log(const char* format, ...);
    void print(const char* format, ...);

  }
}

#endif