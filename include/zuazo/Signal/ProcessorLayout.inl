#include "ProcessorLayout.h"

namespace Zuazo::Signal {

template <typename Tin, typename Tout>
inline ProcessorLayout<Tin, Tout>::ProcessorLayout(	PadProxy<typename ConsumerLayout<Tin>::Input>& input,
													PadProxy<typename SourceLayout<Tout>::Output>& output ) noexcept
	: ConsumerLayout<Tin>(input)
	, SourceLayout<Tout>(output)
{
}



template <typename Tin, typename Tout>
inline ProcessorLayout<Tin, Tout>& operator<<(PadProxy<Input<Tout>>& dst, ProcessorLayout<Tin, Tout>& src) noexcept {
	dst << src.getOutput();
	return src;
}

template <typename Tin, typename Tout>
inline ProcessorLayout<Tin, Tout>& operator<<(ConsumerLayout<Tout>& dst, ProcessorLayout<Tin, Tout>& src) noexcept {
	dst << src.getOutput();
	return src;
}

}