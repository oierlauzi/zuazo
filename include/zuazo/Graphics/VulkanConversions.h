#pragma once

#include "Vulkan.h"

#include "../Version.h"
#include "../Verbosity.h"
#include "../Resolution.h"
#include "../ColorFormat.h"
#include "../ColorRange.h"
#include "../ColorSubsampling.h"
#include "../ColorTransferFunction.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"
#include "../ScalingFilter.h"
#include "../Utils/BufferView.h"

#include <tuple>
#include <vector>

namespace Zuazo::Graphics {

constexpr size_t MAX_PLANE_COUNT = 4;

constexpr uint32_t toVulkan(Version version);

constexpr vk::DebugUtilsMessageSeverityFlagsEXT toVulkan(Verbosity ver);
constexpr Verbosity fromVulkan(vk::DebugUtilsMessageSeverityFlagsEXT ver);

constexpr vk::DebugUtilsMessageSeverityFlagBitsEXT toVulkan(Severity sev);
constexpr Severity fromVulkan(vk::DebugUtilsMessageSeverityFlagBitsEXT sev);

constexpr vk::Extent2D toVulkan(Resolution res);
constexpr Resolution fromVulkan(vk::Extent2D res);

constexpr std::array<std::tuple<vk::Format, vk::ComponentMapping>, MAX_PLANE_COUNT> toVulkan(ColorFormat fmt);
constexpr std::tuple<ColorFormat, ColorTransferFunction> fromVulkan(vk::Format fmt);

std::vector<ColorFormat> getFramebufferFormats(Utils::BufferView<const vk::Format> supported);
std::vector<ColorFormat> getSamplerFormats(Utils::BufferView<const vk::Format> supported);

constexpr std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(const std::tuple<vk::Format, vk::ComponentMapping>& fmt);
std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(const std::tuple<vk::Format, vk::ComponentMapping>& fmt,
															Utils::BufferView<const vk::Format> supportedFormats );

constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorTransferFunction enc);
constexpr std::tuple<ColorPrimaries, ColorTransferFunction> fromVulkan(vk::ColorSpaceKHR space);

constexpr vk::ComponentSwizzle getComponentDefaultValue(char component);
constexpr vk::ComponentSwizzle getComponentSwizzle(char swizzle);
constexpr vk::ComponentSwizzle getComponentSwizzle(char component, std::string_view vulkanFromat, std::string_view memoryFormat);

constexpr vk::ComponentMapping swizzle(std::string_view swizzle);
constexpr vk::ComponentMapping swizzle(std::string_view src, std::string_view dst);

constexpr vk::Format toSrgb(vk::Format format);
constexpr vk::Format fromSrgb(vk::Format format);

constexpr vk::Filter toVulkan(ScalingFilter filt);
constexpr ScalingFilter fromVulkan(vk::Filter filt);

template<typename T>
constexpr vk::ArrayProxy<T> toVulkan(Utils::BufferView<T> bv);
template<typename T>
constexpr Utils::BufferView<T> fromVulkan(vk::ArrayProxy<T> ap);


}

#include "VulkanConversions.inl"