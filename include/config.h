#ifndef DEADEYE_CONFIG_INC_
#define DEADEYE_CONFIG_INC_

#include <opencv2/opencv.hpp>
#include "cpptoml.h"

namespace deadeye {
class Config {
 private:
  std::shared_ptr<cpptoml::table> config_;
  void ConfigureLogger();
  void ConfigureMessage();
  void ConfigureColor();

 public:
  const char* host;
  const char* port;
  cv::Scalar range_lower;
  cv::Scalar range_upper;
  Config(int argc, char** argv);
  virtual ~Config();
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_CONFIG_INC_ */
