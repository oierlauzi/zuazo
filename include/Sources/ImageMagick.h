#pragma once

#include "../Video/VideoOutputBase.h"
#include "../Video/VideoStream.h"
#include "../ZuazoBase.h"

#include <memory>
#include <string>

namespace Magick {
class Image;
} /* namespace Magick */

namespace Zuazo::Sources{

class ImageMagick :
	public Video::TVideoOutputBase<Video::VideoSourcePad<ImageMagick>>,
	public ZuazoBase
{
public:
	ImageMagick()=default;
	ImageMagick(const std::shared_ptr<const Magick::Image>& img);
	ImageMagick(const std::string& path);
	ImageMagick(const ImageMagick& other)=default;
	~ImageMagick()=default;

	std::shared_ptr<const Magick::Image>	getCurrentImage() const;

	void									replace(const std::shared_ptr<const Magick::Image>& img);
	void 									reload();

	void 									open() override;
	void 									close() override;
private:
	std::shared_ptr<const Magick::Image>	m_image;
};

inline std::shared_ptr<const Magick::Image>	ImageMagick::getCurrentImage() const{
	return m_image;
}

inline void ImageMagick::replace(const std::shared_ptr<const Magick::Image>& img){
	m_image=img;
	reload();
}

inline void ImageMagick::ImageMagick::reload(){
	if(isOpen()){
		close();
		open();
	}
}
}
