#include <Sources/V4L2.h>

#include <Graphics/Frame.h>
#include <Graphics/Context.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/Uploader.h>
#include <Utils/ImageAttributes.h>
#include <Utils/ImageBuffer.h>
#include <Utils/Codec.h>
#include <Utils/PixelFormat.h>
#include <Utils/Rational.h>
#include <Utils/Resolution.h>
#include <Utils/VideoMode.h>

#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <map>
#include <memory>
#include <utility>

extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libavutil/frame.h>
	#include <libavutil/avutil.h>
	#include <libavutil/mem.h>
}

/*
 * This code is based on the following tutorials
 * https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/capture.c.html
 * https://jayrambhia.com/blog/capture-v4l2
 */


static int xioctl(int fd, int req, void *arg){
	int result;

    do{
    	result = ioctl ((int)fd, req, arg);
    }while (-1 == result && EINTR == errno);

    return result;
}

using namespace Zuazo::Sources;

const V4L2::PixelFormatMap V4L2::s_pixFmtConverisons[]={
    { Utils::PixelFormats::YUV420P,		Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_YUV420  },
    { Utils::PixelFormats::YUV420P, 	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_YVU420  },
    { Utils::PixelFormats::YUV422P, 	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_YUV422P },
    { Utils::PixelFormats::YUYV422, 	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_YUYV    },
    { Utils::PixelFormats::UYVY422, 	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_UYVY    },
    { Utils::PixelFormats::YUV411P, 	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_YUV411P },
    { Utils::PixelFormats::YUV410P, 	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_YUV410  },
    { Utils::PixelFormats::YUV410P, 	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_YVU410  },
    { Utils::PixelFormats::RGB555LE,	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_RGB555  },
    { Utils::PixelFormats::RGB555BE,	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_RGB555X },
    { Utils::PixelFormats::RGB565LE,	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_RGB565  },
    { Utils::PixelFormats::RGB565BE,	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_RGB565X },
    { Utils::PixelFormats::BGR24,   	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_BGR24   },
    { Utils::PixelFormats::RGB24,   	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_RGB24   },
#ifdef V4L2_PIX_FMT_XBGR32
    { Utils::PixelFormats::BGRX,    	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_XBGR32  },
    { Utils::PixelFormats::XRGB,    	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_XRGB32  },
    { Utils::PixelFormats::BGRA,    	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_ABGR32  },
    { Utils::PixelFormats::ARGB,    	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_ARGB32  },
#endif
    { Utils::PixelFormats::BGRX,    	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_BGR32   },
    { Utils::PixelFormats::XRGB,    	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_RGB32   },
    { Utils::PixelFormats::GRAY8,   	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_GREY    },
#ifdef V4L2_PIX_FMT_Y16
    { Utils::PixelFormats::GRAY16LE,	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_Y16     },
#endif
    { Utils::PixelFormats::NV12,    	Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_NV12    },
    { Utils::PixelFormats::NONE,   		Utils::Codecs::MJPEG,    V4L2_PIX_FMT_MJPEG   },
    { Utils::PixelFormats::NONE,		Utils::Codecs::MJPEG,    V4L2_PIX_FMT_JPEG    },
#ifdef V4L2_PIX_FMT_H264
    { Utils::PixelFormats::NONE,   		Utils::Codecs::H264,     V4L2_PIX_FMT_H264    },
#endif
#ifdef V4L2_PIX_FMT_MPEG4
    { Utils::PixelFormats::NONE,    	Utils::Codecs::MPEG4,    V4L2_PIX_FMT_MPEG4   },
#endif
#ifdef V4L2_PIX_FMT_CPIA1
    { Utils::PixelFormats::NONE,    	Utils::Codecs::CPIA,     V4L2_PIX_FMT_CPIA1   },
#endif
#ifdef V4L2_PIX_FMT_SRGGB8
    { Utils::PixelFormats::BAYER_BGGR8, Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_SBGGR8 },
    { Utils::PixelFormats::BAYER_GBRG8, Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_SGBRG8 },
    { Utils::PixelFormats::BAYER_GRBG8, Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_SGRBG8 },
    { Utils::PixelFormats::BAYER_RGGB8, Utils::Codecs::RAWVIDEO, V4L2_PIX_FMT_SRGGB8 },
#endif
    { Utils::PixelFormats::NONE,    	Utils::Codecs::NONE,     0                    },
};


