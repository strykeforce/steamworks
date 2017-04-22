#include "link.h"

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <array>
#include <msgpack.hpp>

using namespace deadeye;
using namespace std;

Link::Link(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("link")) {
  LoadConfigSettings(config);
  ConfigureNetworking();
}

/**
 * Start the IO thread, runs forever.
 */
void Link::Start() {
  thread_ = std::thread{&Link::Run, this};
  thread_.detach();
}

/**
 * The main IO loop.
 */
void Link::Run() {
  char buf[256];
  for (;;) {
    SPDLOG_DEBUG(logger_, "waiting for command from roborio");
    auto nread = recvfrom(recvfd_, buf, sizeof(buf), 0, NULL, NULL);
    if (nread == -1) {
      logger_->error("Deadeye recvfrom: {}", strerror(errno));
    }
    auto oh = msgpack::unpack(buf, nread);
    auto obj = oh.get();
    array<int, 3> message;
    obj.convert(message);
    SPDLOG_DEBUG(logger_, "Link recvfrom: [{},{},{}]", message[0], message[1],
                 message[2]);
    {
      std::lock_guard<std::mutex> lock(mutex_);
      current_mode_ = message[0];
    }
  }
}

/**
 * Set current mode.
 */
int Link::GetMode() {
  std::lock_guard<std::mutex> lock(mutex_);
  return current_mode_;
}

/**
 * Send the boiler shooting solution.
 */
void Link::SendBoilerSolution(int azimuth_error, int centerline_error) {
  array<int, 3> t{{kBoilerSolutionMesg, azimuth_error, centerline_error}};
  msgpack::sbuffer buf;
  msgpack::pack(buf, t);
  ssize_t nwrite = buf.size();
  if (send(sendfd_, buf.data(), nwrite, 0) != nwrite) {
    logger_->warn("Link error sending message: {}", strerror(errno));
  }
}

/**
 * Send the gear azimuth error.
 */
void Link::SendGearSolution(int azimuth_error, int target_height) {
  // SPDLOG_DEBUG(logger_, "gear solution: az error = {}, target_height = {}",
  //              azimuth_error, target_height);
  array<int, 3> t{{kGearSolutionMesg, azimuth_error, target_height}};
  msgpack::sbuffer buf;
  msgpack::pack(buf, t);
  ssize_t nwrite = buf.size();
  if (send(sendfd_, buf.data(), nwrite, 0) != nwrite) {
    logger_->warn("Link error sending message: {}", strerror(errno));
  }
}

/**
 * Send a no target message.
 */
void Link::SendNoTarget() {
  array<int, 3> t{{kNoTargetMesg, 0, 0}};
  msgpack::sbuffer buf;
  msgpack::pack(buf, t);
  ssize_t nwrite = buf.size();
  if (send(sendfd_, buf.data(), nwrite, 0) != nwrite) {
    logger_->warn("Link error sending message: {}", strerror(errno));
  }
}

/**
 * Configure UDP networking to roboRIO.
 */
void Link::ConfigureNetworking() {
  sendfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (sendfd_ == -1) {
    logger_->critical("Link send socket error: {}", strerror(errno));
  }

  // remote host with "address" and "port" in LINK config table
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (inet_pton(AF_INET, address_.c_str(), &addr.sin_addr) != 1) {
    logger_->critical("Link inet_pton error for address {}", address_);
  }
  addr.sin_port = htons(port_);

  // set up connection to send to remote
  if (connect(sendfd_, (sockaddr*)&addr, sizeof(addr)) == -1) {
    logger_->critical("Link connect error: {}", strerror(errno));
  }

  // configure listening port
  recvfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (recvfd_ == -1) {
    logger_->critical("Link recv socket error: {}", strerror(errno));
  }

  // listening address
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(5800);

  if (::bind(recvfd_, (sockaddr*)&addr, sizeof(addr)) == -1) {
    logger_->critical("Link bind error: {}", strerror(errno));
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

  s_opt = config->get_as<string>("mode");
  if (s_opt) {
    if (*s_opt == "boiler") {
      current_mode_ = Mode::boiler;
      logger_->info("LINK mode set to boiler");
    } else if (*s_opt == "gear") {
      current_mode_ = Mode::gear;
      logger_->info("LINK mode set to gear");
    } else {
      current_mode_ = Mode::idle;
      logger_->info("LINK mode set to idle");
    }
  } else {
    logger_->warn("LINK mode not set");
  }

  logger_->info("link to {}:{}", address_, port_);
}
