#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
  long active_jiffies = LinuxParser::ActiveJiffies(pid_);
  long system_uptime = LinuxParser::UpTime();
  long process_uptime = LinuxParser::UpTime(pid_);

  long seconds = system_uptime - process_uptime;

  if (seconds <= 0) {
    return 0.0;
  }

  return static_cast<float>(active_jiffies / sysconf(_SC_CLK_TCK)) / seconds;
}

// TODO: Return the command that generated this process
string Process::Command() const { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }
// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() > a.CpuUtilization();
}

// TODO: Setter to PID
void Process::SetPid(int pid) { pid_ = pid; }