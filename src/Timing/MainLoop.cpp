#include <Timing/MainLoop.h>

#include <Timing/Scheduler.h>

#include <thread>
#include <mutex>
#include <condition_variable>

namespace Zuazo::Timing {

struct MainLoop::Impl {
	Impl(Scheduler& sch)
		: scheduler(sch)
		//, thread(&MainLoop::Impl::threadFunc, this)
	{
	}

	~Impl(){
		std::lock_guard<std::mutex> lock(mutex);
		//thread.get_stop_source().request_stop();
		cond.notify_one();
	}


	std::reference_wrapper<Scheduler> scheduler;
	//std::jthread thread;
	std::mutex mutex;
	std::condition_variable cond;

	void setScheduler(Scheduler& sch) {
		scheduler = sch;
	}

	Scheduler& getScheduler() const {
		return scheduler;
	}


private:
	/*void threadFunc() {
		auto st = thread.get_stop_token();
		TimePoint time = now();
		std::unique_lock<std::mutex> lock(mutex);

		while(!st.stop_requested()) {
			Scheduler& sch = scheduler;
			const auto timeForNextEvent = sch.getTimeForNextEvent();

			if(timeForNextEvent == Flicks::max()){
				//No events, wait until some arises
				cond.wait(lock);
				time = now();
			} else {
				//Sleep until the next event
				time += timeForNextEvent;

				if(cond.wait_until(lock, time) == std::cv_status::no_timeout){
					sch.setTime(sch.getTime() + timeForNextEvent);
				}
			}
		}
	}*/
};



MainLoop::MainLoop(Scheduler& scheduler)
	: m_impl(scheduler)
{
}

MainLoop::MainLoop(MainLoop&& other) = default;

MainLoop::~MainLoop() = default;

MainLoop& MainLoop::operator=(MainLoop&& other) = default;

void MainLoop::setScheduler(Scheduler& scheduler) {
	m_impl->setScheduler(scheduler);
}

Scheduler& MainLoop::getScheduler() const {
	return m_impl->getScheduler();
}








}