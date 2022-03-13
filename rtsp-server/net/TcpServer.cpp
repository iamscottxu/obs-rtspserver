// Scott Xu
// 2020-12-04 Add multiple socket support.
#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Logger.h"
#include <cstdio>  
#include <utility>

using namespace xop;
using namespace std;

TcpServer::TcpServer(EventLoop* event_loop)
	: event_loop_(event_loop)
	//, port_(0)
	//, acceptor_(new Acceptor(event_loop_))
	//, is_started_(false)
{

}

TcpServer::~TcpServer()
{
	TcpServer::Stop();
}

bool TcpServer::Start(const std::string &ip, const uint16_t port)
{
	/*Stop();

	if (!is_started_) {
		if (acceptor_->Listen(ip, port) < 0) {
			return false;
		}

		port_ = port;
		ip_ = ip;
		is_started_ = true;
		return true;
	}*/
	//return false;

	auto acceptor = std::make_unique<Acceptor>(event_loop_);
	acceptor->SetNewConnectionCallback([this](SOCKET sockfd) {
		if (const TcpConnection::Ptr conn = this->OnConnect(sockfd)) {
			this->AddConnection(sockfd, conn);
			conn->SetDisconnectCallback([this](const TcpConnection::Ptr
				&conn) {
				const auto scheduler = conn->GetTaskScheduler();
				if (SOCKET sockfd = conn->GetSocket(); !scheduler->AddTriggerEvent([this, sockfd] {this->RemoveConnection(sockfd); })) {
					scheduler->AddTimer([this, sockfd]() {this->RemoveConnection(sockfd); return false; }, 100);
				}
			});
		}
	});
	if (acceptor->Listen(ip, port) < 0) return false;
	acceptors_.push_back(std::move(acceptor));
	return true;
}

void TcpServer::Stop()
{
	/*if (is_started_) {
		
		mutex_.lock();
		for (auto iter : connections_) {
			iter.second->Disconnect();
		}
		mutex_.unlock();

		acceptor_->Close();
		is_started_ = false;

		while (1) {
			Timer::Sleep(1);
			if (connections_.empty()) {
				break;
			}
		}
	}*/
	if (acceptors_.empty())
		return;

	mutex_.lock();
	for (const auto iter : connections_)
		iter.second->Disconnect();
	mutex_.unlock();

	for (auto it = acceptors_.begin(); it != acceptors_.end(); ++it)
		(*it)->Close();

	while (!connections_.empty())
		Timer::Sleep(10);

	acceptors_.clear();

	return;
}

TcpConnection::Ptr TcpServer::OnConnect(const SOCKET sockfd)
{
	return std::make_shared<TcpConnection>(event_loop_->GetTaskScheduler().get(), sockfd);
}

void TcpServer::AddConnection(const SOCKET sockfd, TcpConnection::Ptr tcpConn)
{
	std::lock_guard locker(mutex_);
	connections_.emplace(sockfd, tcpConn);
}

void TcpServer::RemoveConnection(const SOCKET sockfd)
{
	std::lock_guard locker(mutex_);
	connections_.erase(sockfd);
}
