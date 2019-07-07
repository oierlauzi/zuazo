#define BOOST_TEST_MODULE Utils::Pool
#include <boost/test/included/unit_test.hpp>

#include "../include/Utils/Pool.h"
#include "../include/Math/Vector.h"

using Vec2Pool = Zuazo::Utils::Pool<Zuazo::Math::Vec2f, float, float>;

BOOST_AUTO_TEST_CASE(default_constructor){
    auto pool = Vec2Pool();

    auto vec0 = pool.get(1, 2);
    BOOST_CHECK(vec0->x == 1 && vec0->y == 2);

    vec0.reset();
    vec0 = pool.get(1, 2);
    BOOST_CHECK(vec0->x == 1 && vec0->y == 2);
}