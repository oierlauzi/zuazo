#pragma once

#include "../Video/VideoSourceBase.h"
#include "../Video/VideoStream.h"
#include "../ZuazoBase.h"

#include <string>

namespace Zuazo::Sources{
class SVG :
		public Video::TVideoSourceBase<Video::VideoSourcePad>,
		public ZuazoBase
{
public:
	SVG(const std::string& dir, float dpi);
	SVG(const SVG& other);
	~SVG();

	SUPPORTS_GETTING_PIXELFORMAT
	SUPPORTS_GETTING_RESOLUTION
	SUPPORTS_GETTING_VIDEOMODE

	void							open() override;
	void							close() override;

	void 							setDpi(float dpi);
	float							getDpi() const;
protected:
	void							update() const {};
private:
	std::string						m_file;
	float							m_dpi;
};

inline void SVG::setDpi(float dpi){
	close();
	m_dpi=dpi;
	open();
}

inline float SVG::getDpi() const{
	return m_dpi;
}
}
