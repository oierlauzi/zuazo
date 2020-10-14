#pragma once 

#include "Layout.h"
#include "Input.h"
#include "Output.h"
#include "SourceLayout.h"

#include <utility>

namespace Zuazo::Signal {

template <typename Tin>
class ConsumerLayout {
public:
	using InputType = Tin;
	using Input = Signal::Input<InputType>;

	explicit ConsumerLayout(Layout::PadProxy<Input>& input) noexcept;
	ConsumerLayout(const ConsumerLayout& other) noexcept = default;
	ConsumerLayout(ConsumerLayout&& other) noexcept = default;
	virtual ~ConsumerLayout() = default;

	ConsumerLayout&										operator=(const ConsumerLayout& other) noexcept = default;
	ConsumerLayout&										operator=(ConsumerLayout&& other) noexcept = default;

	Layout::PadProxy<Input>&							getInput() noexcept;
	const Layout::PadProxy<Input>&						getInput() const noexcept;

private:
	std::reference_wrapper<Layout::PadProxy<Input>>		m_input;

};


//Magic operator in order to concatenate
template <typename Tin>
void operator<<(ConsumerLayout<Tin>& dst, Layout::PadProxy<Output<Tin>>& src) noexcept;

template <typename Tin>
void operator<<(ConsumerLayout<Tin>& dsr, SourceLayout<Tin>& src) noexcept;


}

#include "ConsumerLayout.inl"