#include "Delay.h"

namespace Zuazo::Signal {

template<typename T>
inline Delay<T>::Delay(std::string name, Duration delay)
	: BinomialLayout<T>(std::move(name))
	, m_delay(delay)
{
}



template<typename T>
inline void Delay<T>::setDelay(Duration delay) {
	m_delay = delay;
}

template<typename T>
inline Duration Delay<T>::getDelay() const {
	return m_delay;
}



template<typename T>
inline void Delay<T>::update(TimePoint now) {
	//Push a new element from the input into the delay line
	m_delayLine.push_back(
		Packet{
			now, 
			BinomialLayout<T>::getInputPad().pull()
		}
	);

	//Push all elements older than delay into the output. Only the last one will be preserved
	while(m_delayLine.size() > 0) {
		if(m_delayLine.front().time + m_delay <= now) {
			BinomialLayout<T>::getOutputPad().push(m_delayLine.front().data);
			m_delayLine.pop_front();
		} else {
			break;
		}
	}
}

}