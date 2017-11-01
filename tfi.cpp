// tfi.cpp - test fi.h 
#include "ensure.h"
#include "fi.h"

using namespace functional;
using namespace fi;

double one(double) { return 1.; }
template<class T>
inline T two(T) { return 2; }

template<class T>
void
test_functional(void)
{
	T t[] = {1, 2, 3};

	auto i = apply([](T x) { return x*x; }, t);
	ensure (*i == 1);
	ensure (*++i == 4);
	ensure (*++i == 9);
	ensure (i == t + 2);

	auto j = apply([](T x) { return x*x; }, t);
	ensure (*j++ == 1);
	ensure (*j++ == 4);
	ensure (*j++ == 9);
	ensure (j == t + 3);

	T* pt = t;
	for (auto k = apply([](T x) { return x*x; }, t); k != t + 3; ++k, ++pt)
		ensure (*k == (*pt)*(*pt));

	auto f0 = constant<T>(2);
	ensure (f0(0) == 2);
	
	auto f1 = identity<T>();
	ensure (f1(0) == 0);
	ensure (f1(1) == 1);

	auto f2 = constant<T>(1) + constant<T>(2)*identity<T>();
	ensure (f2(3) == 1 + 2*3);
	// domain_max = <T>max()
	ensure (extrapolate(f2, constant<T>(5))(0) == f2(0)); 
}

template<class T>
void
test_piecewise_constant(void)
{
	T t[] = {1,2,3};
	T f[] = {.1,.2,.3};

	piecewise_constant<T> F(3, t, f);
	ensure (F(t[0]) == f[0]);
	ensure (F(t[1]) == f[1]);
	ensure (F(t[2]) == f[2]);
	ensure (_isnan(F(4)));
	ensure (F(-1) == f[0]);

	piecewise_constant<T> F1(F);
	ensure (F1(t[0]) == f[0]);
	ensure (F1(t[1]) == f[1]);
	ensure (F1(t[2]) == f[2]);
	ensure (_isnan(F1(4)));
	ensure (F1(-1) == f[0]);

	piecewise_constant<T> F2;
	F2 = F1;
	ensure (F2(t[0]) == f[0]);
	ensure (F2(t[1]) == f[1]);
	ensure (F2(t[2]) == f[2]);
	ensure (_isnan(F2(4)));
	ensure (F2(-1) == f[0]);

	F = F2;
	ensure (F(t[0]) == f[0]);
	ensure (F(t[1]) == f[1]);
	ensure (F(t[2]) == f[2]);
	ensure (_isnan(F(4)));
	ensure (F(-1) == f[0]);

	ensure (extrapolate(F, constant<T>(5.))(4) == 5);
}

template<class T>
void
test_fi(void)
{
	std::function<T(T)> f(constant<T>(1.23));
	T x = f(0);
	ensure (x == 1.23);
	f = one;
	ensure (f(0) == 1);
	f = two<T>;
	ensure (f(0) == 2);

	auto s = spot(constant<T>(1.23));
	ensure (s(0) == 1.23);
	ensure (s(1) == 1.23);

	auto d = discount(constant<T>(1.23));

	ensure (d(0) == 1);
	ensure (d(1) == exp(-1.23));

	T t[] = {1,2,3};
	T c[] = {4,5,6};

	T pv0, pv1;
	pv0 = present_value(d)(3,t,c);
	pv1 = present_value(discount(constant<T>(1.23)))(3,t,c);
	ensure (pv0 == pv1);
	pv0 = present_value(discount(one + constant<T>(0.23)))(3,t,c);

	T dur0, dur1;
	dur0 = duration(d,0)(3,t,c);
	dur1 = duration(d)(3,t,c);
	ensure (dur0 == dur1);
	dur0 = duration(d,0.5)(3,t,c);
	ensure (-dur0 < -dur1);
}

void
fms_test_fi(void)
{
	test_functional<float>();
	test_piecewise_constant<double>();
	test_fi<double>();
}