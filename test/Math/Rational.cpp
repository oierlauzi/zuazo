#define BOOST_TEST_MODULE Math::Rational
#include <boost/test/included/unit_test.hpp>
#include <Math/Rational.h>

#include <limits>
#include <cmath>

using Rational = Zuazo::Math::Rational<>;

/*
 * Constructor
 */

BOOST_AUTO_TEST_CASE(default_constructor){
    Rational rat;

    BOOST_CHECK(rat.getNumerator() == 0);
    BOOST_CHECK(rat.getDenominator() == 1);
}

BOOST_AUTO_TEST_CASE(partwise_constructor){
    Rational rat(6, 12); // 6 / 12 = 1 / 2
    
    BOOST_CHECK(rat.getNumerator() == 1);
    BOOST_CHECK(rat.getDenominator() == 2);
}

BOOST_AUTO_TEST_CASE(integer_constructor){
    Rational rat(3L);
    
    BOOST_CHECK(rat.getNumerator() == 3);
    BOOST_CHECK(rat.getDenominator() == 1);
}

BOOST_AUTO_TEST_CASE(fp_constructor_with_finite_decimals){
    Rational rat(2.5); // 2.5 = 25 / 10 = 5 / 2
    
    BOOST_CHECK(rat.getNumerator() == 5);
    BOOST_CHECK(rat.getDenominator() == 2);
}

BOOST_AUTO_TEST_CASE(fp_constructor_with_infinite_decimals){
    Rational rat(1.0 / 3);
    
    BOOST_CHECK(rat.getNumerator() == 1);
    BOOST_CHECK(rat.getDenominator() == 3);
}

BOOST_AUTO_TEST_CASE(fp_constructor_with_pinfinite){
    Rational rat(+std::numeric_limits<Rational::Real>::infinity());
    
    BOOST_CHECK(rat.getNumerator() == +1);
    BOOST_CHECK(rat.getDenominator() == 0);
}

BOOST_AUTO_TEST_CASE(fp_constructor_with_ninfinite){
    Rational rat(-std::numeric_limits<Rational::Real>::infinity());
    
    BOOST_CHECK(rat.getNumerator() == -1);
    BOOST_CHECK(rat.getDenominator() == 0);
}

BOOST_AUTO_TEST_CASE(fp_constructor_with_nan){
    Rational rat(NAN);
    
    BOOST_CHECK(rat.getNumerator() == 0);
    BOOST_CHECK(rat.getDenominator() == 0);
}

/*
 * Cast operators
 */

BOOST_AUTO_TEST_CASE(cast_zero){
    Rational rat(0, 1);

    BOOST_CHECK(static_cast<bool>(rat) == false);
    BOOST_CHECK(static_cast<Rational::Integer>(rat) == 0);
    BOOST_CHECK(static_cast<Rational::Real>(rat) == 0.0);
}

BOOST_AUTO_TEST_CASE(cast_integer){
    Rational rat(1, 1);

    BOOST_CHECK(static_cast<bool>(rat) == true);
    BOOST_CHECK(static_cast<Rational::Integer>(rat) == 1);
    BOOST_CHECK(static_cast<Rational::Real>(rat) == 1.0);
}

BOOST_AUTO_TEST_CASE(cast_real){
    Rational rat(1, 2);

    BOOST_CHECK(static_cast<bool>(rat) == true);
    BOOST_CHECK(static_cast<Rational::Integer>(rat) == 0);
    BOOST_CHECK(static_cast<Rational::Real>(rat) == 0.5);
}


BOOST_AUTO_TEST_CASE(cast_pinf){
    Rational rat(+1, 0);

    BOOST_CHECK(static_cast<bool>(rat) == true);
    BOOST_CHECK(static_cast<Rational::Integer>(rat) == std::numeric_limits<Rational::Integer>::max());
    BOOST_CHECK(static_cast<Rational::Real>(rat) == +std::numeric_limits<Rational::Real>::infinity());
}

