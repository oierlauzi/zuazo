#pragma once

#include <zuazo/Utils/BufferView.h>

#include <cstdint>

namespace Zuazo::Graphics {

Utils::BufferView<const uint32_t> getWholeViewportTriangleSPIRV() noexcept;

}