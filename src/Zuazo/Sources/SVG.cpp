#include "SVG.h"

#include <stdlib.h>
#include <algorithm>
#include <deque>
#include <memory>

#include "../Graphics/Context.h"
#include "../Graphics/GL/ImageBuffer.h"
#include "../Graphics/GL/Texture.h"

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvg/nanosvg.h>
#include <nanosvg/nanosvgrast.h>

using namespace Zuazo::Sources;

SVG::SVG(const std::string& dir, float dpi) :
	m_file(dir),
	m_dpi(dpi)
{
	open();
}

SVG::SVG(const SVG& other) :
		SVG(other.m_file, other.m_dpi)
{
}

SVG::~SVG(){
	close();
}

void SVG::open(){
	//open the file
	NSVGimage* svgFile=nsvgParseFromFile(m_file.c_str(), "px", m_dpi);

	if(!svgFile)
		return; //Could not open the file

	m_resolution=Utils::Resolution(svgFile->width, svgFile->height);

	if(m_resolution){
		NSVGrasterizer* rasterizer = nsvgCreateRasterizer();

		Graphics::GL::ImageBuffer imgBuf(Graphics::GL::ImageAttributes(
				m_resolution,
				Graphics::GL::PixelFormats::RGBA
		));
		imgBuf.data=(u_int8_t*)malloc(imgBuf.att.getSize());

		//Rasterize the image
		nsvgRasterize(
				rasterizer,
				svgFile,
				0, 0, 1,
				imgBuf.data,
				imgBuf.att.res.width,
				imgBuf.att.res.height,
				imgBuf.att.getStride()
		);

		//Upload image to the source
		std::unique_ptr<const Graphics::Frame> newFrame;

		{
			Graphics::UniqueContext ctx(Graphics::Context::getAvalibleCtx());

			std::unique_ptr<Graphics::GL::Texture> tex(
					new Graphics::GL::Texture(imgBuf)
			);

			newFrame=std::unique_ptr<Graphics::Frame>(
					new Graphics::Frame(std::move(tex))
			);
		}

		Source<Graphics::Frame>::push(std::move(newFrame));

		//Deletes the previously generated rasterizer
		nsvgDeleteRasterizer(rasterizer);
		free(imgBuf.data);
		Source<Graphics::Frame>::open();
	}

	//Close the file
	nsvgDelete(svgFile);
}

void SVG::close(){
	Source<Graphics::Frame>::close();
}
