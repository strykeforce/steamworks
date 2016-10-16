#include "robot.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "spdlog/spdlog.h"

#include "config.h"

namespace spd = spdlog;

// http://www.microhowto.info/howto/send_a_udp_datagram_in_c.html

namespace deadeye {
Robot::Robot(std::shared_ptr<deadeye::Config> config) {
  auto console = spd::get("console");
  auto table = config->GetTable("robot");
  if (!table) {
    console->info("Robot not configured.");
    return;
  }
  auto host = (table->get_as<std::string>("address"))->c_str();
  auto port = (table->get_as<std::string>("port"))->c_str();
  console->info("Robot is at {}:{}", host, port);

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_ADDRCONFIG;
  auto err = getaddrinfo(host, port, &hints, &addr_);
  if (err != 0) {
    console->critical("failed to resolve remote socket address (err={})", err);
  }
  fd_ = socket(addr_->ai_family, addr_->ai_socktype, addr_->ai_protocol);
  if (fd_ == -1) {
    console->critical(strerror(errno));
  }
  enabled_ = true;
}

Robot::~Robot() {
  if (!enabled_) {
    return;
  }
  if (close(fd_) == -1) {
    spd::get("console")->error(strerror(errno));
  }
  freeaddrinfo(addr_);
}

void Robot::TargetAt(float const center, float const range) {
  if (!enabled_) {
    return;
  }
  float payload[3] = {center, range, 0.0};
  if (sendto(fd_, payload, sizeof(float) * 3, 0, addr_->ai_addr,
             addr_->ai_addrlen) == -1) {
    spd::get("console")->critical(strerror(errno));
  }
}

void Robot::NoTarget() {
  if (!enabled_) {
    return;
  }
  float payload[3] = {0.0, 0.0, 42.0};
  if (sendto(fd_, payload, sizeof(float) * 3, 0, addr_->ai_addr,
             addr_->ai_addrlen) == -1) {
    spd::get("console")->critical(strerror(errno));
  }
}
} /* deadeye */
