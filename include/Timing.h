#pragma once

#include "UpdateableBase.h"
#include "Utils/Rational.h"

#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <set>
#include <condition_variable>
#include <chrono>

namespace Zuazo{

class Timing{
public:
    typedef std::chrono::steady_clock::time_point   TimePoint;
    typedef TimePoint::duration                     Duration;
    static constexpr Utils::Rational duationPeriod=Utils::Rational(
        static_cast<int64_t>(Duration::period::num), 
        static_cast<int64_t>(Duration::period::den)
    );

    class PeriodicUpdate : public virtual UpdateableBase {
    public:
        PeriodicUpdate(UpdateableBase::UpdatePriority prior);
        PeriodicUpdate(UpdateableBase::UpdatePriority prior, const Duration& interval);
        PeriodicUpdate(UpdateableBase::UpdatePriority prior, const Utils::Rational& rate);
        PeriodicUpdate(const PeriodicUpdate& other)=default;
        virtual ~PeriodicUpdate();

        virtual void 						setInterval(const Duration& interval);
        virtual void						setRate(const Utils::Rational& rate);

        Duration					        getInterval() const;
        Utils::Rational						getRate() const;

        virtual void 						enable() override;
        virtual void 						disable() override;
    private:
        Duration				            m_updateInterval;
    };
    friend PeriodicUpdate;

    class RegularUpdate : public virtual UpdateableBase {
    public:
        RegularUpdate(UpdateableBase::UpdatePriority prior);
        RegularUpdate(const RegularUpdate& other)=default;
        virtual ~RegularUpdate();

        virtual void enable() override;
        virtual void disable() override;
    };
    friend RegularUpdate;

	Timing();
	Timing(const Timing& other)=delete;
    Timing(Timing&& other)=delete;
	~Timing();

	const TimePoint&		            now() const;
	const Duration&	                    getElapsed() const;

	void						        lock() const;
	void						        unlock() const;
	bool						        try_lock() const;

    static Timing*                      getTiming();
    static void                         init();
    static void                         end();
private:
    typedef std::map<UpdateableBase::UpdatePriority, std::set<const UpdateableBase*>> OrderedUpdates;

    struct UpdatePeriod{
        OrderedUpdates                  periodicUpdates;
        mutable Duration		        timeSinceLastUpdate;
    };

    std::map<Duration, UpdatePeriod>    m_periodicUpdates;
    OrderedUpdates                      m_regularUpdates;

	mutable TimePoint	                m_currTime;
	mutable Duration	                m_elapsed;

    std::thread 				        m_thread;
	bool						        m_exit;
	mutable std::mutex 			        m_mutex;
	mutable std::condition_variable	    m_cond;

    void 						        addTiming(const PeriodicUpdate* event);
	void 						        addTiming(const RegularUpdate* event);
	void 						        deleteTiming(const PeriodicUpdate* event);
	void 						        deleteTiming(const RegularUpdate* event);
	void 						        modifyTiming(const PeriodicUpdate* event);

	void						        threadFunc() const;

    static std::unique_ptr<Timing>      s_timing;

};

inline Timing::Duration operator*(const Timing::Duration& dur, double d){
    return Timing::Duration(static_cast<Timing::Duration::rep>(dur.count() * d));
}

inline Timing::Duration operator*(double d, const Timing::Duration& dur){
    return dur * d;
}

/*
 * PERIODIC UPDATE'S INLINE METHOD DEFINITIONS
 */

inline Timing::PeriodicUpdate::PeriodicUpdate(UpdateableBase::UpdatePriority prior) :
    UpdateableBase(prior)
{
}

inline Timing::PeriodicUpdate::PeriodicUpdate(UpdateableBase::UpdatePriority prior, const Duration& interval) :
    UpdateableBase(prior),
	m_updateInterval(interval)
{
}

inline Timing::PeriodicUpdate::PeriodicUpdate(UpdateableBase::UpdatePriority prior, const Utils::Rational& rate) :
    UpdateableBase(prior),
	m_updateInterval(
        rate ? 
        Duration(static_cast<Duration::rep>(1 / (duationPeriod * rate))) : 
        Duration::zero()
    )
{
}

inline Timing::PeriodicUpdate::~PeriodicUpdate(){
    disable();
}

inline void Timing::PeriodicUpdate::setInterval(const Duration& interval){
	m_updateInterval=interval;
	Timing::getTiming()->modifyTiming(this);
}

inline void Timing::PeriodicUpdate::setRate(const Utils::Rational& rate){
    m_updateInterval=rate ? 
        Duration(static_cast<Duration::rep>(1 / (duationPeriod * rate))) : 
        Duration::zero();
    Timing::getTiming()->modifyTiming(this);
}


inline Timing::Duration Timing::PeriodicUpdate::getInterval() const {
	return m_updateInterval;
}

inline Utils::Rational Timing::PeriodicUpdate::getRate() const {
	return Utils::Rational(1 / (duationPeriod * m_updateInterval.count()));
}

inline void Timing::PeriodicUpdate::enable(){
	Timing::getTiming()->addTiming(this);
}

inline void Timing::PeriodicUpdate::disable(){
    Timing* timing=Timing::getTiming();
	if(timing)
		timing->deleteTiming(this);
}

/*
 * REGULAR UPDATE'S INLINE METHOD DEFINITIONS
 */

inline Timing::RegularUpdate::RegularUpdate(UpdateableBase::UpdatePriority prior) :
    UpdateableBase(prior)
{
}

inline Timing::RegularUpdate::~RegularUpdate(){
    disable();
}

inline void Timing::RegularUpdate::enable(){
	Timing::getTiming()->addTiming(this);
}

inline void Timing::RegularUpdate::disable(){
   	Timing* timing=Timing::getTiming();
	if(timing)
		timing->deleteTiming(this);
}

/*
 * TIMING'S INLINE METHOD DEFINITIONS
 */

inline const Timing::TimePoint& Timing::now() const{
	return m_currTime;
}

inline const Timing::Duration& Timing::getElapsed() const{
	return m_elapsed;
}

inline void Timing::lock() const{
	m_mutex.lock();
}

inline void Timing::unlock() const{
	m_mutex.unlock();
}

inline bool Timing::try_lock() const{
	return m_mutex.try_lock();
}

inline Timing* Timing::getTiming(){
    return s_timing.get();
}

inline void Timing::init(){
    s_timing=std::unique_ptr<Timing>(new Timing);
}

inline void Timing::end(){
    s_timing.reset();
}

}