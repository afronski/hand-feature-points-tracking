#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

namespace common {

  class Timer {
    public:
      Timer();
      ~Timer();

      void start();
      void stop();

      double getElapsedTimeInSeconds();
      double getElapsedTimeInMilliseconds();
      double getElapsedTimeInMicroseconds();

    private:
      struct PIMPL;
      PIMPL* implementation;
  };

}


#endif