#include "message.h"

#include "spdlog/spdlog.h"

#include "constants.h"
#include "udp_handler.h"

namespace spd = spdlog;

namespace deadeye {
Message::Message() {
  auto console = spd::get("console");
  sock = CreateUDPDataStreamNOBIND();
  console->debug("sock = {}", sock);
  console->debug("UDP port = {}", kNetworkVisionCommPort);
  while (sock_addr == nullptr) {
    // sock_addr = CreateSocketAddress("10.27.67.206", kNetworkVisionCommPort);
    sock_addr = CreateSocketAddress("127.0.0.1", kNetworkVisionCommPort);
    usleep(1 * 1000000);
  }
}

Message::~Message() {
  // code
}
} /* deadeye */
