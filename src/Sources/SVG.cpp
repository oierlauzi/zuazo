#include <Sources/SVG.h>

#include <Graphics/Context.h>
#include <Graphics/Frame.h>
#include <Graphics/GL/Texture2D.h>
#include <Graphics/GL/UniqueBinding.h>
#include <Graphics/ImageAttributes.h>
#include <Graphics/ImageBuffer.h>
#include <Graphics/PixelFormat.h>
#include <Utils/PixelFormat.h>
#include <Utils/Resolution.h>

#include <algorithm>
#include <memory>

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

	m_videoMode.res=Utils::Resolution(svgFile->width, svgFile->height);

	if(m_videoMode.res){
		NSVGrasterizer* rasterizer = nsvgCreateRasterizer();

		Graphics::ImageBuffer imgBuf(Graphics::ImageAttributes(
				m_videoMode.res,
				Graphics::PixelFormats::RGBA32
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

		//Try to create a texture with the image
		std::unique_ptr<Graphics::GL::Texture2D> tex=Graphics::Frame::createTexture(imgBuf.att);
		if(tex){
			//There was an available texture in the pool
			Graphics::UniqueContext ctx;
			Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBinding(*tex);
			imgBuf.textureSubImage();
		}else{
			//There wasn't an available texture in the pool -> create it
			Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());
			tex=std::unique_ptr<Graphics::GL::Texture2D>(new Graphics::GL::Texture2D);

			Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBinding(*tex);
			imgBuf.textureImage();
		}

		//Upload image to the source
		std::unique_ptr<const Graphics::Frame> newFrame(new Graphics::Frame(std::move(tex), imgBuf.att));
		m_videoSourcePad.push(std::move(newFrame));

		m_videoMode.res=imgBuf.att.res;
		m_videoMode.pixFmt=imgBuf.att.pixFmt.toPixelFormat();

		//Deletes the previously generated rasterizer
		nsvgDeleteRasterizer(rasterizer);
		ZuazoBase::open();
	}

	//Close the file
	nsvgDelete(svgFile);
}

void SVG::close(){
	m_videoSourcePad.reset();
	ZuazoBase::close();
}
