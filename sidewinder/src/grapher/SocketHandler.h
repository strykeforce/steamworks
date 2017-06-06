#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "CodeUtils.h"
#include "NetworkPorts.h"

typedef int Socket;
typedef struct sockaddr_in SocketAddress;

class SocketHandler {
 public:
  static Socket CreateUDPSocket(unsigned short portno);

  static Socket CreateUDPSocketNOPORT();

  static Socket CreateTCPSocketCLIENT();

  static Socket CreateTCPSocketSERVER(unsigned short portno);

  static int FilloutSocketAddress(SocketAddress* serveraddr, char* hostname,
                                  short portno);

  static int TCPConnectToServer(Socket sockfd, SocketAddress* where);

  static Socket TCPWaitForClient(Socket sockfd,
                                 SocketAddress* client_address = NULL);

  static int TCPSend(Socket sockfd, void* data, int length);

  static int UDPSend(Socket sockfd, void* data, int length,
                     SocketAddress* where);

  static int Recv(Socket sockfd, void* where, int length);

  static int SetTimeout(Socket sockfd, int microseconds, int seconds = 0);

  static int RecvFromWho(Socket sockfd, void* where, int length,
                         SocketAddress* who);

  static int PeekRecv(Socket sockfd, void* where, int length);

  static void SendPingsToWhere(Socket sockfd, SocketAddress* where);

  static void WaitForPingsFromWho(Socket sockfd, SocketAddress* sa);

  static void CloseSocket(Socket s);
};
#endif
