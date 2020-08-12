#pragma once

#include "PadBase.h"
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
	using Consumer = Input<T>; friend Consumer;
	using Consumers = std::vector<std::reference_wrapper<Consumer>>;
	using PullCallback = std::function<void()>;

	Output(std::string name, PullCallback pullCbk = {});
	Output(const Output& other) = default;
	Output(Output&& other) = default;
	virtual ~Output() = default;

	Output&						operator=(const Output& other) = default;
	Output&						operator=(Output&& other) = default;

	Consumers					getConsumers() const;

	void						reset();
	void						push(T element);
	const T&					pull() const;

	void						setPullCallback(PullCallback cbk);
	const PullCallback&			getPullCallback() const;

private:
	T							m_lastElement;
	PullCallback				m_pullCallback;

};

template<typename T>
struct Layout::PadProxy<Output<T>>
	: private Output<T>
{
	friend Layout;

	using Consumer = PadProxy<Input<T>>; friend Consumer;
	using Consumers = std::vector<std::reference_wrapper<Consumer>>;

	using Output<T>::operator==;
	using Output<T>::operator!=;
	using Output<T>::operator<;
	using Output<T>::operator<=;
	using Output<T>::operator>;
	using Output<T>::operator>=;
	using Output<T>::getName;

	Consumers					getConsumers() const;
};

}

#include "Output.inl"