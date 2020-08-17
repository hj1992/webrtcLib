#define _CRT_SECURE_NO_WARNINGS
//#include "config.h"
#include "timer.h"
#ifdef _MSC_VER
# include <sys/timeb.h>
#else
# include <sys/time.h>
#endif

//////////////////////////////////////////////////////////////////////////
// Timer

/*Timer::Timer(){
}

Timer::~Timer()
{
	Stop();
}

void Timer::Stop()
{

}

void Timer::OnTimer(unsigned long long now)
{
	timerFun_();
}

void Timer::Start(Fun fun, unsigned interval, TimerType timeType)
{
	Stop();
	
}

void print(const boost::system::error_code&)
{
	std::cout << "Hello, world!" << std::endl;
}

void handle_wait(const boost::system::error_code& error,
	boost::asio::deadline_timer& t,
	int& count)
{
	if (!error)
	{
		std::cout << count << std::endl;
		if (count++ < 5)
		{
			t.expires_from_now(boost::posix_time::seconds(3));
			t.async_wait(boost::bind(handle_wait,
				boost::asio::placeholders::error,
				boost::ref(t),
				boost::ref(count)));
			if (count == 3)
			{
				t.cancel();
			}

		}
	}
}

// ͬ������  
void test_timer_syn()
{
	boost::asio::io_service ios;
	//boost::posix_time::milliseconds(100);
	boost::asio::deadline_timer t(ios, boost::posix_time::seconds(3));
	t.wait();
	std::cout << "syn deadline_timer!" << std::endl;
}

// �첽����:3���ִ��print����
void test_timer_asyn()
{
	boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(3));
	t.async_wait(print);
	std::cout << "asyn deadline_timer!" << std::endl;
	io.run();
}

// �첽ѭ��ִ�з���
void test_timer_asyn_loop()
{
	boost::asio::io_service io;
	boost::asio::deadline_timer t(io);
	size_t a = t.expires_from_now(boost::posix_time::seconds(1));

	int count = 1;
	t.async_wait(boost::bind(handle_wait,
		boost::asio::placeholders::error,
		boost::ref(t),
		boost::ref(count)));
	io.run();
}*/
