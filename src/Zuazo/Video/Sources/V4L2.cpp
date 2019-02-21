#include "V4L2.h"

#include <asm-generic/errno-base.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <utility>


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

using namespace Zuazo::Video::Sources;

V4L2::V4L2(u_int32_t dev) : V4L2("/dev/video" + std::to_string(dev))
{
}

V4L2::V4L2(const std::string& devName){
	m_dev=-1;

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
	if(m_dev==-1)
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

	//Set the default resolution
	m_currVidMode=--m_vidModes.end();

	open();
}

V4L2::~V4L2(){
	close();

	if(m_dev>=0)
		::close(m_dev);

}




void V4L2::open(){

}

void V4L2::close(){

}




void V4L2::capturingThread(){

}