V4L2::V4L2(u_int32_t dev) :
		V4L2("/dev/video" + std::to_string(dev))
{
}

V4L2::V4L2(const std::string& devName) :
		Utils::FileBase(devName)
{
	open();
}

V4L2::~V4L2(){
	close();
}


void V4L2::setVideoMode(const Utils::VideoMode& mode){
	close();
	m_videoMode=mode;
	open();
}

std::set<Zuazo::Utils::VideoMode> V4L2::getSupportedVideoModes() const{
	std::set<Utils::VideoMode> videoModes;

	if(m_dev>0){
		//Get the available pixel formats
		std::set<uint32_t> pixelFormats;
		struct v4l2_fmtdesc v4lpixfmt = {0};
		v4lpixfmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
		while (0 == xioctl(m_dev, VIDIOC_ENUM_FMT, &v4lpixfmt)){
			pixelFormats.insert(v4lpixfmt.pixelformat);
			//Query next pixel format
			v4lpixfmt.index++;
		}

		struct V4L2ImageFmt{
			uint32_t 			pixFmt;
			Utils::Resolution	res;

			int operator<(const V4L2ImageFmt& other) const{
				if(res < other.res)
					return true;
				else if(res == other.res){
					if(pixFmt < other.pixFmt)
						return true;
					else
						return false;
				}else
					return false;
			}
		};

		//Get all the available resolutions
		std::set<V4L2ImageFmt> imgFmts;
		struct v4l2_frmsizeenum v4lsize = {0};

		for(auto& pixFmt : pixelFormats){
			v4lsize.pixel_format=pixFmt;

			//Query all the available sizes
			while (0 == xioctl(m_dev, VIDIOC_ENUM_FRAMESIZES, &v4lsize)){
				if(v4lsize.type==V4L2_FRMSIZE_TYPE_DISCRETE){
					//Only 1 res given. Insert it on the set
					imgFmts.insert(V4L2ImageFmt{
						.pixFmt	=v4lsize.pixel_format,
						.res	=Utils::Resolution(v4lsize.discrete.width, v4lsize.discrete.height)
					});
				}else{
					//More than 1 res is available. Insert them on the set
					for(u_int32_t i=v4lsize.stepwise.min_width; i<=v4lsize.stepwise.max_width; i+=v4lsize.stepwise.step_width){
						for(u_int32_t j=v4lsize.stepwise.min_height; j<=v4lsize.stepwise.max_height; j+=v4lsize.stepwise.step_height){
							imgFmts.insert(V4L2ImageFmt{
								.pixFmt	=v4lsize.pixel_format,
								.res	=Utils::Resolution(i, j)
							});
						}
					}

					break; //No need to query again
				}
				//Query next resolution
				v4lsize.index++;
			}
		}

		//Get the avalible frame intervals for each resolution
		for(auto& imgFmt : imgFmts){
			v4l2_frmivalenum v4lfrt = {0};
			v4lfrt.pixel_format=imgFmt.pixFmt;
			v4lfrt.width=imgFmt.res.width;
			v4lfrt.height=imgFmt.res.height;

			while (0 == xioctl(m_dev, VIDIOC_ENUM_FRAMEINTERVALS, &v4lfrt)){
				if(v4lfrt.type==V4L2_FRMSIZE_TYPE_DISCRETE)
				{
					//Only 1 frame-rate given
					videoModes.insert(Utils::VideoMode{
						.pixFmt=getPixFmt(imgFmt.pixFmt),
						.res=imgFmt.res,
						.codec=getCodec(imgFmt.pixFmt),
						.frameRate=Utils::Rational(v4lfrt.discrete.denominator, v4lfrt.discrete.numerator)
					});
				}else{
					//A interval has been given. Calculate all the possible frame-rates and continue

					for(	v4l2_fract interval=v4lfrt.stepwise.min;
							v4lfrt.stepwise.min.numerator <= v4lfrt.stepwise.max.numerator;
							interval.numerator+=v4lfrt.stepwise.step.numerator)
					{
						videoModes.insert(Utils::VideoMode{
							.pixFmt=getPixFmt(imgFmt.pixFmt),
							.res=imgFmt.res,
							.codec=getCodec(imgFmt.pixFmt),
							.frameRate=Utils::Rational(interval.denominator, interval.numerator)
						});
					}

					break; //No need to query again
				}

				//Query next frame-rate
				v4lfrt.index++;
			}
		}
	}

	return videoModes;
}


