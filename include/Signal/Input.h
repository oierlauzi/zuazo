#pragma once

#include "Layout.h"
#include "PadBase.h"
#include "Output.h"

#include <vector>
#include <string_view>

namespace Zuazo::Signal {

template <typename T>
class Output;

enum class NoSignalAction {
	RETURN_EMPTY,
	HOLD_LAST
};

template <typename T>
class Input : public PadBase {
	friend Output<T>;
public:
	Input() = default;
	template<typename Str>
	Input(Str&& name);
	Input(const Input& other) = default;
	Input(Input&& other) = default;
	virtual ~Input() = default;

	Input&						operator=(const Input& other);
	Input&						operator=(Input&& other);


	void						setSource(Output<T>* src);
	Output<T>*					getSource() const;

	void						setNoSignalAction(NoSignalAction nsa);
	NoSignalAction				getNoSignalAction() const;

	void						reset();
	const T&					pull() const;
	bool						hasChanged() const;

	static const T				NO_SIGNAL;

private:
	Output<T>*					m_source = nullptr;

	mutable T					m_lastElement;
	NoSignalAction				m_onNoSignal = NoSignalAction::RETURN_EMPTY;

	const T&					get() const;
	bool						needsToHold(const T& element) const;
};

template <typename T>
class Layout::PadProxy<Input<T>> : public Layout::PadProxy<PadBase> {
public:
	PadProxy(Input<T>& pad);
	PadProxy(const PadProxy& other) = delete;
	~PadProxy() = default;

	PadProxy& 					operator=(const PadProxy& other) = delete;

	void						setSource(PadProxy<Output<T>>* src);
	PadProxy<Output<T>>* 		getSource() const;

	void 						operator<<(PadProxy<Output<T>>& src);
	void 						operator<<(NoSignal);

	void						setNoSignalAction(NoSignalAction hold);
	NoSignalAction				getNoSignalAction() const;

};

template <typename T>
std::vector<Layout::PadProxy<Input<T>>> getInputs(Layout& layout);

template <typename T>
std::vector<const Layout::PadProxy<Input<T>>> getInputs(const Layout& layout);

template <typename T>
Layout::PadProxy<Input<T>> getInput(Layout& layout, std::string_view str);

template <typename T>
const Layout::PadProxy<Input<T>> getInput(const Layout& layout, std::string_view str);

}

#include "Input.inl"