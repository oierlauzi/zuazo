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

class ColorTransfer {
public:
	ColorTransfer();
	ColorTransfer(const Frame::Descriptor& desc);
	ColorTransfer(const ColorTransfer& other) = delete;
	ColorTransfer(ColorTransfer&& other);
	~ColorTransfer();

	ColorTransfer&						operator=(const ColorTransfer& other) = delete;
	ColorTransfer&						operator=(ColorTransfer&& other);

	bool								operator==(const ColorTransfer& other) const;
	bool								operator!=(const ColorTransfer& other) const;

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

}