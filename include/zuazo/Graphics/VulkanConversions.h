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
#include "../DepthStencilFormat.h"
#include "../ScalingFilter.h"
#include "../BlendingMode.h"
#include "../RenderingLayer.h"
#include "../Utils/BufferView.h"

#include <tuple>
#include <vector>

namespace Zuazo::Graphics {

constexpr size_t MAX_PLANE_COUNT = 4;

constexpr uint32_t toVulkan(Version version) noexcept;

constexpr vk::DebugUtilsMessageSeverityFlagsEXT toVulkan(Verbosity ver) noexcept;
constexpr Verbosity fromVulkan(vk::DebugUtilsMessageSeverityFlagsEXT ver) noexcept;

constexpr vk::DebugUtilsMessageSeverityFlagBitsEXT toVulkan(Severity sev) noexcept;
constexpr Severity fromVulkan(vk::DebugUtilsMessageSeverityFlagBitsEXT sev) noexcept;

constexpr vk::Extent2D toVulkan(Resolution res) noexcept;
constexpr Resolution fromVulkan(vk::Extent2D res) noexcept;

constexpr vk::Extent2D to2D(vk::Extent3D ext) noexcept;
constexpr vk::Extent3D to3D(vk::Extent2D ext) noexcept;

constexpr vk::SamplerYcbcrModelConversion toVulkan(ColorModel colorModel) noexcept;
constexpr ColorModel fromVulkan(vk::SamplerYcbcrModelConversion model) noexcept;

constexpr vk::SamplerYcbcrRange toVulkan(ColorRange colorRange) noexcept;
constexpr ColorRange fromVulkan(vk::SamplerYcbcrRange range) noexcept;

constexpr std::array<std::tuple<vk::Format, vk::ComponentMapping>, MAX_PLANE_COUNT> toVulkan(ColorFormat fmt) noexcept;
constexpr std::tuple<ColorFormat, ColorTransferFunction> fromVulkan(vk::Format fmt) noexcept;

constexpr vk::Format toVulkan(DepthStencilFormat fmt) noexcept;
constexpr DepthStencilFormat fromVulkanDepthStencil(vk::Format fmt) noexcept;

constexpr std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(const std::tuple<vk::Format, vk::ComponentMapping>& fmt) noexcept;
std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(const std::tuple<vk::Format, vk::ComponentMapping>& fmt,
															Utils::BufferView<const vk::Format> supportedFormats ) noexcept;

constexpr vk::Format toSrgb(vk::Format format) noexcept;
constexpr vk::Format fromSrgb(vk::Format format) noexcept;

constexpr bool hasDepth(vk::Format format) noexcept;
constexpr bool hasStencil(vk::Format format) noexcept;

constexpr size_t getPlaneCount(vk::Format format) noexcept;
constexpr bool requiresYCbCrSamplerConversion(vk::Format format) noexcept;

constexpr vk::ImageAspectFlagBits getPlaneAspectBit(size_t plane) noexcept;

constexpr vk::ComponentSwizzle getComponentDefaultValue(char component) noexcept;
constexpr vk::ComponentSwizzle getComponentSwizzle(char swizzle) noexcept;
constexpr vk::ComponentSwizzle getComponentSwizzle(char component, std::string_view vulkanFromat, std::string_view memoryFormat) noexcept;

constexpr vk::ComponentMapping swizzle(std::string_view swizzle) noexcept;
constexpr vk::ComponentMapping swizzle(std::string_view src, std::string_view dst) noexcept;

constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorTransferFunction enc) noexcept;
constexpr std::tuple<ColorPrimaries, ColorTransferFunction> fromVulkan(vk::ColorSpaceKHR space) noexcept;

constexpr vk::Filter toVulkan(ScalingFilter filt) noexcept;
constexpr ScalingFilter fromVulkan(vk::Filter filt) noexcept;

constexpr vk::PipelineStageFlags toPipelineStageFlags(vk::ShaderStageFlags flags) noexcept;
constexpr vk::ShaderStageFlags toShaderStageFlags(vk::PipelineStageFlags flags) noexcept;

constexpr vk::PipelineColorBlendAttachmentState getBlendingConfiguration(BlendingMode mode) noexcept;
constexpr vk::PipelineDepthStencilStateCreateInfo getDepthStencilConfiguration(RenderingLayer layer) noexcept;


template<typename T>
constexpr vk::ArrayProxy<T> toVulkan(Utils::BufferView<T> bv) noexcept;
template<typename T>
constexpr Utils::BufferView<T> fromVulkan(vk::ArrayProxy<T> ap) noexcept;

}

#include "VulkanConversions.inl"