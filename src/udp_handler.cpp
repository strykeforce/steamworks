#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include "spdlog/spdlog.h"

#include "constants.h"
#include "udp_handler.h"

namespace spd = spdlog;

UDPSocket CreateUDPDataStream(int portno) {
  auto console = spd::get("console");
  console->info("UDPHandler::CreateUDPDataStream({})", portno);
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in serveraddr;
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
             sizeof(int));
  bzero((char*)&(serveraddr), sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);
  if (bind(sockfd, (struct sockaddr*)&(serveraddr), sizeof(serveraddr)) < 0)
    console->error("UDPHandler::ERROR on binding {}", portno);
  return sockfd;
}
UDPSocket CreateUDPDataStreamNOBIND() {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
             sizeof(int));
  return sockfd;
}
SocketAddress* CreateSocketAddress(const char* hostname, short portno) {
  auto console = spd::get("console");

  console->info("UDPHandler::CreateSocketAddress({}, {})", hostname, portno);
  struct hostent* server = gethostbyname(hostname);
  if (server == NULL) {
    console->error("gethostbyname({}) failed.", hostname);
    console->error("ERROR: ");
    switch (h_errno) {
      case HOST_NOT_FOUND: {
        console->error("HOST_NOT_FOUND (The specified host is unknown.)");
        break;
      }
      case NO_ADDRESS: {
        console->error(
            "NO_ADDRESS or NO_DATA (The requested name is valid but "
            "does not have an IP address.)");
        break;
      }
      case NO_RECOVERY: {
        console->error(
            "NO_RECOVERY (A nonrecoverable name server error occurred.)");
        break;
      }
      case TRY_AGAIN: {
        console->error(
            "TRY_AGAIN (A temporary error occurred on an uthoritative "
            "name server.  Try again later.)");
        break;
      }
    }
    return NULL;
  }
  SocketAddress* serveraddr = (SocketAddress*)malloc(sizeof(SocketAddress));
  bzero((char*)serveraddr, sizeof(sockaddr_in));
  serveraddr->sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&(serveraddr->sin_addr.s_addr),
        server->h_length);
  serveraddr->sin_port = htons(portno);
  return serveraddr;
}
int UDPSend(UDPSocket sockfd, void* data, int length, SocketAddress* where) {
  return sendto(sockfd, data, length, 0, (const sockaddr*)(where),
                sizeof(sockaddr));
}
int UDPRecv(UDPSocket sockfd, void* where, int length) {
  int size = recv(sockfd, where, length, 0);
  return size;
}
int SetTimeout(UDPSocket sockfd, int microseconds) {
  struct timeval time;
  time.tv_sec = 0;
  time.tv_usec = microseconds;
  return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
}

int UDPRecvFromWho(UDPSocket sockfd,
                   void* where,
                   int length,
                   SocketAddress* who) {
  auto console = spd::get("console");

  console->debug("UDPHandler::UDPRecv({}, {}, {})", sockfd, where, length);
  unsigned int wholength = sizeof(sockaddr_in);
  return recvfrom(sockfd, where, length, 0, (sockaddr*)who, &wholength);
}
int UDPPeekRecv(UDPSocket sockfd, void* where, int length) {
  return recv(sockfd, where, length, MSG_PEEK);
}

#define UDPPingSpeed 1000000 /* Microseconds */
void SendPingsToWhere(UDPSocket sockfd, SocketAddress* where) {
  auto console = spd::get("console");

  char c = 42;
  SetTimeout(sockfd, 1);
  while (true) {
    UDPSend(sockfd, &c, sizeof(c), where);
    int status = UDPRecv(sockfd, &c, sizeof(c));
    console->debug("status == {}", status);
    if (status > 0)
      return;
    // usleep(UDPPingSpeed);
  }
}

SocketAddress* WaitForPingsFromWho(UDPSocket sockfd) {
  auto console = spd::get("console");

  console->debug("UDPHandler::WaitForPingsFromWho({})", sockfd);
  SetTimeout(sockfd, 0);
  char c;
  SocketAddress* sa = new SocketAddress();
  int status = -1;
  while (status < 0) {
    status = UDPRecvFromWho(sockfd, &c, sizeof(c), sa);
    console->debug("status == {}", status);
  }
  UDPSend(sockfd, &c, sizeof(c), sa);
  return sa;
}
