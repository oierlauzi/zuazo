#pragma once

#include <bits/stdint-intn.h>
#include <sys/types.h>
#include <chrono>
#include <mutex>

#include "../Graphics/Frame.h"
#include "../Stream/AsyncSource.h"
#include "../Stream/Consumer.h"
#include "../Stream/Delay.h"
#include "../Stream/LazySource.h"
#include "../Stream/Source.h"
#include "../Stream/SyncConsumer.h"
#include "../Timing/TimeInterval.h"
#include "../Timing/Timing.h"
#include "../Utils/Rational.h"
#include "../Utils/ScalingModes.h"

namespace Zuazo::Media{

/**
 * @brief //TODO
 */
typedef Stream::Source<Graphics::Frame> VideoSource;

/**
 * @brief //TODO
 */
typedef Stream::AsyncSource<Graphics::Frame> AsyncVideoSource;

/**
 * @brief //TODO
 */
typedef Stream::LazySource<Graphics::Frame> LazyVideoSource;

/**
 * @brief //TODO
 */
typedef Stream::Consumer<Graphics::Frame> VideoConsumer;

/**
 * @brief //TODO
 */
typedef Stream::SyncConsumer<Graphics::Frame> SyncVideoConsumer;

/**
 * @brief //TODO
 */
typedef Stream::Delay<Graphics::Frame> VideoDelay;

class Resizeable{
public:
	virtual ~Resizeable()=default;

	virtual void setRes(const Utils::Resolution& res)=0;
	virtual Utils::Resolution getRes() const=0;
};

class VideoScaler{
public:
	virtual ~VideoScaler()=default;

	virtual void setScalingMode(Utils::ScalingModes mode)=0;
	virtual Utils::ScalingModes getScalingMode() const=0;
};

class Clip : virtual public Updateable
{
public:
	Clip() :
		m_duration(Timing::TimeInterval::zero())
	{
		m_hasOverflow=false;
	}

	virtual ~Clip()=default;

	void gotoTime(const Timing::TimeInterval& tp){
		if(m_currTime != tp){
			m_currTime=tp;

			if(m_currTime >=  m_duration){
				//New time is longer than the duration
				m_hasOverflow=true;
				m_currTime %= m_duration;

			}else if(m_currTime < Timing::TimeInterval::zero()){
				//New time is negative
				m_hasOverflow=true;

				while(m_currTime < Timing::TimeInterval::zero()){
					m_currTime += m_duration;
				}
			}

			perform();
		}
	}

	void gotoProgress(double p){
		gotoTime(Timing::TimeInterval(
				static_cast<int64_t>(m_duration.count() * p)
		));
	}

	void addTime(const Timing::TimeInterval& time){
		gotoTime(m_currTime + time);
	}

	void subsTime(const Timing::TimeInterval& time){
		gotoTime(m_currTime - time);
	}

	bool hasOverflow() const{
		return m_hasOverflow;
	}

	void resetOverflow(){
		m_hasOverflow=false;
	}

	Timing::TimeInterval getCurrentTime() const{
		return m_currTime;
	}

	Timing::TimeInterval getDuration() const{
		return m_duration;
	}

	double getProgress() const{
		return (double)m_currTime.count() / m_duration.count() ;
	}
protected:
	void setInfo(const Timing::TimeInterval& duration){
		m_duration=duration;
		m_currTime%=m_duration;
	}
private:
	Timing::TimeInterval		m_currTime;
	Timing::TimeInterval 		m_duration;
	bool						m_hasOverflow;
};


class VideoClip
	: public Clip
	, public VideoSource
{
public:
	VideoClip() :
		m_nbFrames(0),
		m_frameInterval()
	{
	}

	virtual ~VideoClip()=default;


	void nextFrame(){
		Clip::addTime(m_frameInterval);
	}

	void prevFrame(){
		Clip::addTime(m_frameInterval);
	}

	void gotoFrame(u_int64_t nb){
		gotoProgress((double)nb / m_nbFrames);
	}

	Utils::Rational getInterval() const {
		return m_frameInterval;
	}

	Utils::Rational getRate() const {
		return 1 / m_frameInterval;
	}

	u_int64_t getFrameNb() const {
		return m_nbFrames;
	}

	Utils::Resolution getRes() const {
		return m_resolution;
	}

	void open() override{
		std::lock_guard<Timing::Timings> lock(*Timing::timings);
		VideoSource::open();
		Clip::open();
	}

	void close() override{
		std::lock_guard<Timing::Timings> lock(*Timing::timings);
		Clip::close();
		VideoSource::close();
	}
protected:
	void setInfo(const Timing::TimeInterval& duration, u_int64_t frames, const Utils::Resolution& res){
		Clip::setInfo(duration);
		m_nbFrames=frames;
		m_frameInterval=Utils::Rational(duration.count(), frames * Timing::TimeInterval::TIME_UNITS_PER_SECOND);
		m_resolution=res;
	}
private:
	u_int64_t				m_nbFrames;
	Utils::Rational			m_frameInterval;
	Utils::Resolution		m_resolution;
};


}
