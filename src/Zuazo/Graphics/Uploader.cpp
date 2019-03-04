#include "Uploader.h"

#include <sys/types.h>
#include <algorithm>
#include <cstring>

#include "../Utils/Resolution.h"
#include "Frame.h"
#include "GL/Buffer.h"

extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}

#include "../Utils/PixelFormats.h"

using namespace Zuazo::Graphics;

const AVPixelFormat Uploader::s_compatiblePixelFormats[]={
		AV_PIX_FMT_RGB24,
		AV_PIX_FMT_BGR24,
		AV_PIX_FMT_RGBA,
		AV_PIX_FMT_BGRA,
		AV_PIX_FMT_NONE, //List needs to be terminated with AV_PIX_FMT_NONE
};

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

	std::unique_ptr<GL::PixelUnpackBuffer> pbo=Frame::createPixelUnpackBuffer(m_destSize);
	pbo->allocate(m_destSize, GL::PixelUnpackBuffer::BufferUsage::StreamDraw);

	{
		GL::PixelUnpackBuffer::BufferMapping pboData(
				*pbo,
				GL::PixelUnpackBuffer::BufferMapping::Access::Write
		);

		u_int8_t* pboDataPtr=(u_int8_t*)pboData.get();
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
	}

	/*TESTING*/
	/*
	u_int8_t* testingData=(u_int8_t*)malloc(m_destSize);
	pbo->read(testingData);

	FILE * testOutput=fopen("Prueba.rgba", "w+");
	fwrite(testingData, m_destSize, 1, testOutput);
	fclose(testOutput);

	free(testingData);
	*/

	return std::unique_ptr<Frame>(new Frame(
			std::move(pbo),
			m_destAttributes
	));
}

GL::ImageAttributes	Uploader::getBestMatch(const Utils::ImageAttributes& other){
	int loss; //Not used
	Utils::PixelFormat fmt(avcodec_find_best_pix_fmt_of_list(
			s_compatiblePixelFormats,
			other.pixFmt.toAVPixelFormat(),
			other.pixFmt.hasAlpha(),
			&loss
	));

	return GL::ImageAttributes(
			other.res,
			GL::PixelFormat(fmt)
	);
}
