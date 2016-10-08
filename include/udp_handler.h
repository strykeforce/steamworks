#ifndef DEADEYE_UDP_HANDLER
#define DEADEYE_UDP_HANDLER

typedef int UDPSocket;
typedef struct sockaddr_in SocketAddress;

UDPSocket CreateUDPDataStream(int portno);

UDPSocket CreateUDPDataStreamNOBIND();

SocketAddress* CreateSocketAddress(const char* hostname, short portno);

int UDPSend(UDPSocket sockfd, void* data, int length, SocketAddress* where);

int UDPRecv(UDPSocket sockfd, void* where, int length);

int SetTimeout(UDPSocket sockfd, int microseconds);

int UDPRecvFromWho(UDPSocket sockfd,
                   void* where,
                   int length,
                   SocketAddress* who);

int UDPPeekRecv(UDPSocket sockfd, void* where, int length);

void SendPingsToWhere(UDPSocket sockfd, SocketAddress* where);

SocketAddress* WaitForPingsFromWho(UDPSocket sockfd);
#endif /* end of include guard: DEADEYE_UDP_HANDLER */
