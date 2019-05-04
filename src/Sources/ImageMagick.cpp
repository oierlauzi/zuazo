#include <Sources/ImageMagick.h>

#include <Utils/Resolution.h>
#include <Utils/PixelFormat.h>
#include <Graphics/Frame.h>
#include <Graphics/Uploader.h>
#include <Graphics/ImageAttributes.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/ImageBuffer.h>

using namespace Zuazo::Sources;

ImageMagick::ImageMagick(){
	open();
}

ImageMagick::ImageMagick(const std::shared_ptr<const Magick::Image>& img) :
		m_image(img)
{
	open();
}

ImageMagick::ImageMagick(const std::string& path){
	std::shared_ptr<Magick::Image> img(new Magick::Image(path));
	img->magick("RGBA");
	m_image=img;
	open();
}

void ImageMagick::open(){
	if(m_image){
		Magick::Image img(*m_image);

		Magick::Blob blob; 
		img.write(&blob, "RGBA", sizeof(Magick::Quantum) * 8);
		Magick::Geometry size=img.size();

		Graphics::ImageAttributes att(
				Graphics::Resolution(size.width(), size.height()),
				Graphics::PixelFormat(GL_RGBA, Graphics::GL::GLType<Magick::Quantum>, SWIZZLE_MASK_RGBA)
		);

		Graphics::ImageBuffer imgBuf(att, (u_int8_t*)blob.data());

		//Try to create a texture with the image
		std::unique_ptr<Graphics::GL::Texture2D> tex=Graphics::Frame::createTexture(att);
		if(tex){
			//There was an available texture in the pool
			Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBinding(*tex);
			imgBuf.textureSubImage();
		}else{
			//There wasn't an available texture in the pool -> create it
			tex=std::unique_ptr<Graphics::GL::Texture2D>(new Graphics::GL::Texture2D);

			Graphics::GL::UniqueBinding<Graphics::GL::Texture2D> texBinding(*tex);
			imgBuf.textureImage();
		}

		std::unique_ptr<const Graphics::Frame> frame(new Graphics::Frame(std::move(tex), att));
		m_videoSourcePad.push(std::move(frame));

		m_videoMode.res=att.res;
		m_videoMode.pixFmt=att.pixFmt.toPixelFormat();		
	}else{
		m_videoSourcePad.push();
		m_videoMode.res=Utils::Resolution();
		m_videoMode.pixFmt=Utils::PixelFormat();
	}
	ZuazoBase::open();
}

void ImageMagick::close(){
	m_videoSourcePad.reset();
	ZuazoBase::close();
}
