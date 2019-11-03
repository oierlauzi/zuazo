#pragma once

#include "Timing/MainLoop.h"
#include "Graphics/Vulkan.h"

#include <array>
#include <string>
#include <functional>

namespace Zuazo {

using Version = std::array<uint16_t, 4>;

constexpr Version 		version = {0, 1, 0};
extern const Version 	runtimeVersion;



struct ApplicationInfo {
	std::string		name;
	std::string		description;
	Version			version = {};
	bool			isDebug = true;
};

extern void setApplicationInfo(ApplicationInfo&& appInfo);
const ApplicationInfo& getApplicationInfo();



class Instance {
public:
	Instance();
	Instance(const Instance& other) = delete;
	Instance(Instance&& other) = delete;
	~Instance();

	Instance& operator=(const Instance& other) = delete;
	Instance& operator=(Instance&& other) = delete;

	friend bool operator==(const Instance& a, const Instance& b);
	friend bool operator!=(const Instance& a, const Instance& b);

	Timing::MainLoop&			getMainLoop();
	const Timing::MainLoop&		getMainLoop() const;

	Graphics::Vulkan&			getVulkan();
	const Graphics::Vulkan&		getVulkan() const;

	static uint32_t				getInstaceCount();
private:
	Timing::MainLoop			m_loop;
	Graphics::Vulkan			m_vulkan;

	static uint32_t				s_instaceCount;
};

}
