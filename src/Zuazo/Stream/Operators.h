#pragma once

namespace Zuazo::Stream{

template <typename T>
class Source;

template <typename T>
class Consumer;

template <typename T>
Consumer<T>& operator<<(Consumer<T>& cons,  Source<T>& src){
	cons.setSource(&src);
	return cons;
}

template <typename T>
Consumer<T>& operator<<(Consumer<T>& cons,  Source<T>* src){
	cons.setSource(src);
	return cons;
}

template <typename T>
Source<T>& operator>>(Source<T>& src,  Consumer<T>& cons){
	cons.setSource(&src);
	return src;
}

}
