#include "robot.h"

#include <memory>
#include <string>
#include <cstdint>

#include "cpptoml/cpptoml.h"
#include "serial/serial.h"

using namespace deadeye;
using namespace std;

Robot::Robot(std::shared_ptr<cpptoml::table> config) {
  serial_.reset(new serial::Serial("/dev/ttyS0", 115200,
                                   serial::Timeout::simpleTimeout(1000)));
}

void Robot::SendBoilerTarget(int azimuth_error, int range) {
  string message = to_string(azimuth_error) + "," + to_string(range) + "\n";
  serial_->write(message);
}

void Robot::SendBoilerNoTarget() {
  serial_->write("0,-1\n");
}
