#pragma once

namespace Zuazo::Graphics::GL{

enum class PixelTypes{
	RGB		=1<<0,
	A		=1<<1,
	RGBA	=RGB | A,
};
}
