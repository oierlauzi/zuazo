#pragma once

extern "C"{
#include <libavutil/rational.h>
}

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "../Stream/Source.h"
#include "../Timing/NonLinear.h"
#include "../Timing/UpdateOrder.h"
#include "../Utils/TimeInterval.h"
#include "../Video/VideoOutputBase.h"
#include "../Video/VideoStream.h"
#include "../ZuazoBase.h"

struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;

namespace Zuazo::Sources{
	class FFmpeg;
}

namespace Zuazo::Stream{
	template<typename T>
	class FFmpegSourcePad :
		public Source<T>
	{
		friend Sources::FFmpeg;
	public:
		std::shared_ptr<const T> get() const override;
	private:
		FFmpegSourcePad(const Sources::FFmpeg& owner);

		const Sources::FFmpeg& m_owner;
	};
}

namespace Zuazo::Sources{

class FFmpeg :
		public Video::VideoOutputBase,
		public Timing::NonLinear<Timing::UPDATE_ORDER_FF_DEC>,
		public ZuazoBase
{
	friend Stream::FFmpegSourcePad<Video::VideoElement>;
public:
	FFmpeg(const std::string& dir);
	FFmpeg(const FFmpeg& other);
	~FFmpeg();

	void							open() override;
	void							close() override;
protected:
	void							update() const override;
private:
	std::string 					m_file;

	bool							m_exit;
	std::thread						m_decodingThread;
	mutable std::mutex				m_decodeMutex;
	mutable std::condition_variable	m_decodeCondition;

	AVFormatContext*				m_formatCtx;
	AVCodecContext*					m_codecCtx;
	AVCodec*						m_codec;
	int								m_videoStream;

	Stream::FFmpegSourcePad<Video::VideoElement> m_videoSourcePad;

	mutable int64_t					m_currTs;

	static constexpr AVRational		ZUAZO_TIME_BASE_Q={1, Utils::TimeInterval::TIME_UNITS_PER_SECOND};

	void							decodingFunc();
};

}

template<typename T>
inline std::shared_ptr<const T> Zuazo::Stream::FFmpegSourcePad<T>::get() const{
	std::lock_guard<std::mutex> lock(m_owner.m_decodeMutex); //Wait until decoding has finished
	return Source<T>::get();
}

template<typename T>
inline Zuazo::Stream::FFmpegSourcePad<T>::FFmpegSourcePad(const Sources::FFmpeg& owner) :
	m_owner(owner)
{
}

