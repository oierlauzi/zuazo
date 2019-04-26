#pragma once

#include "../NonLinearUpdate.h"
#include "../Stream/Source.h"
#include "../Utils/FileBase.h"
#include "../Graphics/Context.h"
#include "../Video/VideoSourceBase.h"
#include "../Video/VideoStream.h"
#include "../ZuazoBase.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

extern "C"{
	#include <libavutil/rational.h>
}


struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;

namespace Zuazo::Sources{

class FFmpeg :
		public Video::VideoSourceBase,
		public Utils::FileBase,
		public NonLinearUpdate,
		public ZuazoBase
{
public:
	FFmpeg(const std::string& dir);
	FFmpeg(const FFmpeg& other);
	~FFmpeg();

	SUPPORTS_GETTING_PIXELFORMAT
	SUPPORTS_GETTING_RESOLUTION
	SUPPORTS_GETTING_CODEC
	SUPPORTS_GETTING_FRAMERATE
	SUPPORTS_GETTING_PROGRESSIVE
	SUPPORTS_GETTING_VIDEOMODE

	void							open() override;
	void							close() override;

	void							update() const override;
private:
	class SourcePad :  public Stream::SourcePad<Video::VideoElement>{
	public:
		SourcePad()=default;
    	SourcePad(const SourcePad& other)=default;
		SourcePad(SourcePad&& other)=default;
    	virtual ~SourcePad()=default;

    	std::shared_ptr<const Video::VideoElement> get() const override;

    	mutable std::mutex 				m_decodeMutex;
	};

	bool							m_exit;
	std::thread						m_decodingThread;
	mutable std::condition_variable	m_decodeCondition;

	AVFormatContext*				m_formatCtx;
	AVCodecContext*					m_codecCtx;
	AVCodec*						m_codec;
	int								m_videoStream;

	SourcePad 						m_videoSourcePad;

	mutable int64_t					m_currTs;

	static constexpr AVRational		ZUAZO_TIME_BASE_Q={Timing::duationPeriod.num, Timing::duationPeriod.den};

	void							decodingFunc();
};

}

