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
	InputColorTransfer();
	explicit InputColorTransfer(const Frame::Descriptor& desc);
	InputColorTransfer(const InputColorTransfer& other) = delete;
	InputColorTransfer(InputColorTransfer&& other);
	~InputColorTransfer();

	InputColorTransfer&					operator=(const InputColorTransfer& other) = delete;
	InputColorTransfer&					operator=(InputColorTransfer&& other);

	bool								operator==(const InputColorTransfer& other) const;
	bool								operator!=(const InputColorTransfer& other) const;

	void								optimize(	Utils::BufferView<Frame::PlaneDescriptor> planes,
													Utils::BufferView<const vk::Format> supportedFormats );

	const std::byte*					data() const;

	static uint32_t 					getSamplerCount();
	static uint32_t 					getSamplerBinding();
	static uint32_t 					getDataBinding();
	static size_t						size();

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;	
};



class OutputColorTransfer {
public:
	OutputColorTransfer();
	explicit OutputColorTransfer(const Frame::Descriptor& desc);
	explicit OutputColorTransfer(const InputColorTransfer& inputTransfer);
	OutputColorTransfer(const OutputColorTransfer& other) = delete;
	OutputColorTransfer(OutputColorTransfer&& other);
	~OutputColorTransfer();

	OutputColorTransfer&				operator=(const OutputColorTransfer& other) = delete;
	OutputColorTransfer&				operator=(OutputColorTransfer&& other);

	bool								operator==(const OutputColorTransfer& other) const;
	bool								operator!=(const OutputColorTransfer& other) const;

	void								optimize(	Utils::BufferView<Frame::PlaneDescriptor> planes,
													Utils::BufferView<const vk::Format> supportedFormats );

	const std::byte*					data() const;
	static size_t						size();

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;	
};

}