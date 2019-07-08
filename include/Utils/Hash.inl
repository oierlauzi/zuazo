#include <type_traits>
#include <boost/integer.hpp> 
#include "../Math/Funcs.h"

namespace Zuazo::Utils {

template<typename T>
constexpr hash_t Hasher<T>::operator()(const T& h) const {
    if constexpr (std::is_empty<T>::value){
        //Empty object was given
        return 0; 
    } else if constexpr (std::is_integral<T>::value && sizeof(T) <= sizeof(hash_t)){
        //It has a trivial hash
        return static_cast<hash_t>(h);
    } else {
        //Generic hash
        constexpr size_t alignSize = Math::gcd(alignof(T), alignof(hash_t)) * 8;
        typedef typename boost::uint_t<alignSize>::least AlignedType;

        hash_t hash = 0;

        const AlignedType* const start = reinterpret_cast<const AlignedType*>(&h);
        const AlignedType* const end = start + sizeof(T);
        
        for(const AlignedType* ptr = start; ptr < end; ++ptr){
            hash = hashCombine(
                hash, 
                static_cast<hash_t>(*ptr)
            );
        }

        return hash;
    }
}

constexpr hash_t hashCombine(hash_t h1, hash_t h2){
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

}