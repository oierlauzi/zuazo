#pragma once

#include "PadBase.h"
#include "Output.h"
#include "NamingConventions.h"
#include "../Utils/ObserverPattern.h"

#include <vector>
#include <string_view>
#include <optional>

namespace Zuazo::Signal {

template <typename T>
class Output;

enum class NoSignalAction {
	RETURN_EMPTY,
	HOLD_LAST
};

class NoSignal {};
constexpr NoSignal noSignal;

template <typename T>
class Input 
	: public PadBase
	, private Utils::Observer
{
public:
	using Source = Output<T>; friend Source;

	explicit Input(std::string name = makeInputName<T>());
	Input(const Input& other) = default;
	Input(Input&& other) = default;
	virtual ~Input() = default;

	Input&						operator=(const Input& other) = default;
	Input&						operator=(Input&& other) = default;

	void						operator<<(NoSignal);
	void						operator<<(Source& src);

	void						setSource(Source* src);
	Source*						getSource() const;

	void						setNoSignalAction(NoSignalAction nsa);
	NoSignalAction				getNoSignalAction() const;

	void						reset();
	const T&					pull() const;
	bool						hasChanged() const;

private:
	mutable T					m_lastElement;
	NoSignalAction				m_onNoSignal;


	const T&					get() const;
	bool						needsToHold(const T& element) const;
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
	using Input<T>::setNoSignalAction;
	using Input<T>::getNoSignalAction;

	void						setSource(Source* src);
	Source*						getSource() const;
	
	void						operator<<(Source& src);

private:
	~PadProxy() = default;

};

}

#include "Input.inl"