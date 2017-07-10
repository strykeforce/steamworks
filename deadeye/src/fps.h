#pragma once

#include <chrono>

namespace deadeye {

class FPS {
 public:
  FPS();
  void Start();
  void Stop();
  void Update();
  int Elapsed();
  double FramesPerSecond();

 private:
  std::chrono::high_resolution_clock::time_point start_, end_;
  int num_frames_ = 0;
};
} /* deadeye */
