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

	explicit Output(const Layout& layout, 
					std::string name = std::string(makeOutputName<Element>()), 
					PullCallback pullCbk = {} ) noexcept;
	Output(const Output& other) = default;
	Output(Output&& other) noexcept = default;
	virtual ~Output() = default;

	Output&						operator=(const Output& other) noexcept = default;
	Output&						operator=(Output&& other) noexcept = default;

	Consumers					getConsumers() const;

	PadProxy<Output>&			getProxy() noexcept;
	const PadProxy<Output>&		getProxy() const noexcept;

	void						setPullCallback(PullCallback cbk) noexcept;
	const PullCallback&			getPullCallback() const noexcept;

	void						setMaxRecursion(size_t rec) noexcept;
	size_t						getMaxRecursion() const noexcept;
	size_t						getRecursion() const noexcept;

	void						reset() noexcept;
	void						push(Element element) noexcept;
	const Element&				pull() noexcept;
	const Element&				getLastElement() const noexcept;

	static const Element		NO_SIGNAL;

private:
	PullCallback				m_pullCallback;
	size_t						m_maxRecursion;
	size_t						m_recursion;
	T							m_lastElement;

};



template<typename T>
class PadProxy<Output<T>>
	: private Output<T>
{
	friend Output<T>;
	friend Layout;
public:
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
	using PadBase::getLayout;
	using Output<T>::getName;
	using Output<T>::getLastElement;

	using Output<T>::setMaxRecursion;
	using Output<T>::getMaxRecursion;
	using Output<T>::getRecursion;

	Consumers					getConsumers() const;

private:
	~PadProxy() = default;
	
};

template<typename T>
PadProxy<Output<T>>& getOutput(Layout& layout, std::string_view name = makeOutputName<T>());

template<typename T>
const PadProxy<Output<T>>& getOutput(const Layout& layout, std::string_view name = makeOutputName<T>());

}

#include "Output.inl"