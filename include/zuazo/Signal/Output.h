#pragma once

#include "Layout.h"
#include "PadBase.h"

#include <string_view>

namespace Zuazo::Signal {

template <typename T>
class Input;

template <typename T>
class Output : public PadBase {
	friend Input<T>;
public:
	Output() = default;
	template<typename Str>
	Output(Str&& name);
	Output(const Output& other) = default;
	Output(Output&& other) = default;
	virtual ~Output() = default;

	Output&						operator=(const Output& other) = default;
	Output&						operator=(Output&& other) = default;

	void						reset();
	template<typename Q>
	void						push(Q&& element);

private:
	T							m_lastElement;

	virtual const T&			get() const;
};

template <typename T>
class Layout::PadProxy<Output<T>> : public Layout::PadProxy<PadBase> {
	friend Layout::PadProxy<Input<T>>;
public:
	PadProxy(Output<T>& pad);
	PadProxy(const PadProxy& other) = delete;
	~PadProxy() = default;

	PadProxy& 					operator=(const PadProxy& other) = delete;

};


template <typename T>
std::vector<Layout::PadProxy<Output<T>>> getOutputs(Layout& layout);

template <typename T>
std::vector<const Layout::PadProxy<Output<T>>> getOutputs(const Layout& layout);

template <typename T>
Layout::PadProxy<Output<T>> getOutput(Layout& layout, std::string_view str);

template <typename T>
const Layout::PadProxy<Output<T>> getOutput(const Layout& layout, std::string_view str);

}

#include "Output.inl"