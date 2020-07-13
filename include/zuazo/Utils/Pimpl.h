#pragma once

#include <memory>

namespace Zuazo::Utils {

template<typename T>
class Pimpl {
public:
    template<typename... Args> 
	Pimpl(Args&&... args);
	Pimpl(const Pimpl& other) = delete;
	Pimpl(Pimpl&& other) = default;
    ~Pimpl() = default;

	Pimpl&				operator=(const Pimpl& other) = delete;
	Pimpl&				operator=(Pimpl&& other) = default;

    T*					operator->();
	const T*			operator->() const;
    T&					operator*();
	const T&			operator*() const;

	operator bool() const;

private:
	std::unique_ptr<T>	m_ptr;
};

}

#include "Pimpl.inl"