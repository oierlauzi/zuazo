#pragma once

#include <sys/types.h>
#include <algorithm>
#include <atomic>
#include <memory>

#include "Graphics/Frame.h"

namespace Zuazo{
struct Packet{
public:
	struct Data{
		std::unique_ptr<Graphics::Frame>		frame;
	};

	const Data								data;
	const u_int32_t 						id;

	Packet()=delete;
	Packet(Data data);
	Packet(const Packet& other)=delete;
	Packet(Packet&& other)=delete;
	~Packet()=default;
private:
	static std::atomic<u_int32_t>			lastId;
};

inline Packet::Packet(Data data) :
		data(std::move(data)),
		id(lastId++)
{
}


}
