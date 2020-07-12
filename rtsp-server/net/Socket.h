// PHZ
// 2018-5-15

#ifndef XOP_SOCKET_H
#define XOP_SOCKET_H

#if defined(WIN32) || defined(_WIN32) 
#define FD_SETSIZE      1024
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#define SHUT_RD 0
#define SHUT_WR 1 
#define SHUT_RDWR 2

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#if defined(__APPLE) || defined(__APPLE__)
#include <netinet/if_ether.h>
#else
#include <netinet/ether.h>
#endif
#include <netinet/ip.h>
/*#if defined(__APPLE) || defined(__APPLE__)

#else
#include <netpacket/packet.h>
#endif*/
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/route.h>
#include <netdb.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#endif

#include <cstdint>
#include <cstring>

#endif // _XOP_SOCKET_H
