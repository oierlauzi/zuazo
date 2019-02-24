#include "../../Media/Sources/V4L2.h"

#include <asm-generic/errno-base.h>
#include <fcntl.h>
#include <jpeglib.h>
#include <linux/videodev2.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <utility>

#include "../../Utils/ImageAttributes.h"
#include "../../Utils/ImageBuffer.h"
#include "../../Utils/PixelTypes.h"
#include "../../Graphics/Frame.h"


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

using namespace Zuazo::Media::Sources;

V4L2::V4L2(u_int32_t dev) : V4L2("/dev/video" + std::to_string(dev))
{
}

V4L2::V4L2(const std::string& devName){
	m_dev=-1;
	m_currVidMode=m_vidModes.end();

	//Check if the video device directory exists
	struct stat st;
	if (stat(devName.c_str(), &st) == -1)
		return;

	//Check if the directory is a "special" file, FI a tty, V4L2 device...
	//Obviously, it needs to be a special device, as we are looking for a V4L2 device
	if (!S_ISCHR(st.st_mode))
		return;

	//Try to open the device
	m_dev=::open(devName.c_str(), O_RDWR | O_NONBLOCK, 0);

	//Check if file has been successfully opened
	if(m_dev < 0)
		return;

	//Check if it is a video capture device
	v4l2_capability m_cap={0};
	if (-1 == xioctl(m_dev, VIDIOC_QUERYCAP, &m_cap))
		return;

	if (!(m_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
		return;

	//Get all the available resolutions
	std::set<Utils::Resolution> resolutions;
	struct v4l2_frmsizeenum v4lsize = {0};
	v4lsize.pixel_format=V4L2_PIX_FMT_MJPEG; //The only format supported by now

	//Query all the available sizes
	while (0 == xioctl(m_dev, VIDIOC_ENUM_FRAMESIZES, &v4lsize)){
		if(v4lsize.type==V4L2_FRMSIZE_TYPE_DISCRETE){
			//Only 1 res given. Insert it on the set
			resolutions.insert(Utils::Resolution{
				v4lsize.discrete.width,
				v4lsize.discrete.height
			});
		}else{
			//More than 1 res is available. Insert them on the set
			for(u_int32_t i=v4lsize.stepwise.min_width; i<=v4lsize.stepwise.max_width; i+=v4lsize.stepwise.step_width){
				for(u_int32_t j=v4lsize.stepwise.min_height; j<=v4lsize.stepwise.max_height; j+=v4lsize.stepwise.step_height){
					resolutions.insert(Utils::Resolution{
						i,
						j
					});
				}
			}

			break; //No need to query again
		}
		//Query next resolution
		v4lsize.index++;//
	}

	//Get the avalible frame intervals for each resolution
	for(const Utils::Resolution& res : resolutions){
		v4l2_frmivalenum v4lfrt = {0};
		v4lfrt.pixel_format=V4L2_PIX_FMT_MJPEG;
		v4lfrt.width=res.width;
		v4lfrt.height=res.height;

		while (0 == xioctl(m_dev, VIDIOC_ENUM_FRAMEINTERVALS, &v4lfrt)){
			if(v4lfrt.type==V4L2_FRMSIZE_TYPE_DISCRETE){
				//Only 1 frame-rate given
				m_vidModes.insert(VideoMode(res, Utils::Rational(
						v4lfrt.discrete.numerator,
						v4lfrt.discrete.denominator
				)));
			}else{
				//A interval has been given. Calculate all the possible frame-rates and continue

				for(v4l2_fract interval=v4lfrt.stepwise.min;
					v4lfrt.stepwise.min.numerator <= v4lfrt.stepwise.max.numerator;
					interval.numerator+=v4lfrt.stepwise.step.numerator){

						m_vidModes.insert(VideoMode(res, Utils::Rational(
									v4lfrt.discrete.numerator,
									v4lfrt.discrete.denominator
							)));
				}

				break; //No need to query again
			}

			//Query next frame-rate
			v4lfrt.index++;
		}
	}

	if(m_vidModes.size() == 0)
		return; //There are no video modes for this device

	//Set the default resolution (highest one)
	m_currVidMode=--m_vidModes.end();
	AsyncVideoSource::setInterval(m_currVidMode->interval);

	open();
}

V4L2::~V4L2(){
	close();

	if(m_dev>=0)
		::close(m_dev);

}


void V4L2::setVideoMode(const VideoMode& mode){
	close();
	m_currVidMode=m_vidModes.find(mode);

	if(m_currVidMode != m_vidModes.end()){
		//Video mode exists
		AsyncVideoSource::setInterval(m_currVidMode->interval);
		open();
	}
}



void V4L2::open(){
	if(m_currVidMode != m_vidModes.end()){
		//Set the new resolution
		struct v4l2_format fmt = {0};
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = m_currVidMode->resolution.width;
		fmt.fmt.pix.height = m_currVidMode->resolution.height;
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
		fmt.fmt.pix.field = V4L2_FIELD_NONE;

		if (-1 == xioctl(m_dev, VIDIOC_S_FMT, &fmt))
			return; //Error setting the resolution

		//Set the frame-rate
		struct v4l2_streamparm strm={0};
		strm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl(m_dev, VIDIOC_G_PARM, &strm))
			return;

		if(strm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME){
			strm.parm.capture.timeperframe.numerator = m_currVidMode->interval.num;
			strm.parm.capture.timeperframe.denominator = m_currVidMode->interval.den;

			if (-1 == xioctl(m_dev, VIDIOC_S_PARM, &strm))
				return; //Error setting the frame interval
		}// else device does not support setting the framerate

		//Request buffer number
		struct v4l2_requestbuffers req={0};
		req.count = 4;
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_MMAP;

		if (-1 == xioctl(m_dev, VIDIOC_REQBUFS, &req))
			return; //Error requesting buffers

		if (req.count < 2)
			return; //We need at least 2 buffers (4 requested)

		//MMAP the buffers
		for (u_int32_t i=0; i<req.count; i++) {
			struct v4l2_buffer buf={0};
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if(-1 == xioctl(m_dev, VIDIOC_QUERYBUF, &buf))
				return; //Error querying buffers

			//Create a buffer structure and map it
			Buffer buffer(buf, m_dev);

			if(buffer.buffer == MAP_FAILED)
				return; //Failed mapping the

			//Insert the buffer
			m_buffers.emplace_back(std::move(buffer));
		}

		//Initialize Buffers
		for (u_int32_t i=0; i<m_buffers.size(); i++){
			struct v4l2_buffer buf={0};
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if (-1 == xioctl(m_dev, VIDIOC_QBUF, &buf))
				return;
		}

		//Start the streaming
		enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(m_dev, VIDIOC_STREAMON, &type))
			return;

		m_threadExit=false;
		m_capturingThread=std::thread(&V4L2::capturingThread, this);

		AsyncVideoSource::open();
	}
}

