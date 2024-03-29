//Scott Xu
//2020-12-6 Add IPv6 support.
#ifndef XOP_TCP_CONNECTION_H
#define XOP_TCP_CONNECTION_H

#include <atomic>
#include <mutex>
#include "TaskScheduler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Channel.h"
#include "SocketUtil.h"

namespace xop {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	using Ptr = std::shared_ptr<TcpConnection>;
	using Weak = std::weak_ptr<TcpConnection>;
	using DisconnectCallback = std::function<void(Weak conn)>;
	using CloseCallback = std::function<void(Weak conn)>;
	using ReadCallback =
		std::function<bool(Weak conn, BufferReader &buffer)>;

	TcpConnection(SOCKET sockfd,
		      std::shared_ptr<TaskScheduler> task_scheduler);
	virtual ~TcpConnection();

	std::shared_ptr<TaskScheduler> GetTaskScheduler() const
	{
		return task_scheduler_;
	}

	void SetReadCallback(const ReadCallback &cb) { read_cb_ = cb; }

	void SetCloseCallback(const CloseCallback &cb) { close_cb_ = cb; }

	void Send(const std::shared_ptr<char> &data, size_t size);
	void Send(const char *data, size_t size);

	void Disconnect();

	bool IsClosed() const { return is_closed_; }

	bool IsIpv6() const { return ipv6_; }

	SOCKET GetSocket() const { return channel_->GetSocket(); }

	uint16_t GetPort() const
	{
		return SocketUtil::GetPeerPort(channel_->GetSocket(), ipv6_);
	}

	std::string GetIp() const
	{
		return SocketUtil::GetPeerIp(channel_->GetSocket(), ipv6_);
	}

protected:
	friend class TcpServer;

	virtual void HandleRead();
	virtual void HandleWrite();
	virtual void HandleClose();
	virtual void HandleError();

	void SetDisconnectCallback(const DisconnectCallback &cb)
	{
		disconnect_cb_ = cb;
	}

	std::unique_ptr<BufferReader> read_buffer_;
	std::unique_ptr<BufferWriter> write_buffer_;
	std::atomic_bool is_closed_;

private:
	void Close();

	std::shared_ptr<TaskScheduler> task_scheduler_;
	std::shared_ptr<Channel> channel_;
	std::mutex mutex_;
	DisconnectCallback disconnect_cb_;
	CloseCallback close_cb_;
	ReadCallback read_cb_;
	bool ipv6_;
};

}

#endif
