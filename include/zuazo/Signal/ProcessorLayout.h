#pragma once 

#include "ConsumerLayout.h"
#include "SourceLayout.h"

namespace Zuazo::Signal {

template <typename Tin, typename Tout = Tin>
class ProcessorLayout 
	: public ConsumerLayout<Tin>
	, public SourceLayout<Tout>
{
public:
	ProcessorLayout(Layout::PadProxy<typename ConsumerLayout<Tin>::Input>& input,
					Layout::PadProxy<typename SourceLayout<Tout>::Output>& output );
	ProcessorLayout(const ProcessorLayout& other) = default;
	ProcessorLayout(ProcessorLayout&& other) = default;
	virtual ~ProcessorLayout() = default;

	ProcessorLayout&				operator=(const ProcessorLayout& other) = default;
	ProcessorLayout&				operator=(ProcessorLayout&& other) = default;

};

//Magic operator in order to concatenate
template <typename Tin, typename Tout>
ProcessorLayout<Tin, Tout>& operator<<(Layout::PadProxy<Input<Tout>>& dst, ProcessorLayout<Tin, Tout>& src);

template <typename Tin, typename Tout>
ProcessorLayout<Tin, Tout>& operator<<(ConsumerLayout<Tout>& dst, ProcessorLayout<Tin, Tout>& src);

}

#include "ProcessorLayout.inl"