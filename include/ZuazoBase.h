#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace Zuazo::Timing{
class Timings;
}

namespace Zuazo{
/*
 * Class which defines a periodical event
 */
class ZuazoBase{
public:
	ZuazoBase()=default;
	ZuazoBase(const ZuazoBase& other)=default;
	virtual ~ZuazoBase()=default;

	virtual void						open();
	void								asyncOpen();
	virtual void						close();
	void								asyncClose();
	bool								isOpen() const;
private:
	class AsyncOCThread{
	public:
		AsyncOCThread();
		AsyncOCThread(const AsyncOCThread& other)=delete;
		~AsyncOCThread();

		struct Job{
			enum class JobType{
				Open,
				Close,
			};

			JobType	type;
			ZuazoBase& target;
		};

		void								addJob(const Job& job);
	private:
		std::queue<Job>						m_jobs;

		bool								m_exit;
		std::thread							m_thread;
		std::mutex							m_mutex;
		std::condition_variable				m_cond;

		void								threadFunc();
	};

	mutable std::mutex					m_mutex;

	bool								m_isOpen=false;

	static AsyncOCThread				s_asyncOpenCloseThread;
};



}
