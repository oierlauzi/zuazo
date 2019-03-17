#include "ZuazoBase.h"

using namespace Zuazo;

ZuazoBase::AsyncOCThread ZuazoBase::s_asyncOpenCloseThread;

void ZuazoBase::open(){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_isOpen=true;
}

void ZuazoBase::asyncOpen(){
	AsyncOCThread::Job job={
		.type=AsyncOCThread::Job::JobType::Open,
		.target=*this
	};

	s_asyncOpenCloseThread.addJob(job);
}

void ZuazoBase::close(){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_isOpen=false;
}

void ZuazoBase::asyncClose(){
	AsyncOCThread::Job job={
		.type=AsyncOCThread::Job::JobType::Close,
		.target=*this
	};

	s_asyncOpenCloseThread.addJob(job);
}

bool ZuazoBase::isOpen() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_isOpen;
}

ZuazoBase::AsyncOCThread::AsyncOCThread():
		m_exit(false),
		m_thread(&AsyncOCThread::threadFunc, this)
{
}

ZuazoBase::AsyncOCThread::~AsyncOCThread(){
	m_exit=true;
	m_cond.notify_all();
	m_thread.join();
}

void ZuazoBase::AsyncOCThread::addJob(const Job& job){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_jobs.emplace(job);
	m_cond.notify_one();
}

void ZuazoBase::AsyncOCThread::threadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex);
	while(!m_exit){
		if(m_jobs.empty())
			m_cond.wait(lock);

		if(m_jobs.size()){
			switch(m_jobs.front().type){
			case Job::JobType::Open:
				m_jobs.front().target.open();
				break;

			case Job::JobType::Close:
				m_jobs.front().target.close();
				break;
			}

			m_jobs.pop();
		}
	}
}
