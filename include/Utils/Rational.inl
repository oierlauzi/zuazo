namespace Zuazo::Utils{

/*
 * Constructors
 */

constexpr Rational::Rational() :
    m_num(0),
    m_den(0)
{
}

constexpr Rational::Rational(Integer num, Integer den) :
    m_num(num),
    m_den(den)
{
    simplify();
}

constexpr Rational::Rational(Integer number) :
    m_num(number),
    m_den(1)
{
}

constexpr Rational::Rational(Real number) : Rational() {
	/*
	 * This code is based on:
	 * https://rosettacode.org/wiki/Convert_decimal_number_to_rational
	 */

	const size_t precision=64;
	const int32_t MAX_DENOMINATOR=4096;

	Integer h[3] = { 0, 1, 0 }, k[3] = { 1, 0, 0 };
	Integer n=1, d=0;

	bool neg = false;
	if (number < 0) {
		neg = true;
		number = -number;
	}

	while (number != floor(number)) {
		n <<= 1;
		number *= 2;
	}

	d = number;

	for (u_int32_t i = 0; i < precision; i++) {
		Integer a = n ? d / n : 0;
		if (i && !a)
			break;

		Integer x = d;
		d = n;
		n = x % n;
		x = a;

		if (k[1] * a + k[0] >= MAX_DENOMINATOR) {
			x = (MAX_DENOMINATOR - k[0]) / k[1];
			if (x * 2 >= a || k[1] >= MAX_DENOMINATOR)
				i = precision+1;
			else
				break;
		}

		h[2] = x * h[1] + h[0]; h[0] = h[1]; h[1] = h[2];
		k[2] = x * k[1] + k[0]; k[0] = k[1]; k[1] = k[2];
	}

	m_den = k[1];
	m_num = neg ? -h[1] : h[1];
}

constexpr Rational::Integer Rational::getNumerator() const{
    return m_num;
}

constexpr Rational::Integer Rational::getDenominator() const{
    return m_den;
}

/*
 * Casting operators
 */

constexpr Rational::operator bool() const{
    return m_num;
}

constexpr Rational::operator Integer() const{
    Integer result(0);

    if(m_den)           result = m_num / m_den;
    else if(m_num > 0)  result = std::numeric_limits<Integer>::max();
    else if(m_num < 0)  result = std::numeric_limits<Integer>::min();
    //else              result = 0;

    return result;
}

constexpr Rational::operator Real() const{
    return static_cast<Real>(m_num) / m_den; 
}


/*
 * Arithmetic operators
 */

constexpr Rational operator+(const Rational& left, const Rational& right){
    return Rational(
        left.m_num * right.m_den + right.m_num * left.m_den,
        left.m_den * right.m_den
    );
}

constexpr Rational operator-(const Rational& left, const Rational& right){
    return Rational(
        left.m_num * right.m_den - right.m_num * left.m_den,
        left.m_den * right.m_den
    );
}

constexpr Rational operator*(const Rational& left, const Rational& right){
    return Rational(
        left.m_num * right.m_num,
        left.m_den * right.m_den
    );
}

constexpr Rational operator/(const Rational& left, const Rational& right){
    return Rational(
        left.m_num * right.m_den,
        right.m_num * left.m_den
    );
}

constexpr Rational Rational::operator-(){
    return Rational(-m_num, m_den);
}

constexpr Rational& Rational::operator+=(const Rational& right){
    m_num *= right.m_den;
    m_num += right.m_num * left.m_den;
    m_den *= right.m_den;

    simplify();
    return (*this);
}

constexpr Rational& Rational::operator-=(const Rational& right){
    m_num *= right.m_den;
    m_num -= right.m_num * left.m_den;
    m_den *= right.m_den;

    simplify();
    return (*this);
}

constexpr Rational& Rational::operator*=(const Rational& right){
    m_num *= right.m_num;
    m_den *= right.m_den;

    simplify();
    return (*this);
}

constexpr Rational& Rational::operator/=(const Rational& right){
    m_num *= right.m_den;
    m_den *= right.m_num;

    simplify();
    return (*this);
}

/*
 * Comparison operators
 */

constexpr int operator==(const Rational& left, const Rational& right){
    return left.m_num == right.m_num && left.m_den == right.m_den;
}

constexpr int operator!=(const Rational& left, const Rational& right){
    return left.m_num != right.m_num || left.m_den != right.m_den;
}

constexpr int operator<(const Rational& left, const Rational& right){
    return left.m_num * right.m_den < right.m_num * left.m_den;
}

constexpr int operator<=(const Rational& left, const Rational& right){
    return left.m_num * right.m_den <= right.m_num * left.m_den;
}

constexpr int operator>(const Rational& left, const Rational& right){
    return left.m_num * right.m_den > right.m_num * left.m_den;
}

constexpr int operator>=(const Rational& left, const Rational& right){
    return left.m_num * right.m_den >= right.m_num * left.m_den;
}

constexpr void Rational::simplify(){
    //Take out the common factor
	Integer gcd=std::gcd(num, den);
	if(gcd){
		m_num/=gcd;
		m_den/=gcd;
	}

    //Only numerator should be negative
    if(m_den < 0){
        m_num = -m_num;
        m_den = -m_den;
    }
}

}