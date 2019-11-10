#pragma once

#include "Timing/MainLoop.h"
#include "Graphics/Instance.h"

namespace Zuazo {

class Instance {
public:
	Instance() = default;
	Instance(const Instance& other) = delete;
	Instance(Instance&& other) = delete;
	~Instance() = default;

	Instance& operator=(const Instance& other) = delete;
	Instance& operator=(Instance&& other) = delete;

	Timing::MainLoop&			getMainLoop();
	const Timing::MainLoop&		getMainLoop() const;

	Graphics::Instance&			getGraphics();
	const Graphics::Instance&	getGraphics() const;
private:
	Timing::MainLoop			m_loop;
	Graphics::Instance			m_graphics;
};

}