#pragma once

#include "../ZuazoBase.h"
#include "../Video.h"
#include "../ScalingMode.h"
#include "../ScalingFilter.h"
#include "../Utils/Pimpl.h"
#include "../Signal/Input.h"

#include <tuple>

namespace Zuazo::Outputs{

class Window final
	: public ZuazoBase
	, public VideoBase
{
public:
	Window(Instance& instance, const std::string& name);
	Window(Instance& instance, std::string&& name);
	Window(const Window& other) = delete;
	Window(Window&& other);
	virtual ~Window();

	Window&					operator=(const Window& other) = delete;
	Window&					operator=(Window&& other);

	virtual void 			open()  final;
	virtual void 			close() final;
	
	virtual void 			setVideoMode(const VideoMode& videoMode) final;

	void					setScalingMode(ScalingMode mode);
	ScalingMode				getScalingMode() const;

	void					setScalingFilter(ScalingFilter filter);
	ScalingFilter			getScalingFilter() const;

	inline static const auto PADS = std::make_tuple(Signal::Input<Video>("videoIn0"));

private:
	struct Impl;
	Utils::Pimpl<Impl>		m_impl;

};

}