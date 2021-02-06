// Scott Xu
// 2021-1-19

#ifndef XOP_KQUEUE_TASK_SCHEDULER_H
#define XOP_KQUEUE_TASK_SCHEDULER_H

#include "TaskScheduler.h"
#include <mutex>
#include <unordered_map>

namespace xop
{	
class KqueueTaskScheduler : public TaskScheduler
{
public:
        KqueueTaskScheduler(int id = 0);
	virtual ~KqueueTaskScheduler();

	void UpdateChannel(ChannelPtr channel);
	void RemoveChannel(ChannelPtr& channel);

	// timeout: ms
	bool HandleEvent(int timeout);

private:
	void Update(int operation, ChannelPtr& channel);

        int kqueuefd_ = -1;
	std::mutex mutex_;
	std::unordered_map<int, ChannelPtr> channels_;
};

}

#endif
