#pragma once

#include "Vulkan.h"
#include "Frame.h"
#include "../Utils/BufferView.h"
#include "../Utils/Pimpl.h"
#include "../Resolution.h"
#include "../ColorFormat.h"
#include "../ColorSubsampling.h"
#include "../ColorRange.h"
#include "../ColorTransferFunction.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"

#include <utility>
#include <vector>

namespace Zuazo::Graphics {

class InputColorTransfer {
	friend class OutputColorTransfer;
public:
	struct SamplerDescriptor {
		vk::Filter								filter;
		int32_t									samplingMode;
	};

	InputColorTransfer();
	explicit InputColorTransfer(const Frame::Descriptor& desc);
	InputColorTransfer(const Frame::Descriptor& desc, const Chromaticities& customPrimaries);
	InputColorTransfer(const InputColorTransfer& other) = delete;
	InputColorTransfer(InputColorTransfer&& other) noexcept;
	~InputColorTransfer();

	InputColorTransfer&					operator=(const InputColorTransfer& other) = delete;
	InputColorTransfer&					operator=(InputColorTransfer&& other) noexcept;

	bool								operator==(const InputColorTransfer& other) const noexcept;
	bool								operator!=(const InputColorTransfer& other) const noexcept;

	void								optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
													Utils::BufferView<const vk::Format> supportedFormats ) noexcept;

	const std::byte*					data() const noexcept;
	SamplerDescriptor					getSamplerDescriptor(ScalingFilter filter) const noexcept;

	static uint32_t 					getSamplerCount() noexcept;
	static uint32_t 					getSamplerBinding() noexcept;
	static uint32_t 					getDataBinding() noexcept;
	static size_t						size() noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;	
};



class OutputColorTransfer {
public:
	OutputColorTransfer();
	explicit OutputColorTransfer(const Frame::Descriptor& desc);
	OutputColorTransfer(const Frame::Descriptor& desc, const Chromaticities& customPrimaries);
	explicit OutputColorTransfer(const InputColorTransfer& inputTransfer);
	OutputColorTransfer(const OutputColorTransfer& other) = delete;
	OutputColorTransfer(OutputColorTransfer&& other) noexcept;
	~OutputColorTransfer();

	OutputColorTransfer&				operator=(const OutputColorTransfer& other) = delete;
	OutputColorTransfer&				operator=(OutputColorTransfer&& other) noexcept;

	bool								operator==(const OutputColorTransfer& other) const noexcept;
	bool								operator!=(const OutputColorTransfer& other) const noexcept;

	void								optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
													Utils::BufferView<const vk::Format> supportedFormats ) noexcept;

	const std::byte*					data() const noexcept;
	static size_t						size() noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;	
};

}