#ifndef DEADEYE_MESSAGE_INC_
#define DEADEYE_MESSAGE_INC_

#include <netdb.h>
#include <string>

typedef int UDPSocket;
typedef struct sockaddr_in SocketAddress;

namespace deadeye {
class Message {
private:
  std::string host_;
  std::string port_;
  struct addrinfo *addr_ = nullptr;
  int fd_;

public:
  Message(const std::string &host, const std::string &port);
  virtual ~Message();

  void send(const float payload[3]);
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_MESSAGE_INC_ */
