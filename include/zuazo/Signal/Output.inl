#include "Output.h"

namespace Zuazo::Signal {

/*
 * Output
 */

template <typename T>
template <typename Str>
inline Output<T>::Output(Str&& name)
	: PadBase(std::forward<Str>(name))
{
}

template <typename T>
inline void Output<T>::reset() {
	m_lastElement = T();
}

template <typename T>
template<typename Q>
void Output<T>::push(Q&& element) {
	m_lastElement = std::forward<Q>(element);
}

template <typename T>
inline const T& Output<T>::get() const {
	return m_lastElement;
}

/*
 * Layout::PadProxy<Output<T>>
 */

template <typename T>
inline Layout::PadProxy<Output<T>>::PadProxy(Output<T>& pad)
	: PadProxy<PadBase>(pad)
{
}

/*
 * getInput(s) methods
 */

template <typename T>
inline std::vector<Layout::PadProxy<Output<T>>> getOutputs(Layout& layout) {
	return layout.getPads<Output<T>>();
}

template <typename T>
inline std::vector<const Layout::PadProxy<Output<T>>> getOutputs(const Layout& layout){
	return layout.getPads<Output<T>>();
}

template <typename T>
inline Layout::PadProxy<Output<T>> getOutput(Layout& layout, std::string_view str) {
	return layout.getPad<Output<T>>(str);
}

template <typename T>
inline const Layout::PadProxy<Output<T>> getOutput(const Layout& layout, std::string_view str) {
	return layout.getPad<Output<T>>(str);
}

}