#ifndef WIN32
  #include <unistd.h>
#else
  #include <windows.h>
#endif

#include <fstream>
#include <string>

#include "memory-usage.hpp"

namespace common {

  void getMemoryUsage(double& virtualMemory, double& residentSet) {
    const long intoKB = 1024;

    virtualMemory = 0.0;
    residentSet = 0.0;

    #ifndef WIN32
      std::ifstream stat_stream("/proc/self/stat", std::ios_base::in);

      std::string pid, comm, state, ppid, pgrp, session, tty_nr;
      std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
      std::string utime, stime, cutime, cstime, priority, nice;
      std::string O, itrealvalue, starttime;

      unsigned long virtualMemoryAmount;
      long residentSetAmount;

      stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                 >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                 >> utime >> stime >> cutime >> cstime >> priority >> nice
                 >> O >> itrealvalue >> starttime
                 >> virtualMemoryAmount >> residentSetAmount;

      stat_stream.close();

      long pageSizeKB = sysconf(_SC_PAGE_SIZE) / intoKB;

      virtualMemory = static_cast<double>(virtualMemoryAmount) / static_cast<double>(intoKB);
      residentSet = residentSetAmount * pageSizeKB;
    #else
      MEMORYSTATUSEX state;
      state.dwLength = sizeof(state);

      GlobalMemoryStatusEx(&state);

      virtualMemory = static_cast<double>(state.ullTotalVirtual) / static_cast<double>(intoKB) -
                      static_cast<double>(state.ullAvailVirtual) / static_cast<double>(intoKB);
      residentSet = static_cast<double>(state.ullTotalPhys) / static_cast<double>(intoKB) -
                    static_cast<double>(state.ullAvailPhys) / static_cast<double>(intoKB);
    #endif
  }

}