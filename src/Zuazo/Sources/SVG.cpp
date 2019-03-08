#include "SVG.h"

#include <algorithm>
#include <deque>
#include <memory>

#include "../Graphics/Context.h"
#include "../Graphics/GL/Texture2D.h"
#include "../Graphics/GL/UniqueBinding.h"
#include "../Graphics/ImageAttributes.h"
#include "../Graphics/ImageBuffer.h"
#include "../Graphics/PixelFormat.h"

namespace Zuazo {
namespace Graphics {
struct ImageAttributes;
} /* namespace Graphics */
} /* namespace Zuazo */

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

		Graphics::ImageBuffer imgBuf(Graphics::ImageAttributes(
				m_resolution,
				Graphics::PixelFormat::RGBA
		));

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

			std::unique_ptr<Graphics::GL::Texture2D> tex(
					new Graphics::GL::Texture2D
			);

			//Upload the image to the texture
			Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBinding(*tex);
			Graphics::GL::Texture2D::textureImage(imgBuf);

			newFrame=std::unique_ptr<Graphics::Frame>(new Graphics::Frame(
					std::move(tex),
					Graphics::ImageAttributes(m_resolution, Graphics::PixelFormat::RGBA)
			));
		}

		Source<Graphics::Frame>::push(std::move(newFrame));

		//Deletes the previously generated rasterizer
		nsvgDeleteRasterizer(rasterizer);
		Source<Graphics::Frame>::open();
		Updateables::Updateable::open();
	}

	//Close the file
	nsvgDelete(svgFile);
}

void SVG::close(){
	Source<Graphics::Frame>::close();
	Updateables::Updateable::close();
}
