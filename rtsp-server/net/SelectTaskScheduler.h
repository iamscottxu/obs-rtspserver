// PHZ
// 2018-5-15

#ifndef XOP_SELECT_TASK_SCHEDULER_H
#define XOP_SELECT_TASK_SCHEDULER_H

#include "TaskScheduler.h"
#include "Socket.h"
#include <mutex>
#include <unordered_map>

#if defined(WIN32) || defined(_WIN32)
#else
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace xop
{	

class SelectTaskScheduler : public TaskScheduler
{
public:
	explicit SelectTaskScheduler(int id = 0);
	~SelectTaskScheduler() override;

	void UpdateChannel(ChannelPtr channel) override;
	void RemoveChannel(ChannelPtr& channel) override;
	bool HandleEvent(int timeout) override;
	
private:
	fd_set fd_read_backup_{};
	fd_set fd_write_backup_{};
	fd_set fd_exp_backup_{};
	SOCKET maxfd_ = 0;

	bool is_fd_read_reset_ = false;
	bool is_fd_write_reset_ = false;
	bool is_fd_exp_reset_ = false;

	std::mutex mutex_;
	std::unordered_map<SOCKET, ChannelPtr> channels_;
};

}

#endif
