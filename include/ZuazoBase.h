#pragma once

#include "Instance.h"
#include "Signal/Layout.h"
#include "VideoMode.h"

#include <utility>

namespace Zuazo {

class ZuazoBase : public Signal::Layout {
public:
	ZuazoBase(const Instance& instance, std::string&& name);
	ZuazoBase(const ZuazoBase& other) = delete;
	ZuazoBase(ZuazoBase&& other) = default;
	virtual ~ZuazoBase() = default;

	ZuazoBase& 								operator=(const ZuazoBase& other) = delete;
	ZuazoBase& 								operator=(ZuazoBase&& other) = default;

	const Instance&							getInstance() const;

	virtual void 							open() = 0;
	virtual void 							close() = 0;
	bool 									isOpen() const;

	virtual void							setVideoMode(const VideoMode& videoMode) = 0;
	const VideoMode&						getVideoMode() const;

private:
	std::reference_wrapper<const Instance> 	m_instance;

	bool    								m_isOpen;

	VideoMode								m_videoMode;

	
};

}