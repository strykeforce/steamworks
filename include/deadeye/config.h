#ifndef DEADEYE_CONFIG_INC_
#define DEADEYE_CONFIG_INC_

#include <opencv2/opencv.hpp>
#include "cpptoml/cpptoml.h"

namespace deadeye {
class Config {
 private:
  std::shared_ptr<cpptoml::table> config_;
  void ConfigureLogger();

 public:
  Config(std::string path);
  Config(int argc, char** argv);
  virtual ~Config();
  std::shared_ptr<cpptoml::table> GetTable(const std::string& table);
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_CONFIG_INC_ */
