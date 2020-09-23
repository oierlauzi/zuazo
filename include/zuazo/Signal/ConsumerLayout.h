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

	explicit ConsumerLayout(Layout::PadProxy<Input>& input);
	ConsumerLayout(const ConsumerLayout& other) = default;
	ConsumerLayout(ConsumerLayout&& other) = default;
	virtual ~ConsumerLayout() = default;

	ConsumerLayout&										operator=(const ConsumerLayout& other) = default;
	ConsumerLayout&										operator=(ConsumerLayout&& other) = default;

	Layout::PadProxy<Input>&							getInput();
	const Layout::PadProxy<Input>&						getInput() const;

private:
	std::reference_wrapper<Layout::PadProxy<Input>>		m_input;

};


//Magic operator in order to concatenate
template <typename Tin>
void operator<<(ConsumerLayout<Tin>& dst, Layout::PadProxy<Output<Tin>>& src);

template <typename Tin>
void operator<<(ConsumerLayout<Tin>& dsr, SourceLayout<Tin>& src);


}

#include "ConsumerLayout.inl"