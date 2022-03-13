// PHZ
// 2018-5-15

#include "Pipe.h"
#include "SocketUtil.h"
#include <random>

using namespace xop;

Pipe::Pipe()
{
	memset(pipe_fd_, 0, 2);
}

Pipe::~Pipe()
{
	Close();
}

bool Pipe::Create()
{
#if defined(__linux) || defined(__linux__)
	if (pipe2(pipe_fd_, O_NONBLOCK | O_CLOEXEC) < 0) {
		return false;
	}
#else
	TcpSocket rp(socket(AF_INET, SOCK_STREAM, 0)), wp(socket(AF_INET, SOCK_STREAM, 0));
	std::random_device rd;

	pipe_fd_[0] = rp.GetSocket();
	pipe_fd_[1] = wp.GetSocket();
	uint16_t port = 0;
	int again = 5;

	while(again--) {
		port = rd();
		if (rp.Bind("127.0.0.1", port)) {
			break;
		}
	}

	if (again == 0) {
		return false;
	}

	if (!rp.Listen(1)) {
		return false;
	}

	if (!wp.Connect("127.0.0.1", port)) {
		return false;
	}

	pipe_fd_[0] = rp.Accept(); //TODO
	/*if (pipe_fd_[0] < 0) {
		return false;
	}*/

	SocketUtil::SetNonBlock(pipe_fd_[0]);
	SocketUtil::SetNonBlock(pipe_fd_[1]);
#endif
	return true;
}

int Pipe::Write(void *buf, const int len) const
{
#if defined(WIN32) || defined(_WIN32)
    return ::send(pipe_fd_[1], static_cast<char *>(buf), len, 0);
#else
    return ::write(pipe_fd_[1], buf, len);
#endif 
}

int Pipe::Read(void *buf, const int len) const
{
#if defined(WIN32) || defined(_WIN32)
    return recv(pipe_fd_[0], static_cast<char *>(buf), len, 0);
#else
    return ::read(pipe_fd_[0], buf, len);
#endif 
}

void Pipe::Close() const
{
#if defined(WIN32) || defined(_WIN32) 
	closesocket(pipe_fd_[0]);
	closesocket(pipe_fd_[1]);
#else
	::close(pipe_fd_[0]);
	::close(pipe_fd_[1]);
#endif

}


