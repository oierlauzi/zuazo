#pragma once

#include <sys/types.h>
#include <cstring>

extern "C"{
	#include <libavutil/imgutils.h>
	#include <libavutil/mem.h>
}

#include "ImageAttributes.h"
#include "PixelFormat.h"
#include "Resolution.h"

namespace Zuazo::Utils{

struct ImageBuffer{
	ImageAttributes		att;
	u_int8_t*			data[4];

	ImageBuffer(){
		for(u_int32_t i=0; i<4; i++)
			data[i]=nullptr;
	}

	ImageBuffer(const ImageAttributes& att) :
		att(att)
	{
		allocate();
	}

	ImageBuffer(const ImageAttributes& att, const u_int8_t* data) :
		att(att)
	{
		fillData(data);
	}


	ImageBuffer(const ImageAttributes& att, u_int8_t*  const data[4]) :
		att(att)
	{
		for(u_int32_t i=0; i<4; i++){
			this->data[i]=data[i];
		}
	}

	ImageBuffer(const ImageBuffer& other) :
		ImageBuffer(other.att)
	{
		memcpy(data[0], other.data[0], att.getSize());
	}

	ImageBuffer(ImageBuffer&& other) :
		att(other.att)
	{
		for(u_int32_t i=0; i<4; i++){
			data[i]=other.data[i];
			other.data[i]=nullptr;
		}

		wasAllocated=other.wasAllocated;
		other.wasAllocated=false;
	}

	~ImageBuffer(){
		if(wasAllocated)
			deallocate();
	}

	void allocate(){
		int linesizes[4];
		att.getStrides(linesizes);

		av_image_alloc(
				data,
				linesizes,
				att.res.width,
				att.res.height,
				att.pixFmt.toAVPixelFormat(),
				1
		);
		wasAllocated=true;
	}

	void fillData(const u_int8_t* src){
		int linesizes[4];
		att.getStrides(linesizes);

	 	av_image_fill_arrays(
	 			data,
				linesizes,
				src,
				att.pixFmt.toAVPixelFormat(),
				att.res.width,
				att.res.height,
				1
		);
	}

	void deallocate(){
		av_freep(data[0]);
		wasAllocated=false;
	}

private:
	bool wasAllocated=false;
};

}
