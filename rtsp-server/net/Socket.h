// PHZ
// 2018-5-15
// Scott Xu
// 2020-12-2 Add IPv6 Support.

#ifndef XOP_SOCKET_H
#define XOP_SOCKET_H

#if defined(WIN32) || defined(_WIN32)
#define FD_SETSIZE 1024
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/route.h>
#include <netdb.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#if defined(__linux) || defined(__linux__)
#include <netinet/ether.h>
#include <netpacket/packet.h>
#endif
#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#define INET_ADDRSTRLEN 16  /* for IPv4 dotted-decimal */
#define INET6_ADDRSTRLEN 46 /* for IPv6 hex string */
#endif

#include <cstdint>
#include <cstring>

#endif // _XOP_SOCKET_H
