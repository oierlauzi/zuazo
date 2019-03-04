#pragma once

#include "../Graphics/Frame.h"
#include "../Packet.h"
#include "../Stream/Delay.h"

namespace Zuazo::Processors{

typedef Stream::Delay<Packet> 			PacketDelay;
typedef Stream::Delay<Graphics::Frame> 	VideoDelay;
}
