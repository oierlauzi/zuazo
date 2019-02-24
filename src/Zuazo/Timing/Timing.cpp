#include "Timing.h"

#include <chrono>
#include <map>

#include "../Updateable.h"

namespace Zuazo::Timing{

/*
 * Static variables
 */
std::unique_ptr<Timings> timings;


/*
 * Timings' functions
 */

Timings::Timings(){
	m_exit=false;
	m_currTime=TimePoint::now();
	m_thread=std::thread(&Timings::threadFunc, this);
}

Timings::~Timings(){
	m_exit=true;
	m_cond.notify_all();
	m_thread.join();
}

void Timings::threadFunc(){
	std::unique_lock<std::mutex> lock(m_mutex);

	while(!m_exit){

		#ifdef ENABLE_LOAD_DEBUG
		Chronometer chrono;
		chrono.start();
		#endif

		TimingTable::PendingUpdates updates=m_timingTable.getPendingUpdates();

		if(updates.updateables.size()){
			//Make all the needed updates

			//Sort by update priority
			//std::sort(updates.updateables.begin(), updates.updateables.end());
			/*
			 * No need, its actually sorted by default
			 */

			{
				//Graphics::UniqueContext ctx(Graphics::Context::getMainCtx()); //Not needed
				Graphics::UniqueContext ctx(m_ctx);

				//Update all
				for(auto ite=updates.updateables.begin(); ite!=updates.updateables.end(); ++ite){
					for(const Updateable * updateable : ite->second){
						updateable->perform();
					}
				}
			}

			// Increment timing values for each interval and for currTime
			m_currTime+=updates.timeForNextUpdate;
			m_timingTable.incrTime(updates.timeForNextUpdate);

			//Wait until next update
			lock.unlock();
			std::this_thread::sleep_until(m_currTime);
			lock.lock();
		}else{
			m_cond.wait(lock);
		}

		#ifdef ENABLE_LOAD_DEBUG
		chrono.end();
		printf("Elapsed: %ldus\n", chrono.getElapsed().count());
		#endif
	}
}

/*
 * init() and end()
 */
int init(){
	timings=std::unique_ptr<Timings>(new Timings());
	return 0;
}

int end(){
	timings=std::unique_ptr<Timings>();
	return 0;
}

}
