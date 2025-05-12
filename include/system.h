#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   // Return reference to CPU
  std::vector<Process>& Processes();  // Return list of processes
  float MemoryUtilization();          // Return memory usage
  long UpTime();                      // Return system uptime
  int TotalProcesses();               // Return total number of processes
  int RunningProcesses();             // Return actively running processes
  std::string Kernel();               // Return kernel version
  std::string OperatingSystem();      // Return OS name

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif
