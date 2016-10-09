#ifndef DEADEYE_MESSAGE_INC_
#define DEADEYE_MESSAGE_INC_

#include <netdb.h>
#include <string>

#include "config.h"

typedef int UDPSocket;
typedef struct sockaddr_in SocketAddress;

namespace deadeye {
class Robot {
 private:
  std::string host_;
  std::string port_;
  struct addrinfo* addr_ = nullptr;
  int fd_;

 public:
  Robot(std::shared_ptr<deadeye::Config> config);
  virtual ~Robot();

  void Send(const float payload[3]);
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_MESSAGE_INC_ */
