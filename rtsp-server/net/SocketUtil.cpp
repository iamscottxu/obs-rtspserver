// PHZ
// 2018-5-15
// Scott Xu
// 2020-12-2 Add IPv6 Support.

#include "SocketUtil.h"
#include "Socket.h"
#include <iostream>

using namespace xop;

bool SocketUtil::Bind(const SOCKET sockfd, const std::string &ip, const uint16_t port, const bool ipv6)
{
	sockaddr *psockaddr;
	socklen_t addrlen;
	if (ipv6) {
		sockaddr_in6 addr = {0};
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(port);
		inet_pton(AF_INET6, ip.c_str(), &addr.sin6_addr);
		psockaddr = reinterpret_cast<sockaddr *>(&addr);
		addrlen = sizeof(addr);
	} else {
		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
		psockaddr = reinterpret_cast<sockaddr *>(&addr);
		addrlen = sizeof(addr);
	}

	if (bind(sockfd, psockaddr, addrlen) == SOCKET_ERROR) {
		return false;
	}

	return true;
}

void SocketUtil::SetNonBlock(const SOCKET fd)
{
#if defined(WIN32) || defined(_WIN32)
        unsigned long on = 1;
	ioctlsocket(fd, FIONBIO, &on);
#else
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}

void SocketUtil::SetBlock(const SOCKET fd, const int write_timeout)
{
#if defined(WIN32) || defined(_WIN32)
	unsigned long on = 0;
	ioctlsocket(fd, FIONBIO, &on);
#else
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));
#endif
	if (write_timeout > 0) {
#ifdef SO_SNDTIMEO
#if defined(WIN32) || defined(_WIN32)
		auto ms = static_cast<unsigned long>(write_timeout);
		setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&ms),
			   sizeof(unsigned long));
#else
                struct timeval tv = {write_timeout / 1000,
                                     (write_timeout % 1000) * 1000};
                setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof tv);
#endif
#endif
	}
}

void SocketUtil::SetReuseAddr(const SOCKET sockfd)
{
	constexpr int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&on),
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
	int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&on),
			     sizeof on);
#endif
}

void SocketUtil::SetKeepAlive(SOCKET sockfd)
{
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&on), sizeof on);
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
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&size), sizeof size);
}

void SocketUtil::SetRecvBufSize(SOCKET sockfd, int size)
{
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&size), sizeof size);
}

std::string SocketUtil::GetPeerIp(const SOCKET sockfd, const bool ipv6)
{
	if (ipv6) {
		sockaddr_in6 addr = {0};
		char str[INET6_ADDRSTRLEN] = "::0";
		if (GetPeerAddr6(sockfd, &addr) == 0)
			inet_ntop(AF_INET6, &addr.sin6_addr, str, sizeof str);
		return str;
	}
	sockaddr_in addr = {0};
	char str[INET_ADDRSTRLEN] = "0.0.0.0";
	if (GetPeerAddr(sockfd, &addr) == 0)
		inet_ntop(AF_INET, &addr.sin_addr, str, sizeof str);
	return str;
}

std::string SocketUtil::GetSocketIp(SOCKET sockfd, const bool ipv6)
{
	if (ipv6) {
		sockaddr_in6 addr = {0};
		char str[INET6_ADDRSTRLEN] = "::1";
		if (GetSocketAddr6(sockfd, &addr) == 0)
			inet_ntop(AF_INET6, &addr.sin6_addr, str, sizeof str);
		return str;
	}
	sockaddr_in addr = {0};
	char str[INET_ADDRSTRLEN] = "127.0.0.1";
	if (GetSocketAddr(sockfd, &addr) == 0)
		inet_ntop(AF_INET, &addr.sin_addr, str, sizeof str);
	return str;
}

uint16_t SocketUtil::GetPeerPort(SOCKET sockfd, const bool ipv6)
{
	if (ipv6) {
		sockaddr_in6 addr = {0};
		if (GetPeerAddr6(sockfd, &addr) == 0)
			return ntohs(addr.sin6_port);
	}
	sockaddr_in addr = {0};
	if (GetPeerAddr(sockfd, &addr) == 0)
		return ntohs(addr.sin_port);

	return 0;
}

int SocketUtil::GetPeerAddr(SOCKET sockfd, sockaddr_in *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	return getpeername(sockfd, reinterpret_cast<sockaddr *>(addr), &addrlen);
}

int SocketUtil::GetPeerAddr6(const SOCKET sockfd, sockaddr_in6 *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in6);
	return getpeername(sockfd, reinterpret_cast<sockaddr *>(addr), &addrlen);
}

int SocketUtil::GetSocketAddr(SOCKET sockfd, sockaddr_in *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in);
	return getsockname(sockfd, reinterpret_cast<sockaddr *>(addr), &addrlen);
}

int SocketUtil::GetSocketAddr6(SOCKET sockfd, sockaddr_in6 *addr)
{
	socklen_t addrlen = sizeof(struct sockaddr_in6);
	return getsockname(sockfd, reinterpret_cast<sockaddr *>(addr), &addrlen);
}

void SocketUtil::Close(SOCKET sockfd)
{
#if defined(WIN32) || defined(_WIN32)
	::closesocket(sockfd);
#else
        ::close(sockfd);
#endif
}

bool SocketUtil::Connect(SOCKET sockfd, const std::string &ip, const uint16_t port,
                         const int timeout, const bool ipv6)
{
	bool is_connected = true;
	if (timeout > 0) {
		SetNonBlock(sockfd);
	}

	sockaddr *psockaddr;
	socklen_t addrlen;
	if (ipv6) {
		sockaddr_in6 addr = {0};
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(port);
		inet_pton(AF_INET6, ip.c_str(), &addr.sin6_addr);
		psockaddr = reinterpret_cast<sockaddr *>(&addr);
		addrlen = sizeof(addr);
	} else {
		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
		psockaddr = reinterpret_cast<sockaddr *>(&addr);
		addrlen = sizeof(addr);
	}

	if (connect(sockfd, psockaddr, addrlen) == SOCKET_ERROR) {
		if (timeout > 0) {
			is_connected = false;
			fd_set fd_write{};
			FD_ZERO(&fd_write);
			FD_SET(sockfd, &fd_write);
			const timeval tv = {timeout / 1000,
			                    timeout % 1000 * 1000};
			select(static_cast<int>(sockfd) + 1, nullptr, &fd_write, nullptr, &tv);
			if (FD_ISSET(sockfd, &fd_write)) {
				is_connected = true;
			}
			SetBlock(sockfd);
		} else {
			is_connected = false;
		}
	}

	return is_connected;
}

bool SocketUtil::IsIpv6Address(std::string ip)
{
	in6_addr addr6{};
	return inet_pton(AF_INET6, ip.c_str(), &addr6) > 0;
}

bool SocketUtil::IsIpv6Socket(SOCKET sockfd)
{
	sockaddr_in6 addr = {0};
	socklen_t addrlen = sizeof addr;
        getsockname(sockfd, reinterpret_cast<sockaddr *>(&addr), &addrlen);
        if (addr.sin6_family == AF_INET6)
		return true;
	return false;
}
