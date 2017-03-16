#pragma once

#include <cpptoml/cpptoml.h>

namespace steamworks {

class Log {
 public:
  static void Initialize(const std::shared_ptr<cpptoml::table> config);
};
} /* steamworks */
