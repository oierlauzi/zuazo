#pragma once

#include "PadBase.h"
#include "Output.h"
#include "NamingConventions.h"
#include "../Utils/ObserverPattern.h"

#include <vector>
#include <string_view>
#include <functional>

namespace Zuazo::Signal {

template <typename T>
class Output;

class NoSignal {};
constexpr NoSignal noSignal;

template <typename T>
class Input 
	: public PadBase
	, private Utils::Observer
{
public:
	using Element = T;
	using Source = Output<Element>; friend Source;

	explicit Input(std::string name = std::string(makeInputName<Element>())) noexcept;
	Input(const Input& other) = default;
	Input(Input&& other) noexcept = default;
	virtual ~Input() = default;

	Input&						operator=(const Input& other) noexcept = default;
	Input&						operator=(Input&& other) noexcept = default;

	void						operator<<(NoSignal) noexcept;
	void						operator<<(Source& src) noexcept;

	void						setSource(Source* src) noexcept;
	Source*						getSource() const noexcept;

	void						reset() noexcept;
	const Element&				pull() noexcept;
	const Element&				getLastElement() const noexcept; 
	bool						hasChanged() const noexcept;
	
private:
	Element						m_lastElement;


	const Element&				pullFromSource() const noexcept;
};

template<typename T>
class Layout::PadProxy<Input<T>>
	: Input<T>
{
public:
	friend Layout;

	using Source = PadProxy<Output<T>>; friend Source;

	PadProxy() = delete;
	PadProxy(const PadProxy& other) = delete;

	PadProxy& 					operator=(const PadProxy& other) = delete;

	using Input<T>::operator==;
	using Input<T>::operator!=;
	using Input<T>::operator<;
	using Input<T>::operator<=;
	using Input<T>::operator>;
	using Input<T>::operator>=;
	using Input<T>::operator<<;
	using Input<T>::getName;

	void						setSource(Source* src) noexcept;
	Source*						getSource() const noexcept;
	
	void						operator<<(Source& src) noexcept;

private:
	~PadProxy() = default;

};

template<typename T>
Layout::PadProxy<Input<T>>& getInput(Layout& layout, std::string_view name = makeInputName<T>());

template<typename T>
const Layout::PadProxy<Input<T>>& getInput(const Layout& layout, std::string_view name = makeInputName<T>());

}

#include "Input.inl"