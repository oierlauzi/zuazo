#pragma once

#include "Vulkan.h"
#include "StagedFrame.h"
#include "../Utils/Pimpl.h"

#include <memory>


namespace Zuazo::Graphics {

class Uploader {
public:
	Uploader(	const Vulkan& vulkan, 
				const Frame::Descriptor& desc );
	Uploader(const Uploader& other) = delete;
	Uploader(Uploader&& other) noexcept;
	~Uploader();

	Uploader& 										operator=(const Uploader& other) = delete;
	Uploader& 										operator=(Uploader&& other) noexcept;

	const Vulkan&									getVulkan() const noexcept;

	void											setMaxSpareCount(size_t spares) noexcept;
	size_t											getMaxSpareCount() const noexcept;
	size_t											getSpareCount() const noexcept;

	std::shared_ptr<StagedFrame>					acquireFrame() const;

	static std::vector<ColorFormat>					getSupportedFormats(const Vulkan& vulkan);

private:
	struct Impl;
	Utils::Pimpl<Impl>								m_impl;

};



}