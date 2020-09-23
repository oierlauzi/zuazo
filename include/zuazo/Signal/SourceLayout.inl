#include "SourceLayout.h"

namespace Zuazo::Signal {

template <typename Tout>
inline SourceLayout<Tout>::SourceLayout(Layout::PadProxy<Output>& output)
	: m_output(output)
{
}



template <typename Tout>
inline Layout::PadProxy<typename SourceLayout<Tout>::Output>& SourceLayout<Tout>::getOutput() {
	return m_output;
}

template <typename Tout>
inline const Layout::PadProxy<typename SourceLayout<Tout>::Output>& SourceLayout<Tout>::getOutput() const {
	return m_output;
}



template <typename Tout>
inline void operator<<(Layout::PadProxy<Input<Tout>>& dst, SourceLayout<Tout>& src) {
	dst << src.getOutput();
}

}