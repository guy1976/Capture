#pragma once

#include <stack>
#include <mutex>
#include <thread>
#include <condition_variable>

template<class T>
class CCache
{
	std::deque<T>			m_objects;
	std::mutex              locker;
	std::condition_variable cv; 
protected:
	std::function<void(T)> m_deleter;
	virtual ~CCache()
	{
		while (!m_objects.empty())
		{
			m_deleter(m_objects.front());
			m_objects.pop_front();
		}
	}
public:
	void Add(T obj)
	{
		std::lock_guard<std::mutex> lk(locker);
		m_objects.push_back(obj);
		cv.notify_one();
	}
	T& Pop()
	{
		std::unique_lock<std::mutex> lk(locker);
		while (m_objects.empty())
			cv.wait(lk);
		// get reference to front under lock
		T& data = m_objects.front();
		m_objects.pop_front();
		return data;
	}

};