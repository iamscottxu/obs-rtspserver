// PHZ
// 2018-5-15
// Scott Xu
// 2020-12-2 Add IPv6 Support. 

#include "TcpSocket.h"
#include "Socket.h"
#include "SocketUtil.h"
#include "Logger.h"

using namespace xop;

TcpSocket::TcpSocket(SOCKET sockfd, bool ipv6)
	: sockfd_(sockfd), ipv6_(ipv6)
{
    
}

TcpSocket::~TcpSocket()
{
	
}

SOCKET TcpSocket::Create(bool ipv6)
{
	ipv6_ = ipv6;
	sockfd_ = ::socket(ipv6_ ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	return sockfd_;
}

bool TcpSocket::Bind(std::string ip, uint16_t port)
{
	if (!SocketUtil::Bind(sockfd_, ip, port, ipv6_))
	{
		LOG_DEBUG(" <socket=%d> bind <%s:%u> failed.\n", sockfd_,
			  ip.c_str(), port);
		return false;
	}

	return true;
}

bool TcpSocket::Listen(int backlog)
{
	if(::listen(sockfd_, backlog) == SOCKET_ERROR)
	{
		LOG_DEBUG("<socket=%d> listen failed.\n", sockfd_);
		return false;
	}

	return true;
}

SOCKET TcpSocket::Accept()
{
	struct sockaddr *psockaddr;
	socklen_t addrlen = 0;
	if (ipv6_)
	{
		struct sockaddr_in6 addr = {0};
		addrlen = sizeof addr;
		psockaddr = (struct sockaddr *)&addr;
	} else
	{
		struct sockaddr_in addr = {0};
		addrlen = sizeof addr;
		psockaddr = (struct sockaddr *)&addr;
	}

	SOCKET clientfd = ::accept(sockfd_, psockaddr, &addrlen);

	return clientfd;
}

bool TcpSocket::Connect(std::string ip, uint16_t port, int timeout)
{ 
	if (!SocketUtil::Connect(sockfd_, ip, port, timeout, ipv6_))
	{
		LOG_DEBUG("<socket=%d> connect failed.\n", sockfd_);
		return false;
	}

	return true;
}

void TcpSocket::Close()
{
#if defined(__linux) || defined(__linux__) 
    ::close(sockfd_);
#elif defined(WIN32) || defined(_WIN32)
    closesocket(sockfd_);
#else
	
#endif
	sockfd_ = 0;
}

void TcpSocket::ShutdownWrite()
{
	shutdown(sockfd_, SHUT_WR);
	sockfd_ = 0;
}
