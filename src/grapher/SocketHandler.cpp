#include "SocketHandler.h"

Socket SocketHandler::CreateUDPSocket(unsigned short portno) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in serveraddr;
  bzero((char*)&(serveraddr), sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr*)&(serveraddr), sizeof(serveraddr)) < 0) {
    printf("SocketHandler::CreateUDPSocket:\tERROR on binding to port %i \n",
           portno);
  }
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
             sizeof(int));
  return sockfd;
}

Socket SocketHandler::CreateUDPSocketNOPORT() {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
             sizeof(int));
  return sockfd;
}

Socket SocketHandler::CreateTCPSocketCLIENT() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
             sizeof(int));
  return sockfd;
}

Socket SocketHandler::CreateTCPSocketSERVER(unsigned short portno) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server;
  bzero((char*)&(server), sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr*)&(server), sizeof(server)) < 0) {
    printf(
        "SocketHandler::CreateTCPSocketSERVER:\tERROR on binding to port %i\n",
        portno);
  }
  listen(sockfd, 5);
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
             sizeof(int));
  return sockfd;
}

int SocketHandler::FilloutSocketAddress(SocketAddress* serveraddr,
                                        char* hostname, short portno) {
  struct hostent* server = gethostbyname(hostname);
  if (server == NULL) {
    printf(
        "SocketHandler::FilloutSocketAddress: Couldn't get host address of "
        "\"%s\"\n",
        hostname);
    switch (h_errno) {
      case HOST_NOT_FOUND: {
        printf("\tHOST_NOT_FOUND (The specified host is unknown.)\n");
        break;
      }
      case NO_ADDRESS: {
        printf(
            "\tNO_ADDRESS or NO_DATA (The requested name is valid but does not "
            "have an IP address.)\n");
        break;
      }
      case NO_RECOVERY: {
        printf(
            "\tNO_RECOVERY (A nonrecoverable name server error occurred.)\n");
        break;
      }
      case TRY_AGAIN: {
        printf(
            "\tTRY_AGAIN (A temporary error occurred on an authoritative name "
            "server.  Try again later.)\n");
        break;
      }
    }
    return h_errno;
  }
  bzero((char*)serveraddr, sizeof(sockaddr_in));
  serveraddr->sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&(serveraddr->sin_addr.s_addr),
        server->h_length);
  serveraddr->sin_port = htons(portno);
  return 0;
}

int SocketHandler::TCPConnectToServer(Socket sockfd, SocketAddress* where) {
  int returned = connect(sockfd, (const sockaddr*)where, sizeof(SocketAddress));
  if (returned == -1) {
    printf("SocketHandler::TCPConnectToServer:\t%s\n", strerror(errno));
  }
  return returned;
}

Socket SocketHandler::TCPWaitForClient(Socket sockfd, SocketAddress* where) {
  int sizeofstruct = sizeof(SocketAddress);
  int returned = accept(sockfd, (sockaddr*)where, (socklen_t*)&sizeofstruct);
  return returned;
}

int SocketHandler::TCPSend(Socket sockfd, void* data, int length) {
  return send(sockfd, data, length, 0);
}

int SocketHandler::UDPSend(Socket sockfd, void* data, int length,
                           SocketAddress* where) {
  int returned = sendto(sockfd, data, length, 0, (const sockaddr*)(where),
                        sizeof(sockaddr));
  return returned;
}

int SocketHandler::Recv(Socket sockfd, void* where, int length) {
  int size = recv(sockfd, where, length, 0);
  return size;
}

int SocketHandler::SetTimeout(Socket sockfd, int microseconds, int seconds) {
  struct timeval time;
  time.tv_sec = seconds;
  time.tv_usec = microseconds;
  return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
}

int SocketHandler::RecvFromWho(Socket sockfd, void* where, int length,
                               SocketAddress* who) {
  unsigned int wholength = sizeof(sockaddr_in);
  return recvfrom(sockfd, where, length, 0, (sockaddr*)who, &wholength);
}

int SocketHandler::PeekRecv(Socket sockfd, void* where, int length) {
  return recv(sockfd, where, length, MSG_PEEK);
}

#define PingSpeed 100 * 1000 /* Microseconds */
void SocketHandler::SendPingsToWhere(Socket sockfd, SocketAddress* where) {
  char c = 42;
  SetTimeout(sockfd, 1);
  while (true) {
    UDPSend(sockfd, &c, sizeof(c), where);
    int status = Recv(sockfd, &c, sizeof(c));
    if (status > 0) return;
    // usleep(UDPPingSpeed);
  }
}

void SocketHandler::WaitForPingsFromWho(Socket sockfd, SocketAddress* sa) {
  SetTimeout(sockfd, 0);
  char c;
  int status = -1;
  while (status < 0) {
    status = RecvFromWho(sockfd, &c, sizeof(c), sa);
  }
  UDPSend(sockfd, &c, sizeof(c), sa);
}

void SocketHandler::CloseSocket(Socket s) {
  shutdown(s, SHUT_RDWR);
  close(s);
}
