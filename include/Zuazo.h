#pragma once

#include <array>
#include <string>

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

}
