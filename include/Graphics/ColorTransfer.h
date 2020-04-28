#pragma once

#include "Vulkan.h"
#include "Image.h"
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
	ColorTransfer(	ColorFormat format,
					ColorRange range,
					ColorTransferFunction transferFunc,
					ColorModel model,
					ColorPrimaries primaries );
	ColorTransfer(const ColorTransfer& other) = delete;
	ColorTransfer(ColorTransfer&& other);
	~ColorTransfer();

	ColorTransfer&						operator=(const ColorTransfer& other) = delete;
	ColorTransfer&						operator=(ColorTransfer&& other);

	void								optimize(	Utils::BufferView<Image::PlaneDescriptor> planes,
													Utils::BufferView<const vk::Format> supportedFormats );

	const std::byte*					data() const;

	Buffer								createBuffer(const Vulkan& vulkan) const;

	static uint32_t 					getSamplerCount();
	static uint32_t 					getSamplerBinding();
	static uint32_t 					getDataBinding();
	static size_t						size();

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;	
};

}