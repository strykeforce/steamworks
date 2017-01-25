#pragma once

#include <memory>
#include "FlyCapture2.h"
#include "cpptoml/cpptoml.h"

namespace deadeye {

class Deadeye {
 private:
  std::shared_ptr<FlyCapture2::Camera> camera_;
  bool has_gui_;

 public:
  Deadeye(std::shared_ptr<FlyCapture2::Camera> camera);
  virtual ~Deadeye() = default;

  void Display();
};

} /* deadeye */
