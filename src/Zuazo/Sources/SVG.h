#pragma once

#include <string>

#include "../Utils/Resolution.h"
#include "../Video/VideoStream.h"
#include "../Video.h"
#include "../ZuazoBase.h"

namespace Zuazo::Sources{
class SVG :
		public VideoBase,
		public ZuazoBase
{
public:
	Video::VideoSourcePad<SVG>		videoOut;

	SVG(const std::string& dir, float dpi);
	SVG(const SVG& other);
	~SVG();

	void							open() override;
	void							close() override;


	void 							setDpi(float dpi);
	float							getDpi() const;
	Utils::Resolution 				getRes() const;
protected:
	void							update() const {};
private:
	std::string						m_file;
	float							m_dpi;
	Utils::Resolution				m_resolution;
};

inline void SVG::setDpi(float dpi){
	close();
	m_dpi=dpi;
	open();
}

inline float SVG::getDpi() const{
	return m_dpi;
}

inline Utils::Resolution SVG::getRes() const{
	return m_resolution;
}
}
