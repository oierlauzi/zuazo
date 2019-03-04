#pragma once

#include <memory>

namespace Zuazo {
namespace Graphics {
class Frame;
} /* namespace Graphics */
} /* namespace Zuazo */

namespace Zuazo{
struct Packet{
public:
	std::shared_ptr<const Graphics::Frame> 		frame;
};

}
