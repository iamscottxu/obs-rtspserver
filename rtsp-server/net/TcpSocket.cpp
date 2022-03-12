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
		LOG_ERROR(" <socket=%d> bind <%s:%u> failed.\n", sockfd_,
			  ip.c_str(), port);
		return false;
	}

	return true;
}

bool TcpSocket::Listen(int backlog)
{
	if(::listen(sockfd_, backlog) == SOCKET_ERROR)
	{
		LOG_ERROR("<socket=%d> listen failed.\n", sockfd_);
		return false;
	}

	return true;
}

std::tuple<SOCKET, std::string, int> TcpSocket::Accept()
{
	socklen_t addrlen;
	std::tuple<SOCKET, std::string, int> ret;
	if (ipv6_)
	{
		sockaddr_in6 addr = {0};
		addrlen = sizeof addr;
		std::get<0>(ret) = ::accept(
			sockfd_, reinterpret_cast<sockaddr *>(&addr), &addrlen);
		if (std::get<0>(ret) > 0) {
			std::get<1>(ret).resize(INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &addr.sin6_addr,
				std::get<1>(ret).data(), INET6_ADDRSTRLEN);
			std::get<2>(ret) = htons(addr.sin6_port);
		}
	} else
	{
		sockaddr_in addr = {0};
		addrlen = sizeof addr;
		std::get<0>(ret) = ::accept(
			sockfd_, reinterpret_cast<sockaddr *>(&addr), &addrlen);
		if (std::get<0>(ret) > 0) {
			std::get<1>(ret).resize(INET_ADDRSTRLEN);
			inet_ntop(AF_INET, &addr.sin_addr,
				  std::get<1>(ret).data(), INET_ADDRSTRLEN);
			std::get<2>(ret) = htons(addr.sin_port);
		}
	}
	return ret;
}

bool TcpSocket::Connect(std::string ip, uint16_t port, int timeout)
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
