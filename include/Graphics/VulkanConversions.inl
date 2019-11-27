#include "VulkanConversions.h"

namespace Zuazo::Graphics {

constexpr vk::Extent2D toVulkan(const Resolution& res){
	return vk::Extent2D(
		res.width,
		res.height
	);
}

constexpr Resolution fromVulkan(const vk::Extent2D& res){
	return Resolution(
		res.width,
		res.height
	);
}


}