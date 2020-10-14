#pragma once W

#include "Layout.h"
#include "Input.h"
#include "Output.h"

#include <utility>

namespace Zuazo::Signal {

template <typename Tout>
class SourceLayout {
public:
	using OutputType = Tout;
	using Output = Signal::Output<OutputType>;

	explicit SourceLayout(Layout::PadProxy<Output>& output) noexcept;
	SourceLayout(const SourceLayout& other) noexcept = default;
	SourceLayout(SourceLayout&& other) noexcept = default;
	virtual ~SourceLayout() = default;

	SourceLayout&										operator=(const SourceLayout& other) noexcept = default;
	SourceLayout&										operator=(SourceLayout&& other) noexcept = default;
	
	Layout::PadProxy<Output>&							getOutput() noexcept;
	const Layout::PadProxy<Output>&						getOutput() const noexcept;

private:
	std::reference_wrapper<Layout::PadProxy<Output>>	m_output;

};


//Magic operator in order to concatenate
template <typename Tout>
void operator<<(Layout::PadProxy<Input<Tout>>& dst, SourceLayout<Tout>& src) noexcept;

//template <typename Tout>
//void operator<<(ConsumerLayout<Tout>& dst, SourceLayout<Tout>& src); //Already defined

}

#include "SourceLayout.inl"