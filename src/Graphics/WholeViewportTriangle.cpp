#include <zuazo/Graphics/WholeViewportTriangle.h>

namespace Zuazo::Graphics {

Utils::BufferView<const uint32_t> getWholeViewportTriangleSPIRV() noexcept {
	static
	#include <whole_viewport_triangle_vert.h>
	return whole_viewport_triangle_vert;
}
	
}