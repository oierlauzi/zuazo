#include "ConsumerLayout.h"

namespace Zuazo::Signal {

template <typename Tin>
inline ConsumerLayout<Tin>::ConsumerLayout(Layout::PadProxy<Input>& input)
	: m_input(input)
{
}



template <typename Tin>
inline Layout::PadProxy<typename ConsumerLayout<Tin>::Input>& ConsumerLayout<Tin>::getInput() {
	return m_input;
}

template <typename Tin>
inline const Layout::PadProxy<typename ConsumerLayout<Tin>::Input>& ConsumerLayout<Tin>::getInput() const {
	return m_input;
}



template <typename Tin>
inline void operator<<(ConsumerLayout<Tin>& dst, Layout::PadProxy<Output<Tin>>& src) {
	dst.getInput() << src;
}

template <typename Tin>
inline void operator<<(ConsumerLayout<Tin>& dst, SourceLayout<Tin>& src) {
	dst.getInput() << src;
}

}