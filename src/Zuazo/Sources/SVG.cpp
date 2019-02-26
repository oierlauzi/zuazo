#include "SVG.h"

#include <algorithm>
#include <memory>

#include "../Graphics/Context.h"
#include "../Graphics/Frame.h"
#include "../Graphics/GL/Texture.h"
#include "../Stream/Packet.h"
#include "../Utils/ImageAttributes.h"
#include "../Utils/ImageBuffer.h"
#include "../Utils/PixelTypes.h"

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
		SVG(other.m_file, other.m_resolution)
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

		Utils::ImageBuffer imgBuf(Utils::ImageAttributes(
				m_resolution,
				Utils::PixelTypes::RGBA
		));

		//Rasterize the image
		nsvgRasterize(
				rasterizer,
				svgFile,
				0, 0, 1,
				imgBuf.data,
				imgBuf.att.res.width,
				imgBuf.att.res.height,
				imgBuf.att.stride()
		);

		//Upload image to the source
		std::unique_ptr<Graphics::Frame> newFrame;

		{
			Graphics::UniqueContext ctx(Graphics::Context::getAvalibleCtx());

			std::unique_ptr<Graphics::GL::Texture> tex(
					new Graphics::GL::Texture(imgBuf)
			);

			newFrame=std::unique_ptr<Graphics::Frame>(
					new Graphics::Frame(std::move(tex))
			);
		}

		std::unique_ptr<const Stream::Packet> packet(new Stream::Packet(Stream::Packet::Data{
			std::move(newFrame)
		}));

		SourceBase::push(std::move(packet));

		//Deletes the previously generated rasterizer
		nsvgDeleteRasterizer(rasterizer);
		SourceBase::open();
	}

	//Close the file
	nsvgDelete(svgFile);
}

void SVG::close(){
	SourceBase::close();
}
