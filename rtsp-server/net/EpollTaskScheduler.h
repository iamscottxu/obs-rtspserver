// PHZ
// 2018-5-15

#ifndef XOP_EPOLL_TASK_SCHEDULER_H
#define XOP_EPOLL_TASK_SCHEDULER_H

#include "TaskScheduler.h"
#include <mutex>
#include <unordered_map>

namespace xop {
class EpollTaskScheduler : public TaskScheduler {
public:
	EpollTaskScheduler(int id = 0);
	~EpollTaskScheduler() override;

	void UpdateChannel(const ChannelPtr &channel) override;
	void RemoveChannel(const ChannelPtr &channel) override;

	// timeout: ms
	bool HandleEvent(int timeout) override;

private:
	void Update(int operation, ChannelPtr &channel);

	int epollfd_ = -1;
	std::mutex mutex_;
	std::unordered_map<int, ChannelPtr> channels_;
};

}

#endif
