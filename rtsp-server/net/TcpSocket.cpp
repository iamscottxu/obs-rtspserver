// PHZ
// 2018-5-15
// Scott Xu
// 2020-12-2 Add IPv6 Support. 

#include "TcpSocket.h"
#include "Socket.h"
#include "SocketUtil.h"
#include "Logger.h"

using namespace xop;

TcpSocket::TcpSocket(SOCKET sockfd, const bool ipv6)
	: sockfd_(sockfd), ipv6_(ipv6)
{
    
}

TcpSocket::~TcpSocket()
= default;

SOCKET TcpSocket::Create(const bool ipv6)
{
	ipv6_ = ipv6;
	sockfd_ = ::socket(ipv6_ ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	return sockfd_;
}

bool TcpSocket::Bind(const std::string &ip, const uint16_t port) const
{
	if (!SocketUtil::Bind(sockfd_, ip, port, ipv6_))
	{
		LOG_ERROR(" <socket=%d> bind <%s:%u> failed.\n", sockfd_,
			  ip.c_str(), port);
		return false;
	}

	return true;
}

bool TcpSocket::Listen(const int backlog) const
{
	if(::listen(sockfd_, backlog) == SOCKET_ERROR)
	{
		LOG_ERROR("<socket=%d> listen failed.\n", sockfd_);
		return false;
	}

	return true;
}

SOCKET TcpSocket::Accept() const
{
	sockaddr *psockaddr;
	socklen_t addrlen;
	if (ipv6_)
	{
		sockaddr_in6 addr = {0};
		addrlen = sizeof addr;
		psockaddr = reinterpret_cast<sockaddr *>(&addr);
	} else
	{
		sockaddr_in addr = {0};
		addrlen = sizeof addr;
		psockaddr = reinterpret_cast<sockaddr *>(&addr);
	}

	const SOCKET socket_fd = ::accept(sockfd_, psockaddr, &addrlen);

	return socket_fd;
}

bool TcpSocket::Connect(const std::string &ip, const uint16_t port, const int timeout) const
{ 
	if (!SocketUtil::Connect(sockfd_, ip, port, timeout, ipv6_))
	{
		LOG_ERROR("<socket=%d> connect failed.\n", sockfd_);
		return false;
	}

	return true;
}

void TcpSocket::Close()
{
#if defined(WIN32) || defined(_WIN32)
        closesocket(sockfd_);
#else
	::close(sockfd_);
#endif
	sockfd_ = 0;
}

void TcpSocket::ShutdownWrite()
{
	shutdown(sockfd_, SHUT_WR);
	sockfd_ = 0;
}
