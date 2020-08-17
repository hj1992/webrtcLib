#ifndef _ATIMER_H
#define _ATIMER_H

#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <boost/timer.hpp>
#include <boost/asio.hpp>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


template<typename Duration = boost::posix_time::milliseconds>
class ATimer
{
public:
	//ATimer() : m_timer(m_ios, Duration(0)), m_isSingleShot(false) { this->user = NULL; }
	ATimer(void* user) : m_timer(m_ios, Duration(0)), m_isSingleShot(false) { this->user = user; }
	~ATimer()
	{
		stop();
	}
	
	void start(unsigned int duration)
	{
		if (m_ios.stopped())
		{
			return;
		}

		m_isActive = true;
		m_duration = duration;
		m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(m_duration));



		m_func = [this]
		{
			m_timer.async_wait([this](const boost::system::error_code&)
			{
				/*for (auto& func : m_funcVec)
				{
					func(this->user);
				}
				

				if (!m_isSingleShot)
				{
					m_timer.expires_at(m_timer.expires_at() + Duration(m_duration));
					m_func(this->user);
				}*/
				callfunc(user);
				m_timer.expires_at(m_timer.expires_at() + Duration(m_duration));
				m_func();
			});
		};

		m_func();
		m_thread = std::thread([this] { m_ios.run(); });
	}


	void stop()
	{
		if (!m_isActive)
			return;
		m_isActive = false;
		m_ios.stop();
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		
	}

	void bind(const std::function<void(void*)>& func)
	{
		//m_funcVec.emplace_back(func);
		callfunc = func;
	}

	void setSingleShot(bool isSingleShot)
	{
		m_isSingleShot = isSingleShot;
	}

	bool isSingleShot() const
	{
		return m_isSingleShot;
	}

	bool isActive() const
	{
		return m_isActive;
	}
	
private:
	boost::asio::io_service m_ios;
	boost::asio::deadline_timer m_timer;
	std::function<void(void)> m_func = nullptr;
	std::function<void(void*)> callfunc = nullptr;
	//std::vector<std::function<void(void*)>> m_funcVec;
	std::thread m_thread;
	unsigned int m_duration = 0;
	std::atomic<bool> m_isSingleShot;
	bool m_isActive = false;
	void* user;
};

#endif
