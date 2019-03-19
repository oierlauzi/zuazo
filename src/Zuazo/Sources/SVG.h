#pragma once

#include <string>

#include "../Utils/Resolution.h"
#include "../Video/VideoOutputBase.h"
#include "../Video/VideoStream.h"
#include "../ZuazoBase.h"

namespace Zuazo::Sources{
class SVG :
		public Video::TVideoOutputBase<Video::VideoSourcePad<SVG>>,
		public ZuazoBase
{
public:
	SVG(const std::string& dir, float dpi);
	SVG(const SVG& other);
	~SVG();

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
