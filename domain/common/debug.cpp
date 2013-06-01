#include <cstdarg>
#include <cstdio>
#include <iostream>

#include "debug.hpp"

namespace common {
  namespace debug {

    bool printEnabled = true;

    void log(const char* format, ...) {
      #ifdef DEBUG
        if (printEnabled) {
          char buffer[256];

          std::va_list arguments;

          va_start(arguments, format);
            vsprintf(buffer, format, arguments);
          va_end(arguments);

          std::cout << buffer << std::flush;
        }
      #endif
    }

    void print(const char* format, ...) {
      if (printEnabled) {
        char buffer[256];

        std::va_list arguments;

        va_start(arguments, format);
          vsprintf(buffer, format, arguments);
        va_end(arguments);

        std::cout << buffer << std::flush;
      }
    }

  }
}