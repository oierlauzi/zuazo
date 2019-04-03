#include <Graphics/Uploader.h>

#include <Graphics/Frame.h>
#include <Graphics/GL/Buffer.h>
#include <Graphics/GL/UniqueBinding.h>
#include <Graphics/PixelFormat.h>
#include <Utils/Resolution.h>
#include <Graphics/Context.h>

#include <glad/glad.h>
#include <sys/types.h>
#include <algorithm>
#include <cstring>
#include "../../include/Utils/PixelFormat.h"

extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}
using namespace Zuazo::Graphics;

Uploader::Uploader(){
	memset(m_srcStrides, 0, sizeof(m_srcStrides));
	memset(m_destStrides, 0, sizeof(m_destStrides));
	m_destSize=0;
	m_swsContext=nullptr;
}

Uploader::Uploader(const Uploader& other){
	memcpy(m_srcStrides, other.m_srcStrides, sizeof(m_srcStrides));
	memcpy(m_destStrides, other.m_destStrides, sizeof(m_destStrides));
	m_destSize=other.m_destSize;

	m_swsContext=sws_getContext(
			m_srcAttributes.res.width,
			m_srcAttributes.res.height,
			m_srcAttributes.pixFmt.toAVPixelFormat(),
			m_destAttributes.res.width,
			m_destAttributes.res.height,
			m_destAttributes.pixFmt.toAVPixelFormat(),
			SWS_POINT,
			nullptr,
			nullptr,
			nullptr
	);
}

Uploader::~Uploader(){
	sws_freeContext(m_swsContext); //Accepts nullptr
}

std::unique_ptr<const Frame> Uploader::getFrame(const Utils::ImageBuffer& buf) const{
	if(buf.att != m_srcAttributes){
		//Attributes have changed
		m_srcAttributes=buf.att;
		m_destAttributes=getBestMatch(buf.att);

		m_srcAttributes.getStrides(m_srcStrides);
		m_destStrides[0]=m_destAttributes.getStride(); // [1], [2] and [3] should be set to 0 by constructor
		m_destSize=m_destAttributes.getSize();

		//Set up the sws context
		sws_freeContext(m_swsContext); //Accepts nullptr
		m_swsContext=sws_getContext(
				m_srcAttributes.res.width,
				m_srcAttributes.res.height,
				m_srcAttributes.pixFmt.toAVPixelFormat(),
				m_destAttributes.res.width,
				m_destAttributes.res.height,
				m_destAttributes.pixFmt.toAVPixelFormat(),
				SWS_POINT,
				nullptr,
				nullptr,
				nullptr
		);
	}

	//Get a Pixel Unpack Buffer
	std::unique_ptr<GL::PixelUnpackBuffer> pbo=Frame::createPixelUnpackBuffer(m_destSize);
	if(!pbo){
		UniqueContext mainCtx(Context::getMainCtx());
		pbo=std::unique_ptr<GL::PixelUnpackBuffer> (new GL::PixelUnpackBuffer);
	}

	UniqueContext ctx;

	//Bind the PBO
	GL::UniqueBinding<GL::PixelUnpackBuffer> bufferBinding(*pbo);

	//Reallocate data on the PBO
	GL::PixelUnpackBuffer::bufferData(
			m_destSize,
			GL::PixelUnpackBuffer::Usage::StreamDraw
	);

	//Map the PBO's data into memory
	u_int8_t* pboDataPtr=(u_int8_t*)glMapBuffer(pbo->GLType, GL_WRITE_ONLY); //Could be done using the implemented OOP mapping
	if(pboDataPtr){
		//Succesfuly maped PBO
		u_int8_t* planes[4]={
				pboDataPtr,
				nullptr,
				nullptr,
				nullptr
		};

		//Do the conversion
		sws_scale(
				m_swsContext,
				buf.data,
				m_srcStrides,
				0,
				m_srcAttributes.res.height,
				planes,
				m_destStrides
		);

		glUnmapBuffer(pbo->GLType); //Unmap the PBO
	}

	return std::unique_ptr<Frame>(new Frame(
			std::move(pbo),
			m_destAttributes
	));
}

ImageAttributes	Uploader::getBestMatch(const Utils::ImageAttributes& other){
	int loss=0; //Needs to be set to 0, otherwise, erratic conversions will occur.
	Utils::PixelFormat fmt(avcodec_find_best_pix_fmt_of_list(
			(const AVPixelFormat*)compatiblePixelFormats,
			other.pixFmt.toAVPixelFormat(),
			other.pixFmt.hasAlpha(),
			&loss
	));

	return ImageAttributes(
			other.res,
			PixelFormat(fmt)
	);
}
