#pragma once

#include "ImageAttributes.h"

extern "C"{
	#include <libavutil/pixfmt.h>
}
#include <stddef.h>
#include <memory>

#include "../Utils/ImageAttributes.h"
#include "../Utils/ImageBuffer.h"
#include "ImageBuffer.h"

struct SwsContext;

namespace Zuazo {
namespace Graphics {
class Frame;
} /* namespace Graphics */
} /* namespace Zuazo */

namespace Zuazo::Graphics{

class Uploader{
public:
	Uploader();
	Uploader(const Uploader& other);
	~Uploader();

	std::unique_ptr<const Frame> 	getFrame(const Utils::ImageBuffer& buf) const;
private:
	mutable Utils::ImageAttributes	m_srcAttributes;
	mutable ImageAttributes			m_destAttributes;
	mutable int						m_srcStrides[4];
	mutable int						m_destStrides[4];
	mutable size_t					m_destSize;

	mutable SwsContext*				m_swsContext;

	static ImageAttributes			getBestMatch(const Utils::ImageAttributes& other);

};

}
