#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      std::string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream file(kProcDirectory + kMeminfoFilename);
  string line, key;
  float memTotal = 0, memFree = 0;
  float value;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream stream(line);
      stream >> key >> value;
      if (key == "MemTotal:")
        memTotal = value;
      else if (key == "MemFree:")
        memFree = value;
      if (memTotal > 0 && memFree > 0) break;
    }
  }

  if (memTotal == 0) return 0.0;
  return (memTotal - memFree) / memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream file(kProcDirectory + kUptimeFilename);
  string line;
  if (file.is_open() && std::getline(file, line)) {
    std::istringstream stream(line);
    float uptime;
    stream >> uptime;
    return static_cast<long>(uptime);
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> values = CpuUtilization();
  long total = 0;
  for (const string& val : values) {
    total += stol(val);
  }
  return total;
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream file(kProcDirectory + std::to_string(pid) + "/stat");
  std::string line;

  if (file.is_open() && std::getline(file, line)) {
    std::istringstream stream(line);
    std::vector<std::string> values(
        (std::istream_iterator<std::string>(stream)), {});
    if (values.size() >= 17) {
      long utime = std::stol(values[13]);
      long stime = std::stol(values[14]);
      long cutime = std::stol(values[15]);
      long cstime = std::stol(values[16]);
      return utime + stime + cutime + cstime;
    }
  }

  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> values = CpuUtilization();
  if (values.size() < 10) return 0;

  return stol(values[CPUStates::kUser_]) + stol(values[CPUStates::kNice_]) +
         stol(values[CPUStates::kSystem_]) + stol(values[CPUStates::kIRQ_]) +
         stol(values[CPUStates::kSoftIRQ_]) + stol(values[CPUStates::kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> values = CpuUtilization();
  if (values.size() < 5) return 0;

  return stol(values[CPUStates::kIdle_]) + stol(values[CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() {
  std::ifstream file(kProcDirectory + kStatFilename);
  std::string line, key;
  std::vector<std::string> values;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream stream(line);
      stream >> key;
      if (key == "cpu") {
        std::string value;
        while (stream >> value) {
          values.push_back(value);
        }
        break;
      }
    }
  }

  return values;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream file(kProcDirectory + kStatFilename);
  string line, key;
  int value;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream stream(line);
      stream >> key >> value;
      if (key == "processes") return value;
    }
  }

  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream file(kProcDirectory + kStatFilename);
  string line, key;
  int value;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream stream(line);
      stream >> key >> value;
      if (key == "procs_running") return value;
    }
  }

  return 0;
}

// TODO: Read and return the command associated with a process
std::string LinuxParser::Command(int pid) {
  std::ifstream file(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::string line;
  if (file.is_open() && std::getline(file, line)) {
    return line;
  }
  return "";
}

// TODO: Read and return the memory used by a process
std::string LinuxParser::Ram(int pid) {
  std::ifstream file(kProcDirectory + std::to_string(pid) + kStatusFilename);
  std::string line, key;
  int value;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream stream(line);
      stream >> key >> value;
      if (key == "VmSize:") {
        return std::to_string(value / 1024);  // kB → MB
      }
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
std::string LinuxParser::Uid(int pid) {
  std::ifstream file(kProcDirectory + std::to_string(pid) + kStatusFilename);
  std::string line, key, uid;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream stream(line);
      stream >> key >> uid;
      if (key == "Uid:") return uid;
    }
  }
  return "";
}

// TODO: Read and return the user associated with a process
std::string LinuxParser::User(int pid) {
  std::string uid = Uid(pid);
  std::ifstream file(kPasswordPath);
  std::string line;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      size_t delim1 = line.find(':');
      size_t delim2 = line.find(':', delim1 + 1);
      std::string name = line.substr(0, delim1);
      std::string id =
          line.substr(delim2 + 1, line.find(':', delim2 + 1) - delim2 - 1);
      if (id == uid) return name;
    }
  }

  return "";
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream file(kProcDirectory + std::to_string(pid) + "/stat");
  std::string line;

  if (file.is_open() && std::getline(file, line)) {
    std::istringstream stream(line);
    std::vector<std::string> values(
        (std::istream_iterator<std::string>(stream)), {});
    if (values.size() > 21) {
      long starttime = std::stol(values[21]);
      long hertz = sysconf(_SC_CLK_TCK);
      return UpTime() - (starttime / hertz);
    }
  }
  return 0;
}
