#pragma once

#include <array>
#include <string>
#include <functional>

namespace Zuazo {

using Version = std::array<uint32_t, 4>;
constexpr Version version = {0, 1, 0};

struct ApplicationInfo {
	std::string		name;
	std::string		description;
	Version			version = {};
	bool			isDebug = true;
};

extern void init(ApplicationInfo&& appinfo = ApplicationInfo());
extern void end();

extern const Version& getRuntimeVersion();
extern const ApplicationInfo& getApplicationInfo();


}
