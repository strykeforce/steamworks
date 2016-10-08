#include "message.h"

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

namespace spd = spdlog;

// http://www.microhowto.info/howto/send_a_udp_datagram_in_c.html

namespace deadeye {
Message::Message(const std::string& host, const std::string& port)
    : host_(host), port_(port) {
  auto console = spd::get("console");
  console->info("Sending targeting to {}:{}", host_, port_);
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_ADDRCONFIG;
  auto err = getaddrinfo(host_.c_str(), port_.c_str(), &hints, &addr_);
  if (err != 0) {
    console->critical("failed to resolve remote socket address (err={})", err);
  }
  fd_ = socket(addr_->ai_family, addr_->ai_socktype, addr_->ai_protocol);
  if (fd_ == -1) {
    console->critical(strerror(errno));
  }
}

Message::~Message() {
  freeaddrinfo(addr_);
}

void Message::send(const float payload[3]) {
  if (sendto(fd_, payload, sizeof(float) * 3, 0, addr_->ai_addr,
             addr_->ai_addrlen) == -1) {
    spd::get("console")->critical(strerror(errno));
  }
}
} /* deadeye */
