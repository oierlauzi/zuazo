#pragma once

#include <sys/types.h>
#include <memory>
#include <mutex>
#include <queue>

#include "../Timing.h"
#include "../Utils/Rational.h"
#include "Source.h"

namespace Zuazo::Stream{

template <typename T>
class LazySource : public Source<T>, public Updateable{
public:
	LazySource()=default;
	LazySource(const LazySource& other)=default;
	virtual ~LazySource()=default;

	virtual std::shared_ptr<const T>	get() const override;
	virtual std::shared_ptr<const T>	get(Timing::TimePoint* ts) const override;
protected:

private:

};

/*
 * METHOD DEFINITIONS
 */

template <typename T>
inline std::shared_ptr<const T>	LazySource<T>::get() const{
	const_cast<LazySource<T>*>(this)->perform();
	return Source<T>::get();
}

template <typename T>
inline std::shared_ptr<const T>	LazySource<T>::get(Timing::TimePoint* ts) const{
	const_cast<LazySource<T>*>(this)->perform();
	return Source<T>::get(ts);
}
}
