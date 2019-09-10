#pragma once

namespace Zuazo::Utils {

/*
 * Based on:
 * https://medium.com/@snowp/transparent-phantom-types-in-c-de6ac5bed1d1 
 */
template<typename T, typename Tag>
class PhantomClass : T {
public:
    template<class Args...>
    explicit PhantomClass(Args... args) : T(std::forward<Args>(args)...) {}
};

}