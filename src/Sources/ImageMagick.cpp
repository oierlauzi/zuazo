#include <Sources/ImageMagick.h>

#include <Utils/PixelFormat.h>
#include <Utils/Resolution.h>
#include <Graphics/Frame.h>
#include <Graphics/Uploader.h>
#include <Graphics/ImageAttributes.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/ImageBuffer.h>

#include <Magick++.h>

using namespace Zuazo::Sources;

ImageMagick::ImageMagick(const std::shared_ptr<const Magick::Image>& img) :
		m_image(img)
{
	open();
}

ImageMagick::ImageMagick(const std::string& path) :
		ImageMagick(std::shared_ptr<const Magick::Image>(new Magick::Image(path)))
{
}

void ImageMagick::open(){
	if(m_image){
		Magick::Geometry size=m_image->size();
		//Get all the pixels
		const Magick::PixelPacket* pixels=m_image->getConstPixels(0, 0, size.width(), size.height());

		Graphics::ImageAttributes att(
				Graphics::Resolution(size.width(), size.height()),
				Graphics::PixelFormat(GL_RGBA, Graphics::GL::GLType<Magick::Quantum>)
		);

		Graphics::ImageBuffer imgBuf(att);

		//Reorder pixels
		size_t nPixels=size.width() * size.height();
		Magick::PixelPacket* bufPtr=(Magick::PixelPacket*) imgBuf.data;

		for(size_t i=0; i<nPixels; i++){
			bufPtr[i].blue		=pixels[i].red;
			bufPtr[i].green		=pixels[i].green;
			bufPtr[i].red		=pixels[i].blue;
			bufPtr[i].opacity	=pixels[i].red;
		}

		//Try to create a texture with the image
		std::unique_ptr<Graphics::GL::Texture2D> tex=Graphics::Frame::createTexture(att);
		if(tex){
			//There was an available texture in the pool
			Graphics::UniqueContext ctx(Graphics::Context::getAvailableCtx());
			Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBinding(*tex);
			imgBuf.textureSubImage();
		}else{
			//There wasn't an available texture in the pool -> create it
			Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());
			tex=std::unique_ptr<Graphics::GL::Texture2D>(new Graphics::GL::Texture2D);

			Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBinding(*tex);
			imgBuf.textureImage();
		}

		std::unique_ptr<const Graphics::Frame> frame(new Graphics::Frame(std::move(tex), att));
		m_videoSourcePad.push(std::move(frame));

		m_videoMode.res=att.res;
		m_videoMode.pixFmt=att.pixFmt.toPixelFormat();

		ZuazoBase::open();
	}
}

void ImageMagick::close(){
	m_videoSourcePad.reset();
	ZuazoBase::close();
}
