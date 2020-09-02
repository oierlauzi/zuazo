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
	using Element = T;
	using Source = Output<Element>; friend Source;
	using PushCallback = std::function<void(const Element&)>;

	explicit Input(std::string name = std::string(makeInputName<Element>()), PushCallback pushCbk = {});
	Input(const Input& other) = default;
	Input(Input&& other) = default;
	virtual ~Input() = default;

	Input&						operator=(const Input& other) = default;
	Input&						operator=(Input&& other) = default;

	void						operator<<(NoSignal);
	void						operator<<(Source& src);

	void						setSource(Source* src);
	Source*						getSource() const;

	void						setPushCallback(PushCallback cbk);
	const PushCallback&			getPushCallback() const;

	void						setNoSignalAction(NoSignalAction nsa);
	NoSignalAction				getNoSignalAction() const;

	void						reset();
	void						push(const Element& el);
	const Element&				pull(); ///< \note This function MUST NOT be called from the push callback
	const Element&				getLastElement() const; 
	bool						hasChanged() const; ///< \note This function MUST NOT be called from the push callback

private:
	PushCallback				m_pushCallback;
	NoSignalAction				m_onNoSignal;
	Element						m_lastElement;


	const Element&				pullFromSource() const;
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

template<typename T>
Layout::PadProxy<Input<T>>& getInput(Layout& layout, std::string_view name = makeInputName<T>());

template<typename T>
const Layout::PadProxy<Input<T>>& getInput(const Layout& layout, std::string_view name = makeInputName<T>());

}

#include "Input.inl"