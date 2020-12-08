// PHZ
// 2018-5-15
// Scott Xu
// 2020-12-2 Add IPv6 Support. 

#ifndef XOP_SOCKET_UTIL_H
#define XOP_SOCKET_UTIL_H

#include "Socket.h"
#include <string>

namespace xop
{
    
class SocketUtil
{
public:
	static bool Bind(SOCKET sockfd, std::string ip, uint16_t port, bool ipv6 = false);
	static void SetNonBlock(SOCKET fd);
	static void SetBlock(SOCKET fd, int write_timeout=0);
	static void SetReuseAddr(SOCKET fd);
	static void SetReusePort(SOCKET sockfd);
	static void SetNoDelay(SOCKET sockfd);
	static void SetKeepAlive(SOCKET sockfd);
	static void SetNoSigpipe(SOCKET sockfd);
	static void SetSendBufSize(SOCKET sockfd, int size);
	static void SetRecvBufSize(SOCKET sockfd, int size);
	static std::string GetPeerIp(SOCKET sockfd, bool ipv6 = false);
	static std::string GetSocketIp(SOCKET sockfd, bool ipv6 = false);
	static uint16_t GetPeerPort(SOCKET sockfd, bool ipv6 = false);
	static int GetPeerAddr(SOCKET sockfd, struct sockaddr_in *addr);
	static int GetPeerAddr6(SOCKET sockfd, struct sockaddr_in6 *addr);
	static int GetSocketAddr(SOCKET sockfd, struct sockaddr_in *addr);
	static int GetSocketAddr6(SOCKET sockfd, struct sockaddr_in6 *addr);
	static void Close(SOCKET sockfd);
	static bool Connect(SOCKET sockfd, std::string ip, uint16_t port,
			    int timeout = 0, bool ipv6 = false);
	static bool IsIpv6Address(std::string ip);
	static bool SocketUtil::IsIpv6Socket(SOCKET sockfd);
};

}

#endif // _SOCKET_UTIL_H




