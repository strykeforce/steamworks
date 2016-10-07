

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include "NetworkPorts.cpp"

// Any Questions?
// http://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html
#ifndef UDP_Printout
#define UDP_Printout 1
#endif
typedef int UDPSocket;
typedef struct sockaddr_in SocketAddress;

class UDPHandler {
 public:
  static UDPSocket CreateUDPDataStream(int portno) {
#if UDP_Printout
    printf("UDPHandler::CreateUDPDataStream(%i)\n", portno);
#endif
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
      printf("UDPHandler::ERROR on binding %i", portno);
    return sockfd;
  }
  static UDPSocket CreateUDPDataStreamNOBIND() {
#if UDP_Printout
    printf("UDPHandler::CreateUDPDataStreamNOBIND()\n");
#endif
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
               sizeof(int));
    return sockfd;
  }
  static SocketAddress* CreateSocketAddress(const char* hostname,
                                            short portno) {
    //#if UDP_Printout
    printf("UDPHandler::CreateSocketAddress(%s, %i)\n", hostname, portno);
    //#endif
    struct hostent* server = gethostbyname(hostname);
    if (server == NULL) {
      printf("gethostbyname(%s) failed.\n", hostname);
      printf("ERROR: ");
      switch (h_errno) {
        case HOST_NOT_FOUND: {
          printf("HOST_NOT_FOUND (The specified host is unknown.)\n");
          break;
        }
        case NO_ADDRESS: {
          printf(
              "NO_ADDRESS or NO_DATA (The requested name is valid but does not "
              "have an IP address.)\n");
          break;
        }
        case NO_RECOVERY: {
          printf(
              "NO_RECOVERY (A nonrecoverable name server error occurred.)\n");
          break;
        }
        case TRY_AGAIN: {
          printf(
              "TRY_AGAIN (A temporary error occurred on an authoritative name "
              "server.  Try again later.)\n");
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
  static int UDPSend(UDPSocket sockfd,
                     void* data,
                     int length,
                     SocketAddress* where) {
#if UDP_Printout
    printf("UDPHandler::UDPSend(%i, %p, %i, %p)\n", sockfd, data, length,
           where);
#endif
    return sendto(sockfd, data, length, 0, (const sockaddr*)(where),
                  sizeof(sockaddr));
  }
  static int UDPRecv(UDPSocket sockfd, void* where, int length) {
#if UDP_Printout
    printf("UDPHandler::UDPRecv(%i, %p, %i)\n", sockfd, where, length);
#endif
    int size = recv(sockfd, where, length, 0);
#if UDP_Printout
    printf("UDPHandler::UDPRecv(%i, %p, %i) returned %i\n", sockfd, where,
           length, size);
#endif
    return size;
  }
  static int SetTimeout(UDPSocket sockfd, int microseconds) {
#if UDP_Printout
    printf("UDPHandler::SetTimeout(%i, %i)\n", sockfd, microseconds);
#endif
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = microseconds;
    return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
  }
  static int UDPRecvFromWho(UDPSocket sockfd,
                            void* where,
                            int length,
                            SocketAddress* who) {
#if UDP_Printout
    printf("UDPHandler::UDPRecv(%i, %p, %i)\n", sockfd, where, length);
#endif
    unsigned int wholength = sizeof(sockaddr_in);
    return recvfrom(sockfd, where, length, 0, (sockaddr*)who, &wholength);
  }
  static int UDPPeekRecv(UDPSocket sockfd, void* where, int length) {
#if UDP_Printout
    printf("UDPHandler::UDPPeekRecv(%i, %p, %i)\n", sockfd, where, length);
#endif
    return recv(sockfd, where, length, MSG_PEEK);
  }
#define UDPPingSpeed 1000000 /* Microseconds */
  static void SendPingsToWhere(UDPSocket sockfd, SocketAddress* where) {
#if UDP_Printout
    printf("UDPHandler::SendPingsToWhere(%i, %p)\n", sockfd, where);
#endif
    char c = 42;
    SetTimeout(sockfd, 1);
    while (true) {
      UDPSend(sockfd, &c, sizeof(c), where);
      int status = UDPRecv(sockfd, &c, sizeof(c));
#if UDP_Printout
      printf("status == %i\n", status);
#endif
      if (status > 0)
        return;
      // usleep(UDPPingSpeed);
    }
  }
  static SocketAddress* WaitForPingsFromWho(UDPSocket sockfd) {
#if UDP_Printout
    printf("UDPHandler::WaitForPingsFromWho(%i)\n", sockfd);
#endif
    SetTimeout(sockfd, 0);
    char c;
    SocketAddress* sa = new SocketAddress();
    int status = -1;
    while (status < 0) {
      status = UDPRecvFromWho(sockfd, &c, sizeof(c), sa);
#if UDP_Printout
      printf("status == %i\n", status);
      printf("sa == %p\n", sa);
#endif
    }
    UDPSend(sockfd, &c, sizeof(c), sa);
    return sa;
  }
};
