#pragma once

#include "Vulkan.h"
#include "StagedFrame.h"
#include "../Utils/Pimpl.h"

#include <memory>


namespace Zuazo::Graphics {

class Uploader {
public:
	Uploader(	const Vulkan& vulkan, 
				const Frame::Descriptor& conf );
	Uploader(const Uploader& other) = delete;
	Uploader(Uploader&& other);
	~Uploader();

	Uploader& 										operator=(const Uploader& other) = delete;
	Uploader& 										operator=(Uploader&& other);

	const Vulkan&									getVulkan() const;
	std::shared_ptr<StagedFrame>					acquireFrame() const;
	void											clear();
	void											shrink(size_t maxSpares = 0);

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};



}