void V4L2::close(){
	AsyncVideoSource::close();

	//End capture theread
	m_threadExit=true;
	if(m_capturingThread.joinable())
		m_capturingThread.join();

	//Stop streaming
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(m_dev, VIDIOC_STREAMOFF, &type))
		return;

    //Release buffers
    m_buffers.erase(m_buffers.begin(), m_buffers.end());

	struct v4l2_requestbuffers req={0};
	req.count = 0;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(m_dev, VIDIOC_REQBUFS, &req))
		return;
}

void V4L2::capturingThread(){
    //Initialize jpeg decompressor
	jpeg_decompress_struct m_decmp={0};
	jpeg_error_mgr errMgr={0};
	m_decmp.err=jpeg_std_error(&errMgr);
    jpeg_create_decompress(&m_decmp);

	Utils::ImageAttributes imgAtt{
		Utils::Resolution(m_currVidMode->resolution),
		Utils::PixelTypes::RGB
	};

    Utils::ImageBuffer imgBuffer(imgAtt);

    //Main loop
    while(!m_threadExit){
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(m_dev, &fds);

		//Time out for select
		struct timeval tv = {0};
		tv.tv_sec = 2;

		//Wait for a frame
		if(0 >= select(m_dev+1, &fds, NULL, NULL, &tv))
			continue; //Timeout

		//Request the buffer
		v4l2_buffer buf={0};
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if(-1 == xioctl(m_dev, VIDIOC_DQBUF, &buf))
			continue; //Error requesting buffer


		if(buf.index>=m_buffers.size()){
			//Invalid index given
			xioctl(m_dev, VIDIOC_QBUF, &buf);
			continue;
		}

		//Set decoder's source
		jpeg_mem_src(&m_decmp, m_buffers[buf.index].buffer, buf.bytesused);

		//Read the JPEG's header
		jpeg_read_header(&m_decmp, TRUE);

		//Set output format as RGB and decompress it
		m_decmp.out_color_space = JCS_EXT_RGB;
		jpeg_start_decompress(&m_decmp);

		size_t stride=m_decmp.output_width * m_decmp.output_components;
		u_int8_t * rowPtr[1];
		rowPtr[0]=imgBuffer.data;

		//Read image line-by-line
		while (m_decmp.output_scanline < m_decmp.output_height) {
			jpeg_read_scanlines(&m_decmp, rowPtr, 1);
			rowPtr[0]+=stride;
		}

		jpeg_finish_decompress(&m_decmp);

		//Buffer no longer needed
		if(-1 == xioctl(m_dev, VIDIOC_QBUF, &buf))
			continue;

		//Upload image to the source
		std::unique_ptr<const Graphics::Frame> newFrame;

		{
			Graphics::UniqueContext ctx(Graphics::Context::getAvalibleCtx());

			newFrame=std::unique_ptr<Graphics::Frame>(
					new Graphics::Frame(imgBuffer)
			);
		}

		AsyncVideoSource::push(std::move(newFrame));
    }

    jpeg_destroy_decompress(&m_decmp);
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