void V4L2::open(){
	m_dev=-1;

	//Check if the video device directory exists
	struct stat st;
	if (stat(m_directory.c_str(), &st) == -1)
		return;

	//Check if the directory is a "special" file, FI a tty, V4L2 device...
	//It needs to be a special device, as we are looking for a V4L2 device
	if (!S_ISCHR(st.st_mode))
		return;

	//Try to open the device
	m_dev=::open(m_directory.c_str(), O_RDWR | O_NONBLOCK, 0);

	//Check if file has been successfully opened
	if(m_dev < 0)
		return;

	//Check if it is a video capture device
	v4l2_capability m_cap={0};
	if (-1 == xioctl(m_dev, VIDIOC_QUERYCAP, &m_cap)){
		close();
		return;
	}

	if (!(m_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
		close();
		return;
	}

	std::set<Utils::VideoMode> vidModes=getSupportedVideoModes();
	std::set<Utils::VideoMode>::iterator vidMode=vidModes.find(m_videoMode);
	if(vidMode==vidModes.end()){
		//Requested video mode does not exist
		//Set the highest video mode as current
		if(vidModes.begin() != vidModes.end()){
			m_videoMode=*(--vidModes.end());
		}else{
			//There are no video modes available
			close();
			return;
		}
	}

	//Set the new resolution
	struct v4l2_format fmt = {0};
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = m_videoMode.res.width;
	fmt.fmt.pix.height = m_videoMode.res.height;
	fmt.fmt.pix.pixelformat = getV4L2PixFmt(m_videoMode.codec, m_videoMode.pixFmt);
	fmt.fmt.pix.field = V4L2_FIELD_NONE;

	if (-1 == xioctl(m_dev, VIDIOC_S_FMT, &fmt)){
		//Error setting the resolution
		close();
		return;
	}

	//Set the frame-rate
	struct v4l2_streamparm strm={0};
	strm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(m_dev, VIDIOC_G_PARM, &strm)){
		close();
		return;
	}

	if(strm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME){
		strm.parm.capture.timeperframe.numerator = m_videoMode.frameRate.den;
		strm.parm.capture.timeperframe.denominator = m_videoMode.frameRate.num;

		if (-1 == xioctl(m_dev, VIDIOC_S_PARM, &strm)){
			//Error setting the frame interval
			close();
			return;
		}

		m_videoSourcePad.setRate(m_videoMode.frameRate);
	} else{
		//Device does not support setting the framerate
		m_videoSourcePad.setRate(Utils::Rational(
				strm.parm.capture.timeperframe.denominator,
				strm.parm.capture.timeperframe.numerator
		));
	}

	//Request buffer number
	struct v4l2_requestbuffers req={0};
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(m_dev, VIDIOC_REQBUFS, &req)){
		//Error requesting buffers
		close();
		return;
	}

	if (req.count < 2){
		//We need at least 2 buffers (4 requested)
		close();
		return;
	}

	//MMAP the buffers
	for (u_int32_t i=0; i<req.count; i++) {
		struct v4l2_buffer buf={0};
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if(-1 == xioctl(m_dev, VIDIOC_QUERYBUF, &buf)){
			//Error querying buffers
			close();
			return;
		}

		//Create a buffer structure and map it
		Buffer buffer(buf, m_dev);

		if(buffer.buffer == MAP_FAILED){
			//Failed mapping the buffer
			close();
			return;
		}
		//Insert the buffer
		m_buffers.emplace_back(std::move(buffer));
	}

	//Initialize Buffers
	for (u_int32_t i=0; i<m_buffers.size(); i++){
		struct v4l2_buffer buf={0};
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(m_dev, VIDIOC_QBUF, &buf)){
			close();
			return;
		}
	}

	//Start the streaming
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl(m_dev, VIDIOC_STREAMON, &type)){
		close();
		return;
	}

	m_threadExit=false;
	if(m_videoMode.codec == Utils::Codecs::RAWVIDEO)
		m_capturingThread=std::thread(&V4L2::rawCapturingThread, this);
	else
		m_capturingThread=std::thread(&V4L2::compressedCapturingThread, this);

	m_videoSourcePad.enable();
	ZuazoBase::open();
}

