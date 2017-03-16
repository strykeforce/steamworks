#include "link.h"

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <array>
#include <msgpack.hpp>

using namespace deadeye;
using namespace std;

Link::Link(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")) {
  LoadConfigSettings(config);
  ConfigureNetworking();
}

/**
 * Set current mode.
 */
int Link::GetMode() {
#ifdef DEADEYE_TEST
  return Mode::idle;
#else
  return Mode::boiler;
#endif
}

/**
 * Send the boiler shooting solution.
 */
void Link::SendBoilerSolution(int azimuth_error, int centerline_error) {
  array<int, 3> t{{kBoilerSolutionMesg, azimuth_error, centerline_error}};
  msgpack::sbuffer buf;
  msgpack::pack(buf, t);
  ssize_t nwrite = buf.size();
  if (send(sockfd_, buf.data(), nwrite, 0) != nwrite) {
    logger_->warn("Link error sending message: {}", strerror(errno));
  }
  SPDLOG_DEBUG(logger_, "Link sent: {}",
               msgpack::unpack(buf.data(), buf.size()).get());
}

/**
 * Send the gear azimuth error.
 */
void Link::SendGearSolution(int azimuth_error, int range) {
  SPDLOG_DEBUG(logger_, "gear solution: az error = {}, range = {}",
               azimuth_error, range);
}

/**
 * Send a no target message.
 */
void Link::SendNoTarget() {
  array<int, 3> t{{kNoTargetMesg, 0, 0}};
  msgpack::sbuffer buf;
  msgpack::pack(buf, t);
  ssize_t nwrite = buf.size();
  if (send(sockfd_, buf.data(), nwrite, 0) != nwrite) {
    logger_->warn("Link error sending message: {}", strerror(errno));
  }
  SPDLOG_DEBUG(logger_, "Link sent: {}",
               msgpack::unpack(buf.data(), buf.size()).get());
}

/**
 * Configure UDP networking to roboRIO.
 */
void Link::ConfigureNetworking() {
  sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_ == -1) {
    logger_->critical("Link socket error: {}", strerror(errno));
  }

  // remote host with "address" and "port" in LINK config table
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (inet_pton(AF_INET, address_.c_str(), &addr.sin_addr) != 1) {
    logger_->critical("Link inet_pton error for address {}", address_);
  }
  addr.sin_port = htons(port_);

  // set up connection
  if (connect(sockfd_, (sockaddr*)&addr, sizeof(addr)) == -1) {
    logger_->critical("Link connect error: {}", strerror(errno));
  }
}

/**
 * Load configuration settings.
 */
void Link::LoadConfigSettings(const std::shared_ptr<cpptoml::table> config_in) {
  assert(config_in);
  auto config = config_in->get_table("LINK");
  if (!config) {
    throw invalid_argument("LINK table missing from config");
  }

  auto s_opt = config->get_as<string>("address");
  if (s_opt) {
    address_ = *s_opt;
  } else {
    logger_->warn("LINK address setting missing, using default");
  }

  auto i_opt = config->get_as<int>("port");
  if (i_opt) {
    port_ = *i_opt;
  } else {
    logger_->warn("LINK port setting missing, using default");
  }

  logger_->info("link to {}:{}", address_, port_);
}
