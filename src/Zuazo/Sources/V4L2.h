#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <map>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "../Graphics/Frame.h"
#include "../Stream/AsyncSource.h"
#include "../Utils/Rational.h"
#include "../Utils/Resolution.h"
#include "../Utils/TimeInterval.h"
#include "../Video.h"

struct v4l2_buffer;

namespace Zuazo::Sources{

class V4L2:
		public Stream::AsyncSource<Graphics::Frame>,
		public ResizeableVideoBase
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

		int operator==(const VideoMode& other) const{
			if(resolution == other.resolution && interval == other.interval)
				return 1;
			else
				return 0;
		}

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
				return 1;
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
	void						setInterval(const Utils::TimeInterval& interval) override;
	void						setRes(const Utils::Resolution& res) override;
	void						setVideoMode(const VideoMode& mode);

	//Utils::Resolution			getRes() const override;
	VideoMode					getVideoMode() const;
	const std::set<VideoMode>&	getVideoModes() const;
private:
	struct Buffer{
		size_t						bufSize;
		u_int8_t *					buffer;

		Buffer(const v4l2_buffer& v4l2BufReq, int dev);
		Buffer(const Buffer& other)=delete;
		Buffer(Buffer&& other);
		~Buffer();

		Buffer& operator=(Buffer&& other);
	};

	int							m_dev;
	std::set<VideoMode>			m_vidModes;
	std::set<VideoMode>::iterator m_currVidMode;

	std::vector<Buffer>			m_buffers;

	std::thread					m_capturingThread;
	bool						m_threadExit;

	void						reqBuffer(v4l2_buffer* buf);
	void						freeBuffer(v4l2_buffer* buf);
	void 						mpegCapturingThread();
};

inline void V4L2::setRate(const Utils::Rational& rate){
	setVideoMode(VideoMode{
		m_currVidMode->resolution,
		1 / rate
	});
}

inline void V4L2::setInterval(const Utils::TimeInterval& interval){
	setVideoMode(VideoMode{
		m_currVidMode->resolution,
		static_cast<Utils::Rational>(interval)
	});
}

inline void V4L2::setRes(const Utils::Resolution& res){
	setVideoMode(VideoMode{
		res,
		m_currVidMode->interval
	});
}

/*inline Utils::Resolution V4L2::getRes() const{
	if(m_currVidMode != m_vidModes.end())
		return m_currVidMode->resolution;
	else
		return Utils::Resolution();
}*/

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
