#include "processor.h"

#include "linux_parser.h"

float Processor::Utilization() {
  std::vector<std::string> cpu_values = LinuxParser::CpuUtilization();
  if (cpu_values.size() < 10) return 0.0;

  long user = stol(cpu_values[LinuxParser::kUser_]);
  long nice = stol(cpu_values[LinuxParser::kNice_]);
  long system = stol(cpu_values[LinuxParser::kSystem_]);
  long idle = stol(cpu_values[LinuxParser::kIdle_]);
  long iowait = stol(cpu_values[LinuxParser::kIOwait_]);
  long irq = stol(cpu_values[LinuxParser::kIRQ_]);
  long softirq = stol(cpu_values[LinuxParser::kSoftIRQ_]);
  long steal = stol(cpu_values[LinuxParser::kSteal_]);

  long idle_time = idle + iowait;
  long non_idle_time = user + nice + system + irq + softirq + steal;
  long total_time = idle_time + non_idle_time;

  long total_diff = total_time - prev_total_;
  long active_diff = non_idle_time - prev_active_;

  prev_total_ = total_time;
  prev_active_ = non_idle_time;

  float current =
      (total_diff > 0) ? static_cast<float>(active_diff) / total_diff : 0.0;

  utilization_cache_.push_back(current);
  if (utilization_cache_.size() > kCacheSize) {
    utilization_cache_.pop_front();
  }

  float avg = 0.0;
  for (float val : utilization_cache_) {
    avg += val;
  }
  return avg / utilization_cache_.size();
}
