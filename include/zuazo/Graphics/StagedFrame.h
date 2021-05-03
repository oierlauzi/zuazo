#pragma once

#include "Vulkan.h"
#include "Frame.h"

#include "../Utils/Pimpl.h"

namespace Zuazo::Graphics {

class StagedFrame 
	: public Frame 
{
public:
	class Cache;

	using PixelData = Utils::BufferView<const Utils::BufferView<std::byte>>;
	using ConstPixelData = Utils::BufferView<const Utils::BufferView<const std::byte>>;

	StagedFrame(const Vulkan& vulkan,
				std::shared_ptr<const Descriptor> desc,
				std::shared_ptr<const Cache> cache = nullptr,
				std::shared_ptr<void> usrPtr = nullptr  );
	StagedFrame(const StagedFrame& other) = delete;
	StagedFrame(StagedFrame&& other) noexcept;
	virtual ~StagedFrame(); 

	StagedFrame& 								operator=(const StagedFrame& other) noexcept = delete;
	StagedFrame&								operator=(StagedFrame&& other) noexcept;

	PixelData									getPixelData() noexcept;
	ConstPixelData								getPixelData() const noexcept;
	void										flush();
	bool										waitCompletion(uint64_t timeo) const;

	static std::shared_ptr<const Cache>			createCache(const Vulkan& vulkan, 
															const Frame::Descriptor& frameDesc );

	static Utils::Discrete<ColorFormat>			getSupportedFormats(const Vulkan& vulkan);

private:
	struct Impl;
	Utils::Pimpl<Impl>							m_impl;

};



}