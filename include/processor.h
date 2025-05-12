#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <deque>
#include <cstddef>

class Processor {
 public:
  float Utilization();

 private:
  long prev_total_ = 0;
  long prev_active_ = 0;
  std::deque<float> utilization_cache_;
  static constexpr size_t kCacheSize = 5;
};

#endif  // PROCESSOR_H
