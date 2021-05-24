#include "SourceLayout.h"

namespace Zuazo::Signal {

template <typename Tout>
inline SourceLayout<Tout>::SourceLayout(PadProxy<Output>& output) noexcept
	: m_output(output)
{
}



template <typename Tout>
inline PadProxy<typename SourceLayout<Tout>::Output>& SourceLayout<Tout>::getOutput() noexcept {
	return m_output;
}

template <typename Tout>
inline const PadProxy<typename SourceLayout<Tout>::Output>& SourceLayout<Tout>::getOutput() const noexcept {
	return m_output;
}



template <typename Tout>
inline void operator<<(PadProxy<Input<Tout>>& dst, SourceLayout<Tout>& src) noexcept {
	dst << src.getOutput();
}

}