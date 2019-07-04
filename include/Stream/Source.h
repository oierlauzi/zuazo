#pragma once

#include <memory>
#include <atomic>

namespace Zuazo::Stream{

template <typename T>
class Source{
public:
	typedef size_t id_t;

	Source() = default;
	Source(const Source& other) = default;
	Source(Source&& other) = default;
	virtual ~Source() = default;

	virtual std::shared_ptr<const T>	get() const;
	id_t								getId() const;
protected:
	void								push() const;
	void								push(const std::shared_ptr<const T>& element) const;
	void								push(std::shared_ptr<const T>&& element) const;

	void								reset();
private:
	mutable std::shared_ptr<const T>	m_last;
	mutable std::shared_ptr<const T>	m_lastId;

	static std::atomic<id_t>			s_id;
};

template <typename T>
class SourcePad :
		public Source<T>
{
public:
	using Source<T>::Source;
	using Source<T>::push;
	using Source<T>::reset;
};

}

#include "Source.inl"
