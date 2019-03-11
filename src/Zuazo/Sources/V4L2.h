#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "../Graphics/Frame.h"
#include "../Stream/AsyncSource.h"
#include "../Utils/Codec.h"
#include "../Utils/PixelFormat.h"
#include "../Utils/Rational.h"
#include "../Utils/Resolution.h"
#include "../Utils/TimeInterval.h"
#include "../Utils/VideoMode.h"
#include "../Video.h"

struct v4l2_buffer;

namespace Zuazo::Sources{

class V4L2:
		public Stream::AsyncSource<Graphics::Frame>,
		public ResizeableVideoBase
{
public:
	V4L2(u_int32_t dev);
	V4L2(const std::string& devName);
	V4L2(const V4L2& other)=delete;
	~V4L2();

	void						open() override;
	void						close() override;

	void						setRate(const Utils::Rational& rate) override;
	void						setInterval(const Utils::TimeInterval& interval) override;
	void						setRes(const Utils::Resolution& res) override;
	void						setVideoMode(const Utils::VideoMode& mode);

	//Utils::Resolution			getRes() const override;
	Utils::VideoMode			getCurrentVideoMode() const;
	std::set<Utils::VideoMode>	getVideoModes() const;
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

	struct PixelFormatMap{
		Utils::PixelFormat			pixFmt;
		Utils::Codec				codec;
		uint32_t					v4l2PixFmt;
	};

	std::string					m_name;
	int							m_dev;
	Utils::VideoMode			m_currVidMode;

	std::vector<Buffer>			m_buffers;

	std::thread					m_capturingThread;
	bool						m_threadExit;

	void						reqBuffer(v4l2_buffer* buf);
	void						freeBuffer(v4l2_buffer* buf);
	void 						compressedCapturingThread();
	void 						rawCapturingThread();

	static const PixelFormatMap	s_pixFmtConverisons[];

	static Utils::Codec			getCodec(uint32_t v4l2Fmt);
	static Utils::PixelFormat	getPixFmt(uint32_t v4l2Fmt);
	static uint32_t				getV4L2PixFmt(const Utils::Codec& codec, const Utils::PixelFormat& fmt);
};

inline void V4L2::setRate(const Utils::Rational& rate){ //TODO
	//TODO
}

inline void V4L2::setInterval(const Utils::TimeInterval& interval){
	//TODO
}

inline void V4L2::setRes(const Utils::Resolution& res){
	//TODO
}

inline Utils::VideoMode	V4L2::getCurrentVideoMode() const{
	return m_currVidMode;
}

/*inline Utils::Resolution V4L2::getRes() const{
	if(m_currVidMode != m_vidModes.end())
		return m_currVidMode->resolution;
	else
		return Utils::Resolution();
}*/
}
