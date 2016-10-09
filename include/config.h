#ifndef DEADEYE_CONFIG_INC_
#define DEADEYE_CONFIG_INC_

#include "cpptoml.h"

namespace deadeye {
class Config {
 private:
  std::shared_ptr<cpptoml::table> config_;
  void ConfigureLogger();
  void ConfigureMessage();

 public:
  const char* host;
  const char* port;
  Config(int argc, char** argv);
  virtual ~Config();
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_CONFIG_INC_ */
