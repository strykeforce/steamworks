#include "fps.h"

using namespace deadeye;
using namespace std::chrono;

FPS::FPS() {}

void FPS::Start() {
  start_ = std::chrono::high_resolution_clock::now();
  num_frames_ = 0;
}

void FPS::Stop() {
  end_ = std::chrono::high_resolution_clock::now();
}

void FPS::Update() {
  num_frames_++;
}

int FPS::Elapsed() {
  return duration_cast<std::chrono::milliseconds>(end_ - start_).count();
}

double FPS::FramesPerSecond() {
  return 1000.0 * static_cast<double>(num_frames_) / Elapsed();
}
