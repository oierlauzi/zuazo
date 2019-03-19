#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "../Utils/Codec.h"
#include "../Utils/PixelFormat.h"
#include "../Utils/VideoMode.h"
#include "../Video/VideoOutputBase.h"
#include "../Video/VideoStream.h"
#include "../ZuazoBase.h"

namespace Zuazo {
namespace Utils {
struct Resolution;
} /* namespace Utils */
} /* namespace Zuazo */

struct v4l2_buffer;

namespace Zuazo::Sources{

class V4L2:
		public Video::TVideoOutputBase<Video::AsyncVideoSourcePad<V4L2>>,
		public ZuazoBase
{
public:
	Video::AsyncVideoSourcePad<V4L2> videoOut;

	V4L2(u_int32_t dev);
	V4L2(const std::string& devName);
	V4L2(const V4L2& other)=delete;
	~V4L2();

	void						open() override;
	void						close() override;

	SUPPORTS_SETTING_VIDEOMODE
	void						setVideoMode(const Utils::VideoMode& mode) override;

	SUPPORTS_LISTING_VIDEOMODES
	std::set<Utils::VideoMode>	getSupportedVideoModes() const override;
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
}