void V4L2::close(){
	if(m_dev>=0){
		//End capture theread
		m_threadExit=true;
		if(m_capturingThread.joinable())
			m_capturingThread.join();

		//Stop streaming
		enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		xioctl(m_dev, VIDIOC_STREAMOFF, &type);

		//Release buffers
		m_buffers.clear();

		struct v4l2_requestbuffers req={0};
		req.count = 0;
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_MMAP;

		xioctl(m_dev, VIDIOC_REQBUFS, &req);

		::close(m_dev);
		m_dev=-1;
	}

	m_videoSourcePad.disable();
	m_videoSourcePad.flushBuffer();
	m_videoSourcePad.reset();

	ZuazoBase::close();
}

void V4L2::reqBuffer(v4l2_buffer* buf){
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_dev, &fds);

	//Time out for select
	struct timeval tv = {0};
	tv.tv_sec = 2;

	//Wait for a frame
	if(0 >= select(m_dev+1, &fds, NULL, NULL, &tv))
		return; //Timeout

	//Request the buffer
	*buf={0};
	buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf->memory = V4L2_MEMORY_MMAP;

	if(-1 == xioctl(m_dev, VIDIOC_DQBUF, buf)){
		*buf={0};
		return; //Error requesting buffer
	}


	if(buf->index>=m_buffers.size()){
		//Invalid index given
		freeBuffer(buf);
		*buf={0};
		return;
	}
}

void V4L2::freeBuffer(v4l2_buffer* buf){
	//No need to check if it was successfully unmapped
	//As it has no remedy :-<
	xioctl(m_dev, VIDIOC_QBUF, buf);
}

void V4L2::compressedCapturingThread(){
	const AVCodec *codec=avcodec_find_decoder(m_videoMode.codec.toAVCodecID());
	if(!codec)
		return; //Something went horribly wrong

	//Allocate the context
	AVCodecContext* codecCtx=avcodec_alloc_context3(codec);
	if(!codecCtx){
		return; //Error
	}

	codecCtx->width=m_videoMode.res.width;
	codecCtx->height=m_videoMode.res.height;

	//Open the context
	if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
		avcodec_free_context(&codecCtx);
		return;
	}

	//Allocate space for the packet
	AVPacket *pkt=av_packet_alloc();
	if(!pkt){
		avcodec_free_context(&codecCtx);
		return;
	}

	AVFrame* decodedFrame=av_frame_alloc();
	if(!decodedFrame){
		avcodec_free_context(&codecCtx);
		avcodec_free_context(&codecCtx);
		return;
	}

	Graphics::Uploader uplo;
	v4l2_buffer buf;
	int ret;

	Utils::ImageBuffer decodedImgBuf(
			Utils::ImageAttributes(
					m_videoMode.res,
					m_videoMode.pixFmt
			), (u_int8_t*)nullptr
	);

    //Main loop
    while(!m_threadExit){
    	reqBuffer(&buf);

    	if(!buf.bytesused){
    		continue;
    	}

		//Create the packet with the given data
		u_int8_t* bufData=(u_int8_t*)av_malloc(buf.bytesused);
		memcpy(bufData, m_buffers[buf.index].buffer, buf.bytesused); //copy the data
		av_packet_from_data (pkt, bufData, buf.bytesused);

    	freeBuffer(&buf); //V4L2 buffer no longer needed

		//Try to decode the packet
		ret=avcodec_send_packet(codecCtx, pkt);
		av_packet_unref(pkt);
		if(ret<0){
			continue;
		}

		ret = avcodec_receive_frame(codecCtx, decodedFrame);
		if(ret<0){
			continue;
		}

		memcpy(decodedImgBuf.data, decodedFrame->data, sizeof(decodedImgBuf.data)); //Copy plane pointers

		if(decodedImgBuf.att.pixFmt == Utils::PixelFormats::NONE){
			decodedImgBuf.att.pixFmt=Utils::PixelFormat(codecCtx->pix_fmt);

			//Change deprecated formats
			if(decodedImgBuf.att.pixFmt == Utils::PixelFormats::YUVJ420P)
				decodedImgBuf.att.pixFmt = Utils::PixelFormats::YUV420P;
			else if(decodedImgBuf.att.pixFmt == Utils::PixelFormats::YUVJ422P)
				decodedImgBuf.att.pixFmt = Utils::PixelFormats::YUV422P;
			else if(decodedImgBuf.att.pixFmt == Utils::PixelFormats::YUVJ440P)
				decodedImgBuf.att.pixFmt = Utils::PixelFormats::YUV440P;
			else if(decodedImgBuf.att.pixFmt == Utils::PixelFormats::YUVJ444P)
				decodedImgBuf.att.pixFmt = Utils::PixelFormats::YUV444P;

			m_videoMode.pixFmt=decodedImgBuf.att.pixFmt;
		}

		m_videoSourcePad.push(
				uplo.getFrame(decodedImgBuf)
		);
    }

    //Free everything
    avcodec_free_context(&codecCtx);
    av_packet_free(&pkt);
    av_frame_free(&decodedFrame);
}

