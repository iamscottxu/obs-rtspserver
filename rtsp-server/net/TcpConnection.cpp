//Scott Xu
//2020-12-6 Add IPv6 support.
#include "TcpConnection.h"
#include "SocketUtil.h"

using namespace xop;

TcpConnection::TcpConnection(
	const SOCKET sockfd, std::shared_ptr<TaskScheduler> task_scheduler)
	: read_buffer_(new BufferReader),
	  write_buffer_(new BufferWriter(500)),
	  is_closed_(false),
	  task_scheduler_(std::move(task_scheduler)),
	  channel_(new Channel(sockfd)),
	  ipv6_(SocketUtil::IsIpv6Socket(sockfd))
{
	channel_->SetReadCallback([this] { this->HandleRead(); });
	channel_->SetWriteCallback([this] { this->HandleWrite(); });
	channel_->SetCloseCallback([this] { this->HandleClose(); });
	channel_->SetErrorCallback([this] { this->HandleError(); });

	SocketUtil::SetNonBlock(sockfd);
	SocketUtil::SetSendBufSize(sockfd, 100 * 1024);
	SocketUtil::SetKeepAlive(sockfd);

	channel_->EnableReading();
	task_scheduler_->UpdateChannel(channel_);
}

TcpConnection::~TcpConnection()
{
	if (const SOCKET fd = channel_->GetSocket(); fd > 0) {
		SocketUtil::Close(fd);
	}
}

void TcpConnection::Send(const std::shared_ptr<char> &data, const size_t size)
{
	if (!is_closed_) {
		mutex_.lock();
		write_buffer_->Append(data, size);
		mutex_.unlock();

		this->HandleWrite();
	}
}

void TcpConnection::Send(const char *data, const size_t size)
{
	if (!is_closed_) {
		mutex_.lock();
		write_buffer_->Append(data, size);
		mutex_.unlock();

		this->HandleWrite();
	}
}

void TcpConnection::Disconnect()
{
	std::lock_guard lock(mutex_);
	task_scheduler_->AddTriggerEvent([this] { this->Close(); });
}

void TcpConnection::HandleRead()
{
	{
		std::lock_guard lock(mutex_);

		if (is_closed_) {
			return;
		}

		if (const int ret = read_buffer_->Read(channel_->GetSocket());
		    ret <= 0) {
			this->Close();
			return;
		}
	}

	if (read_cb_) {
		if (const bool ret =
			    read_cb_(weak_from_this(), *read_buffer_);
		    !ret) {
			std::lock_guard lock(mutex_);
			this->Close();
		}
	}
}

void TcpConnection::HandleWrite()
{
	if (is_closed_) {
		return;
	}

	//std::lock_guard<std::mutex> lock(mutex_);
	if (!mutex_.try_lock()) {
		return;
	}

	bool empty;
	//do
	//{
	if (const int ret = write_buffer_->Send(channel_->GetSocket());
	    ret < 0) {
		this->Close();
		mutex_.unlock();
		return;
	}
	empty = write_buffer_->IsEmpty();
	//} while (false);

	if (empty) {
		if (channel_->IsWriting()) {
			channel_->DisableWriting();
			task_scheduler_->UpdateChannel(channel_);
		}
	} else if (!channel_->IsWriting()) {
		channel_->EnableWriting();
		task_scheduler_->UpdateChannel(channel_);
	}

	mutex_.unlock();
}

void TcpConnection::Close()
{
	if (!is_closed_) {
		is_closed_ = true;
		task_scheduler_->RemoveChannel(channel_);

		if (close_cb_) {
			close_cb_(weak_from_this());
		}

		if (disconnect_cb_) {
			disconnect_cb_(weak_from_this());
		}
	}
}

void TcpConnection::HandleClose()
{
	std::lock_guard lock(mutex_);
	this->Close();
}

void TcpConnection::HandleError()
{
	std::lock_guard lock(mutex_);
	this->Close();
}
