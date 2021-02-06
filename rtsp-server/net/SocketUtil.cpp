// PHZ
// 2018-5-15
// Scott Xu
// 2020-12-2 Add IPv6 Support.

#include "SocketUtil.h"
#include "Socket.h"
#include <iostream>

using namespace xop;

bool SocketUtil::Bind(SOCKET sockfd, std::string ip, uint16_t port, bool ipv6)
{
	struct sockaddr *psockaddr;
	socklen_t addrlen = 0;
	if (ipv6) {
		struct sockaddr_in6 addr = {0};
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(port);
		inet_pton(AF_INET6, ip.c_str(), &addr.sin6_addr);
		psockaddr = (struct sockaddr *)&addr;
		addrlen = sizeof(addr);
	} else {
		struct sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
		psockaddr = (struct sockaddr *)&addr;
		addrlen = sizeof(addr);
	}

	if (::bind(sockfd, psockaddr, addrlen) == SOCKET_ERROR) {
		return false;
	}

	return true;
}

void SocketUtil::SetNonBlock(SOCKET fd)
{
#if defined(__linux) || defined(__linux__)
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	unsigned long on = 1;
	ioctlsocket(fd, FIONBIO, &on);
#endif
}

void SocketUtil::SetBlock(SOCKET fd, int writeTimeout)
{
#if defined(__linux) || defined(__linux__)
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));
#elif defined(WIN32) || defined(_WIN32)
	unsigned long on = 0;
	ioctlsocket(fd, FIONBIO, &on);
#else
#endif
	if (writeTimeout > 0) {
#ifdef SO_SNDTIMEO
#if defined(__linux) || defined(__linux__)
		struct timeval tv = {writeTimeout / 1000,
				     (writeTimeout % 1000) * 1000};
		setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof tv);
#elif defined(WIN32) || defined(_WIN32)
		unsigned long ms = (unsigned long)writeTimeout;
		setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&ms,
			   sizeof(unsigned long));
#else
#endif
#endif
	}
}

void SocketUtil::SetReuseAddr(SOCKET sockfd)
{
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on,
		   sizeof on);
}

void SocketUtil::SetReusePort(SOCKET sockfd)
{
#ifdef SO_REUSEPORT
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char *)&on,
		   sizeof(on));
#endif
}

void SocketUtil::SetNoDelay(SOCKET sockfd)
{
#ifdef TCP_NODELAY
	int on = 1;
	int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&on,
			     sizeof(on));
#endif
}

void SocketUtil::SetKeepAlive(SOCKET sockfd)
{
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on));
}

void SocketUtil::SetNoSigpipe(SOCKET sockfd)
{
#ifdef SO_NOSIGPIPE
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (char *)&on, sizeof(on));
#endif
}

void SocketUtil::SetSendBufSize(SOCKET sockfd, int size)
{
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));
}

void SocketUtil::SetRecvBufSize(SOCKET sockfd, int size)
{
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));
}

std::string SocketUtil::GetPeerIp(SOCKET sockfd, bool ipv6)
{
	if (ipv6) {
		struct sockaddr_in6 addr = {0};
		char str[INET6_ADDRSTRLEN] = "::0";
		if (GetPeerAddr6(sockfd, &addr) == 0)
			inet_ntop(AF_INET6, &addr.sin6_addr, str, sizeof(str));
		return str;
	}
	struct sockaddr_in addr = {0};
	char str[INET_ADDRSTRLEN] = "0.0.0.0";
	if (GetPeerAddr(sockfd, &addr) == 0)
		inet_ntop(AF_INET, &addr.sin_addr, str, sizeof(str));
	return str;
}

std::string SocketUtil::GetSocketIp(SOCKET sockfd, bool ipv6)
{
	if (ipv6) {
		struct sockaddr_in6 addr = {0};
		char str[INET6_ADDRSTRLEN] = "::1";
		if (GetSocketAddr6(sockfd, &addr) == 0)
			inet_ntop(AF_INET6, &addr.sin6_addr, str, sizeof(str));
		return str;
	}
	struct sockaddr_in addr = {0};
	char str[INET_ADDRSTRLEN] = "127.0.0.1";
	if (GetSocketAddr(sockfd, &addr) == 0)
		inet_ntop(AF_INET, &addr.sin_addr, str, sizeof(str));
	return str;
}

uint16_t SocketUtil::GetPeerPort(SOCKET sockfd, bool ipv6)
{
	if (ipv6) {
		struct sockaddr_in6 addr = {0};
		if (GetPeerAddr6(sockfd, &addr) == 0)
			return ntohs(addr.sin6_port);
	}
	struct sockaddr_in addr = {0};
	if (GetPeerAddr(sockfd, &addr) == 0)
		return ntohs(addr.sin_port);

	return 0;
}

int SocketUtil::GetPeerAddr(SOCKET sockfd, struct sockaddr_in *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	return getpeername(sockfd, (struct sockaddr *)addr, &addrlen);
}

int SocketUtil::GetPeerAddr6(SOCKET sockfd, struct sockaddr_in6 *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in6);
	return getpeername(sockfd, (struct sockaddr *)addr, &addrlen);
}

int SocketUtil::GetSocketAddr(SOCKET sockfd, struct sockaddr_in *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	return getsockname(sockfd, (struct sockaddr *)addr, &addrlen);
}

int SocketUtil::GetSocketAddr6(SOCKET sockfd, struct sockaddr_in6 *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in6);
	return getsockname(sockfd, (struct sockaddr *)addr, &addrlen);
}

void SocketUtil::Close(SOCKET sockfd)
{
#if defined(__linux) || defined(__linux__)
	::close(sockfd);
#elif defined(WIN32) || defined(_WIN32)
	::closesocket(sockfd);
#endif
}

bool SocketUtil::Connect(SOCKET sockfd, std::string ip, uint16_t port,
			 int timeout, bool ipv6)
{
	bool isConnected = true;
	if (timeout > 0) {
		SocketUtil::SetNonBlock(sockfd);
	}

	struct sockaddr *psockaddr;
	socklen_t addrlen = 0;
	if (ipv6) {
		struct sockaddr_in6 addr = {0};
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(port);
		inet_pton(AF_INET6, ip.c_str(), &addr.sin6_addr);
		psockaddr = (struct sockaddr *)&addr;
		addrlen = sizeof(addr);
	} else {
		struct sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
		psockaddr = (struct sockaddr *)&addr;
		addrlen = sizeof(addr);
	}

	if (::connect(sockfd, psockaddr, addrlen) == SOCKET_ERROR) {
		if (timeout > 0) {
			isConnected = false;
			fd_set fdWrite;
			FD_ZERO(&fdWrite);
			FD_SET(sockfd, &fdWrite);
			struct timeval tv = {timeout / 1000,
					     timeout % 1000 * 1000};
			select((int)sockfd + 1, NULL, &fdWrite, NULL, &tv);
			if (FD_ISSET(sockfd, &fdWrite)) {
				isConnected = true;
			}
			SocketUtil::SetBlock(sockfd);
		} else {
			isConnected = false;
		}
	}

	return isConnected;
}

bool SocketUtil::IsIpv6Address(std::string ip)
{
	struct in6_addr addr6;
	return inet_pton(AF_INET6, ip.c_str(), &addr6) > 0;
}

bool SocketUtil::IsIpv6Socket(SOCKET sockfd)
{
	struct sockaddr_in6 addr = {0};
	socklen_t addrlen = sizeof(addr);
        getsockname(sockfd, (struct sockaddr*)&addr, &addrlen);
        if (addr.sin6_family == AF_INET6)
		return true;
	return false;
}
