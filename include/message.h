#ifndef DEADEYE_MESSAGE_INC_
#define DEADEYE_MESSAGE_INC_

typedef int UDPSocket;
typedef struct sockaddr_in SocketAddress;

namespace deadeye {
class Message {
 public:
  // private:
  SocketAddress* sock_addr;
  UDPSocket sock;

 public:
  Message();
  virtual ~Message();

  // void send(const float[] & packet);
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_MESSAGE_INC_ */
