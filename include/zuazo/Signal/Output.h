#pragma once

#include "PadBase.h"
#include "NamingConventions.h"
#include "../Utils/ObserverPattern.h"

#include <functional>
#include <vector>

namespace Zuazo::Signal {

template <typename T>
class Input;

template <typename T>
class Output 
	: public PadBase
	, private Utils::Subject
{
public:
	using Element = T;
	using Consumer = Input<Element>; friend Consumer;
	using Consumers = std::vector<std::reference_wrapper<Consumer>>;

	using PullCallback = std::function<void(Output<T>&)>;

	explicit Output(std::string name = std::string(makeOutputName<Element>()), PullCallback pullCbk = {});
	Output(const Output& other) = default;
	Output(Output&& other) = default;
	virtual ~Output() = default;

	Output&						operator=(const Output& other) = default;
	Output&						operator=(Output&& other) = default;

	Consumers					getConsumers() const;

	void						setPullCallback(PullCallback cbk);
	const PullCallback&			getPullCallback() const;

	void						reset();
	void						push(Element element); ///< \note This function MUST NOT be called from the pull callback
	const Element&				pull();
	const Element&				getLastElement() const;

	static const Element		NO_SIGNAL;

private:
	PullCallback				m_pullCallback;
	T							m_lastElement;

};

template<typename T>
class Layout::PadProxy<Output<T>>
	: Output<T>
{
public:
	friend Layout;

	using Consumer = PadProxy<Input<T>>; friend Consumer;
	using Consumers = std::vector<std::reference_wrapper<Consumer>>;

	PadProxy() = delete;
	PadProxy(const PadProxy& other) = delete;

	PadProxy& 					operator=(const PadProxy& other) = delete;

	using Output<T>::operator==;
	using Output<T>::operator!=;
	using Output<T>::operator<;
	using Output<T>::operator<=;
	using Output<T>::operator>;
	using Output<T>::operator>=;
	using Output<T>::getName;

	Consumers					getConsumers() const;

private:
	~PadProxy() = default;
	
};

template<typename T>
Layout::PadProxy<Output<T>>& getOutput(Layout& layout, std::string_view name = makeOutputName<T>());

template<typename T>
const Layout::PadProxy<Output<T>>& getOutput(const Layout& layout, std::string_view name = makeOutputName<T>());

}

#include "Output.inl"