BOOST_AUTO_TEST_CASE(cast_ninf){
    Rational rat(-1, 0);

    BOOST_CHECK(static_cast<bool>(rat) == true);
    BOOST_CHECK(static_cast<Rational::Integer>(rat) == std::numeric_limits<Rational::Integer>::min());
    BOOST_CHECK(static_cast<Rational::Real>(rat) == -std::numeric_limits<Rational::Real>::infinity());
}

BOOST_AUTO_TEST_CASE(cast_nan){
    Rational rat(0, 0);

    BOOST_CHECK(static_cast<bool>(rat) == false);
    BOOST_CHECK(static_cast<Rational::Integer>(rat) == 0);
    BOOST_CHECK(std::isnan(static_cast<Rational::Real>(rat)));
}

/*
 * Arithmetic operators
 */

BOOST_AUTO_TEST_CASE(plus_operator){
    BOOST_CHECK( Rational(1, 2) + Rational(1, 8) ==  Rational(5, 8) );
}

BOOST_AUTO_TEST_CASE(minus_operator){
    BOOST_CHECK( Rational(1, 2) - Rational(1, 3) ==  Rational(1, 6) );
}

BOOST_AUTO_TEST_CASE(multiply_operator){
    BOOST_CHECK( Rational(1, 2) * Rational(5, 3) ==  Rational(5, 6) );
}

BOOST_AUTO_TEST_CASE(divide_operator){
    BOOST_CHECK( Rational(1, 2) / Rational(5, 3) ==  Rational(3, 10) );
}

BOOST_AUTO_TEST_CASE(negative_operator){
    BOOST_CHECK( -Rational(1, 1) ==  Rational(-1, 1) );
}

BOOST_AUTO_TEST_CASE(increment_operator){
    Rational rat(1, 1);
    rat += Rational(1, 2);
    BOOST_CHECK( rat ==  Rational(3, 2) );
}

BOOST_AUTO_TEST_CASE(decrement_operator){
    Rational rat(1, 1);
    rat -= Rational(1, 2);
    BOOST_CHECK( rat ==  Rational(1, 2) );
}

BOOST_AUTO_TEST_CASE(multiply_eq_operator){
    Rational rat(1, 1);
    rat *= Rational(1, 2);
    BOOST_CHECK( rat ==  Rational(1, 2) );
}

BOOST_AUTO_TEST_CASE(divide_eq_operator){
    Rational rat(1, 1);
    rat /= Rational(1, 2);
    BOOST_CHECK( rat ==  Rational(2, 1) );
}

/*
 * Comparison operators
 */

BOOST_AUTO_TEST_CASE(eq_operator){
    BOOST_CHECK( Rational(2, 1) ==  Rational(4, 2) );
}

BOOST_AUTO_TEST_CASE(neq_operator){
    BOOST_CHECK( Rational(3, 1) !=  Rational(4, 1) );
    BOOST_CHECK( Rational(1, 3) !=  Rational(1, 4) );
}

BOOST_AUTO_TEST_CASE(gt_operator){
    BOOST_CHECK( Rational(1, 3) >  Rational(1, 4) );
    BOOST_CHECK( !(Rational(1, 3) >  Rational(1, 3)) );
    BOOST_CHECK( !(Rational(1, 3) >  Rational(1, 2)) );
}

BOOST_AUTO_TEST_CASE(ge_operator){
    BOOST_CHECK( Rational(1, 3) >=  Rational(1, 4) );
    BOOST_CHECK( Rational(1, 3) >=  Rational(1, 3) );
    BOOST_CHECK( !(Rational(1, 3) >=  Rational(1, 2)) );
}

BOOST_AUTO_TEST_CASE(lt_operator){
    BOOST_CHECK( !(Rational(1, 3) <  Rational(1, 4)) );
    BOOST_CHECK( !(Rational(1, 3) <  Rational(1, 3)) );
    BOOST_CHECK( Rational(1, 3) <  Rational(1, 2) );
}

BOOST_AUTO_TEST_CASE(le_operator){
    BOOST_CHECK( !(Rational(1, 3) <=  Rational(1, 4)) );
    BOOST_CHECK( Rational(1, 3) <=  Rational(1, 3) );
    BOOST_CHECK( Rational(1, 3) <=  Rational(1, 2) );
}
