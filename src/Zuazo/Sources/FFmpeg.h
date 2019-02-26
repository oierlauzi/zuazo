#pragma once

#include <bits/stdint-intn.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "../Timing/TimeInterval.h"
#include "../Utils/PixelTypes.h"
#include "VideoClipBase.h"

struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;

namespace Zuazo {
namespace Graphics {
class Frame;
} /* namespace Graphics */
} /* namespace Zuazo */

namespace Zuazo::Sources{

class FFmpeg : public VideoClipBase{
public:
	FFmpeg(const std::string& dir);
	FFmpeg(const FFmpeg& other);
	~FFmpeg();

	void							open() override;
	void							close() override;

	virtual std::shared_ptr<const Packet> get() const override;
protected:
	void							update() const;
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

	static constexpr AVRational		ZUAZO_TIME_BASE_Q={1, Timing::TimeInterval::TIME_UNITS_PER_SECOND};

	void							decodingFunc();

	static AVPixelFormat			findBestMatch(AVPixelFormat fmt);
	static Utils::PixelTypes		toPixelTypes(AVPixelFormat fmt);
};

}
