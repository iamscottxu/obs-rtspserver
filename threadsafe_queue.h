
#ifndef __THREADSAFE_QUEUE_H_
#define __THREADSAFE_QUEUE_H_


#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>

using namespace std;
template<typename T> class threadsafe_queue
{
public:
	threadsafe_queue(size_t size_limit)
		: size_limit(size_limit), m_termination(false)
	{
	}
	~threadsafe_queue() = default;

	/**
	 * 1. When termination is not called, one element is dequeued every time the 
	 *    queue is called until the queue is empty. This method blocks the thread.
         * 2. After termination is called, this method will never block. If it is 
         *    already in a blocked state, contact the blocked state.
         * 3. When true is returned, the value is valid. When false is returned, value 
         *    is invalid. Returns false when termination is called and the queue is empty.
	 **/

	//return nullptr if the queue is empty
	std::shared_ptr<T> wait_and_pop()
	{
		unique_lock<mutex> lk(mut);
		data_cond.wait(lk, [this] {
			return !data_queue.empty() ||
			       m_termination.load(memory_order_acquire);
		});

		//dequeue if not empty
		if (!data_queue.empty())
		{
			shared_ptr<T> res = data_queue.front();
			data_queue.pop();
			return res;
		}

		//If the queue is empty, return nullptr
		return nullptr;
	}

	//return false if the queue is empty
	bool wait_and_pop(T &&value)
	{
		shared_ptr<T> res = wait_and_pop();
		if (res == nullptr)
			return false;
		value = std::move(res);
		return true;
	}

	//return nullptr if the queue is empty
	std::shared_ptr<T> try_pop()
	{
		lock_guard<mutex> lk(mut);

		//dequeue if not empty
		if (!data_queue.empty())
		{
			shared_ptr<T> res = data_queue.front();
			data_queue.pop();
			return res;
		}

		//If the queue is empty, return nullptr
		return nullptr;
	}

	//return false if the queue is empty
	bool try_pop(T &&value)
	{
		shared_ptr<T> res = try_pop();
		if (res == nullptr)
			return false;
		value = std::move(res);
		return true;
	}

	//insert queue, move
	void move_push(T &&new_value)
	{
		if (m_termination.load(memory_order_acquire))
			return;
		shared_ptr<T> data(make_shared<T>(std::move(new_value)));
		unique_lock<mutex> lk(mut);
		data_queue.push(data);
		if (data_queue.size() > size_limit) {
			data_queue.pop();
			m_dropped_count.fetch_add(1, memory_order_relaxed);
		}
		data_cond.notify_one();
	}

	//insert queue
	void push(T new_value)
	{
		move_push(new_value);
	}

	bool empty()
	{
		unique_lock<mutex> lk(mut);
		return data_queue.empty();
	}

	size_t size()
	{
		unique_lock<mutex> lk(mut);
		return data_queue.size();
	}

	size_t dropped_count() const
	{
		return m_dropped_count.load(memory_order_relaxed);
	}

	//Set this queue to terminated state.
	//In the exit state, the enqueue is ignored, and the dequeue can be performed.
	//When the queue is empty, wait_and_pop will not block.
	void termination()
	{
		unique_lock<mutex> lk(mut);
		m_termination.store(true, memory_order_release);
		data_cond.notify_all();
	}

	//Get whether this queue is terminated
	bool is_termination() const
	{
		return m_termination.load(memory_order_acquire);
	}

private:
	mutex mut;
	queue<shared_ptr<T>> data_queue;
	const size_t size_limit;
	condition_variable data_cond;
	atomic_bool m_termination;
	atomic_size_t m_dropped_count;
};

#endif
