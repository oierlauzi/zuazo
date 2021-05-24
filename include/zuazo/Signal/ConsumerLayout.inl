#include "ConsumerLayout.h"

namespace Zuazo::Signal {

template <typename Tin>
inline ConsumerLayout<Tin>::ConsumerLayout(PadProxy<Input>& input) noexcept
	: m_input(input)
{
}



template <typename Tin>
inline PadProxy<typename ConsumerLayout<Tin>::Input>& ConsumerLayout<Tin>::getInput() noexcept {
	return m_input;
}

template <typename Tin>
inline const PadProxy<typename ConsumerLayout<Tin>::Input>& ConsumerLayout<Tin>::getInput() const noexcept {
	return m_input;
}



template <typename Tin>
inline void operator<<(ConsumerLayout<Tin>& dst, PadProxy<Output<Tin>>& src) noexcept {
	dst.getInput() << src;
}

template <typename Tin>
inline void operator<<(ConsumerLayout<Tin>& dst, SourceLayout<Tin>& src) noexcept {
	dst.getInput() << src;
}

}