void V4L2::rawCapturingThread(){

	Graphics::Uploader uplo;
	v4l2_buffer buf;

	Utils::ImageBuffer decodedImgBuf(
			Utils::ImageAttributes(
					m_videoMode.res,
					m_videoMode.pixFmt
			), (u_int8_t*)nullptr
	);

    //Main loop
    while(!m_threadExit){
    	reqBuffer(&buf);

    	decodedImgBuf.fillData(m_buffers[buf.index].buffer);

		m_videoSourcePad.push(
				uplo.getFrame(decodedImgBuf)
		);

		freeBuffer(&buf); //V4L2 buffer no longer needed
    }
}

Zuazo::Utils::Codec	V4L2::getCodec(uint32_t v4l2Fmt){
	for(u_int32_t i=0; s_pixFmtConverisons[i].v4l2PixFmt; i++){
		if( s_pixFmtConverisons[i].v4l2PixFmt == v4l2Fmt){
			return s_pixFmtConverisons[i].codec; //Found
		}
	}

	return Utils::Codec(); //Not found
}

Zuazo::Utils::PixelFormat V4L2::getPixFmt(uint32_t v4l2Fmt){
	for(u_int32_t i=0; s_pixFmtConverisons[i].v4l2PixFmt; i++){
		if( s_pixFmtConverisons[i].v4l2PixFmt == v4l2Fmt){
			return s_pixFmtConverisons[i].pixFmt; //Found
		}
	}

	return Utils::PixelFormat(); //Not found
}

uint32_t V4L2::getV4L2PixFmt(const Utils::Codec& codec, const Utils::PixelFormat& fmt){
	for(u_int32_t i=0; s_pixFmtConverisons[i].v4l2PixFmt; i++){
		if(s_pixFmtConverisons[i].codec == codec && s_pixFmtConverisons[i].pixFmt == fmt){
			return s_pixFmtConverisons[i].v4l2PixFmt; //Found
		}
	}

	return 0; //Not found
}













V4L2::Buffer::Buffer(const v4l2_buffer& v4l2BufReq, int dev){
	bufSize=v4l2BufReq.length;
	buffer=(u_int8_t*)mmap(nullptr, v4l2BufReq.length, PROT_READ | PROT_WRITE, MAP_SHARED, dev, v4l2BufReq.m.offset);
}

V4L2::Buffer::Buffer(Buffer&& other){
	*this=std::move(other);
}

V4L2::Buffer::~Buffer(){
	if(buffer != MAP_FAILED && buffer)
		munmap(buffer, bufSize);
}

V4L2::Buffer& V4L2::Buffer::operator=(Buffer&& other){
	bufSize=other.bufSize;
	buffer=other.buffer;

	other.bufSize=0;
	other.buffer=nullptr;

	return *this;
}
