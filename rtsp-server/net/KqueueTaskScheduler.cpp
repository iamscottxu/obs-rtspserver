// Scott Xu
// 2021-1-19

#include "KqueueTaskScheduler.h"

#if defined(__APPLE__) || defined(__MACH__)
#include <sys/event.h>
#include <errno.h>
#endif

using namespace xop;

KqueueTaskScheduler::KqueueTaskScheduler(int id) : TaskScheduler(id)
{
#if defined(__APPLE__) || defined(__MACH__)
        kqueuefd_ = kqueue();
#endif
	this->KqueueTaskScheduler::UpdateChannel(wakeup_channel_);
}

KqueueTaskScheduler::~KqueueTaskScheduler() {}

void KqueueTaskScheduler::UpdateChannel(ChannelPtr channel)
{
	std::lock_guard<std::mutex> lock(mutex_);
#if defined(__APPLE__) || defined(__MACH__)
        int fd = channel->GetSocket();
        if (channels_.find(fd) != channels_.end()) {
                if (channel->IsNoneEvent()) {
                        Update(EV_DELETE, channel);
                        channels_.erase(fd);
                } else {
                        Update(EV_ADD|EV_ENABLE, channel);
                }
        } else {
                if (!channel->IsNoneEvent()) {
                        channels_.emplace(fd, channel);
                        Update(EV_ADD|EV_ENABLE, channel);
                }
        }
#endif
}

void KqueueTaskScheduler::Update(int operation, ChannelPtr &channel)
{
#if defined(__APPLE__) || defined(__MACH__)
        struct kevent events[2] = {0};
        int num_events = 0;

	if (channel->IsReading())
		EV_SET(&events[num_events++], channel->GetSocket(), EVFILT_READ, operation, NULL, NULL, channel.get());
        if (channel->IsWriting())
                EV_SET(&events[num_events++], channel->GetSocket(), EVFILT_WRITE, operation, NULL, NULL, channel.get());

        if (kevent(kqueuefd_, events, num_events, nullptr, 0, nullptr) <
            0) {
        }
#endif
}

void KqueueTaskScheduler::RemoveChannel(ChannelPtr &channel)
{
	std::lock_guard lock(mutex_);
#if defined(__APPLE__) || defined(__MACH__)
        int fd = channel->GetSocket();

        if (channels_.find(fd) != channels_.end()) {
                Update(EV_DELETE, channel);
                channels_.erase(fd);
        }
#endif
}

bool KqueueTaskScheduler::HandleEvent(int timeout)
{
#if defined(__APPLE__) || defined(__MACH__)
        struct kevent events[512] = {0};
        int num_events = -1;

	if (timeout > 0) {
                struct timespec _timeout = {0};
                _timeout.tv_sec = timeout / 1000;
                _timeout.tv_nsec = (timeout % 1000) * 1000 * 1000;
                num_events = kevent(kqueuefd_, nullptr, 0, events, 512, &_timeout);
	} else
                num_events = kevent(kqueuefd_, nullptr, 0, events, 512, nullptr);
        if (num_events < 0) {
                if (errno != EINTR) {
                        return false;
                }
        }

        for (int n = 0; n < num_events; n++) {
                auto filter = events[n].filter;
                auto flags = events[n].flags;
                auto channel = (Channel *)events[n].udata;

                if (!channel) continue;

		int handleEventEvents = EVENT_NONE;
		if (filter == EVFILT_READ)
                        handleEventEvents = EVENT_IN;
		else if (filter == EVFILT_WRITE)
			handleEventEvents = EVENT_OUT;

		if (flags & EV_ERROR)
                        handleEventEvents = handleEventEvents | EVENT_ERR;
		if (flags & EV_EOF)
                        handleEventEvents = handleEventEvents | EVENT_HUP;

                channel->HandleEvent(handleEventEvents);
        }
        return true;
#else
	return false;
#endif
}
