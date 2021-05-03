#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "Sampler.h"
#include "../Utils/Pimpl.h"
#include "../Utils/BufferView.h"

#include <cstdint>

namespace Zuazo::Graphics {

class ColorTransferRead {
public:
	ColorTransferRead();
	explicit ColorTransferRead(const Frame::Descriptor& desc);
	ColorTransferRead(const ColorTransferRead& other) = delete;
	ColorTransferRead(ColorTransferRead&& other);
	~ColorTransferRead();

	ColorTransferRead& operator=(const ColorTransferRead& other) = delete;
	ColorTransferRead& operator=(ColorTransferRead&& other);

	bool operator==(const ColorTransferRead& rhs) const noexcept;
	bool operator!=(const ColorTransferRead& rhs) const noexcept;

	size_t size() const noexcept;
	std::byte* data() noexcept;
	const std::byte* data() const noexcept;

	void optimize(	Utils::BufferView<Image::Plane> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept;
	void optimize(const Sampler& sampler) noexcept;

	bool isPassthough() const noexcept;
	bool isLinear() const noexcept;
	size_t getPlaneCount() const noexcept;


	vk::DescriptorSetLayout createDescriptorSetLayout(	const Vulkan& vulkan,
														const Sampler& sampler ) const;

	static Utils::BufferView<const vk::SpecializationMapEntry> getSpecializationMap() noexcept;

	static Utils::BufferView<const uint32_t> getSPIRV() noexcept;

	static size_t getSamplerBinding() noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>	m_impl;

};



class ColorTransferWrite {
public:
	ColorTransferWrite();
	explicit ColorTransferWrite(const Frame::Descriptor& desc);
	ColorTransferWrite(const ColorTransferWrite& other) = delete;
	ColorTransferWrite(ColorTransferWrite&& other);
	~ColorTransferWrite();

	ColorTransferWrite& operator=(const ColorTransferWrite& other) = delete;
	ColorTransferWrite& operator=(ColorTransferWrite&& other);

	bool operator==(const ColorTransferWrite& rhs) const noexcept;
	bool operator!=(const ColorTransferWrite& rhs) const noexcept;

	size_t size() const noexcept;
	std::byte* data() noexcept;
	const std::byte* data() const noexcept;

	void optimize(	Utils::BufferView<Image::Plane> planes,
					Utils::BufferView<const vk::Format> supportedFormats ) noexcept;

	bool isPassthough() const noexcept;
	bool isLinear() const noexcept;
	size_t getPlaneCount() const noexcept;


	static vk::DescriptorSetLayout createDescriptorSetLayout(const Vulkan& vulkan);

	static Utils::BufferView<const vk::SpecializationMapEntry> getSpecializationMap() noexcept;

	static Utils::BufferView<const uint32_t> getSPIRV() noexcept;

	static size_t getInputAttachmentBinding() noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>	m_impl;
};

}
