#pragma once

#include <bits/stdint-intn.h>
#include <libavutil/rational.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "../Stream/Source.h"
#include "../Timing/NonLinear.h"
#include "../Utils/TimeInterval.h"
#include "../Video/VideoStream.h"
#include "../Video.h"
#include "../ZuazoBase.h"

struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;

namespace Zuazo::Sources{

class FFmpeg :
		public VideoBase,
		public Timing::NonLinear<Timing::UPDATE_ORDER_FF_DEC>,
		public ZuazoBase
{
public:
	template<typename T>
	class FFmpegPad :
		public Stream::Source<T>
	{
		friend FFmpeg;
	public:
		std::shared_ptr<const T> get() const override;
	private:
		FFmpegPad(const FFmpeg& owner);

		const FFmpeg& m_owner;
	};

	FFmpegPad<Video::VideoElement>	videoOut;

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


	mutable int64_t					m_currTs;

	static constexpr AVRational		ZUAZO_TIME_BASE_Q={1, Utils::TimeInterval::TIME_UNITS_PER_SECOND};

	void							decodingFunc();
};

template<typename T>
std::shared_ptr<const T> FFmpeg::FFmpegPad<T>::get() const{
	std::lock_guard<std::mutex> lock(m_owner.m_decodeMutex); //Wait until decoding has finished
	return Stream::Source<T>::get();
}

template<typename T>
FFmpeg::FFmpegPad<T>::FFmpegPad(const FFmpeg& owner) :
	m_owner(owner)
{
}

}
