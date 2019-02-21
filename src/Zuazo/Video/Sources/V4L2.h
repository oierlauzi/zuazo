#pragma once

#include <stddef.h>
#include <cstdio>
#include <jpeglib.h>
#include <sys/types.h>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "../../Utils/Rational.h"
#include "../../Utils/Resolution.h"
#include "../Video.h"

namespace Zuazo::Video::Sources{

class V4L2:
		public AsyncVideoSource,
		public Resizeable
{
public:
	struct VideoMode{
		Utils::Resolution			resolution;
		Utils::Rational				interval;

		VideoMode()=default;
		VideoMode(const Utils::Resolution& res, const Utils::Rational& interval) :
			resolution(res),
			interval(interval)
		{
		}

		VideoMode(const VideoMode& other)=default;
		~VideoMode()=default;

		int operator<(const VideoMode& other) const{
			if(resolution < other.resolution){
				//My resolution is smaller
				return 1;
			} else if(resolution > other.resolution){
				//My resolution is bigger
				return 0;
			} else{
				//Both resolutions are equal
				if(interval < other.interval){
					//Lower interval -> Higher frame rate
					return 0;
				}else{
					return 1;
				}
			}
		}
	};

	V4L2(u_int32_t dev);
	V4L2(const std::string& devName);
	V4L2(const V4L2& other)=delete;
	~V4L2();

	void						open() override;
	void						close() override;

	void						setRate(const Utils::Rational& rate) override;
	void						setInterval(const Utils::Rational& interval) override;
	void						setRes(const Utils::Resolution& res) override;
	void						setVideoMode(const VideoMode& mode);

	Utils::Resolution			getRes() const override;
	VideoMode					getVideoMode() const;
	const std::set<VideoMode>&	getVideoModes() const;
private:
	int							m_dev;
	std::set<VideoMode>			m_vidModes;
	std::set<VideoMode>::iterator m_currVidMode;


	struct Buffer{
		u_int8_t *				buffer=NULL;
		size_t					bufSize=0;
	};
	std::vector<Buffer>			m_buffers;

	jpeg_decompress_struct 		m_decmp;
	jpeg_error_mgr				m_decmpErr;

	std::thread					m_capturingThread;
	std::mutex					m_mutex;

	void 						capturingThread();
};

inline void V4L2::setRate(const Utils::Rational& rate){
	setVideoMode(VideoMode{
		m_currVidMode->resolution,
		1 / rate
	});
}

inline void V4L2::setInterval(const Utils::Rational& interval){
	setVideoMode(VideoMode{
		m_currVidMode->resolution,
		interval
	});
}

inline void V4L2::setRes(const Utils::Resolution& res){
	setVideoMode(VideoMode{
		res,
		m_currVidMode->interval
	});
}

inline void V4L2::setVideoMode(const VideoMode& mode){
	close();
	m_currVidMode=m_vidModes.find(mode);

	if(m_currVidMode != m_vidModes.end()){
		//Video mode exists
		AsyncVideoSource::setInterval(m_currVidMode->interval);
		open();
	}
}

inline Utils::Resolution V4L2::getRes() const{
	if(m_currVidMode != m_vidModes.end())
		return m_currVidMode->resolution;
	else
		return Utils::Resolution();
}

inline V4L2::VideoMode V4L2::getVideoMode() const{
	if(m_currVidMode != m_vidModes.end())
		return *m_currVidMode;
	else
		return VideoMode();
}

inline const std::set<V4L2::VideoMode>& V4L2::getVideoModes() const{
	return m_vidModes;
}
}
