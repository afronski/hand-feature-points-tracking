#include <cstdlib>

#ifdef WIN32
  #include <windows.h>
#else
  #include <sys/time.h>
#endif

#include "Timer.hpp"

namespace common {

  // Internal implementation.
  struct Timer::PIMPL {
    double startTimeInMicroseconds;
    double endTimeInMicroseconds;

    bool stopped;

    #ifdef WIN32
      LARGE_INTEGER frequency;
      LARGE_INTEGER startCount;
      LARGE_INTEGER endCount;
    #else
      timeval startCount;
      timeval endCount;
    #endif
  };

  // Constructor and destructor.
  Timer::Timer(): implementation(new PIMPL()) {
    #ifdef WIN32
      QueryPerformanceFrequency(&implementation->frequency);

      implementation->startCount.QuadPart = 0;
      implementation->endCount.QuadPart = 0;
    #else
      implementation->startCount.tv_sec = implementation->startCount.tv_usec = 0;
      implementation->endCount.tv_sec = implementation->endCount.tv_usec = 0;
    #endif

    implementation->stopped = false;

    implementation->startTimeInMicroseconds = 0;
    implementation->endTimeInMicroseconds = 0;
  }

  Timer::~Timer() {
    if (implementation != 0) {
      delete implementation;
    }
  }

  // Public methods.
  void Timer::start() {
    implementation->stopped = false;

    #ifdef WIN32
      QueryPerformanceCounter(&implementation->startCount);
    #else
      gettimeofday(&implementation->startCount, 0);
    #endif
  }

  void Timer::stop() {
    implementation->stopped = true;

    #ifdef WIN32
      QueryPerformanceCounter(&implementation->endCount);
    #else
      gettimeofday(&implementation->endCount, 0);
    #endif
  }

  double Timer::getElapsedTimeInMicroseconds() {
    #ifdef WIN32
      if (!implementation->stopped) {
        QueryPerformanceCounter(&implementation->endCount);
      }

      implementation->startTimeInMicroseconds = implementation->startCount.QuadPart *
                                            (1000000.0 / implementation->frequency.QuadPart);
      implementation->endTimeInMicroseconds = implementation->endCount.QuadPart *
                                          (1000000.0 / implementation->frequency.QuadPart);
    #else
      if (!implementation->stopped) {
        gettimeofday(&implementation->endCount, NULL);
      }

      implementation->startTimeInMicroseconds = (implementation->startCount.tv_sec * 1000000.0) +
                                            implementation->startCount.tv_usec;
      implementation->endTimeInMicroseconds = (implementation->endCount.tv_sec * 1000000.0) +
                                          implementation->endCount.tv_usec;
    #endif

    return implementation->endTimeInMicroseconds - implementation->startTimeInMicroseconds;
  }

  double Timer::getElapsedTimeInMilliseconds() {
    return getElapsedTimeInMicroseconds() * 0.001;
  }

  double Timer::getElapsedTimeInSeconds() {
    return getElapsedTimeInMicroseconds() * 0.000001;
  }

}