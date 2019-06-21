namespace Zuazo::Time {

constexpr Duration rate_2_period(const Math::Rational& rate){
    return Duration(static_cast<Rep>( rate / CLOCK_RESOLUTION ));
}

constexpr Math::Rational period_2_rate(Duration dur){
    return 1 / (CLOCK_RESOLUTION * dur.count();
}

}