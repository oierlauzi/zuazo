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
	ProcessorLayout(PadProxy<typename ConsumerLayout<Tin>::Input>& input,
					PadProxy<typename SourceLayout<Tout>::Output>& output ) noexcept;
	ProcessorLayout(const ProcessorLayout& other) noexcept = default;
	ProcessorLayout(ProcessorLayout&& other) noexcept = default;
	virtual ~ProcessorLayout() = default;

	ProcessorLayout&				operator=(const ProcessorLayout& other) noexcept = default;
	ProcessorLayout&				operator=(ProcessorLayout&& other) noexcept = default;

};

//Magic operator in order to concatenate
template <typename Tin, typename Tout>
ProcessorLayout<Tin, Tout>& operator<<(PadProxy<Input<Tout>>& dst, ProcessorLayout<Tin, Tout>& src) noexcept;

template <typename Tin, typename Tout>
ProcessorLayout<Tin, Tout>& operator<<(ConsumerLayout<Tout>& dst, ProcessorLayout<Tin, Tout>& src) noexcept;

}

#include "ProcessorLayout.inl"