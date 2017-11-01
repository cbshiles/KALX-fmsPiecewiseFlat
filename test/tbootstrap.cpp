// tbootstrap.cpp - test bootstrap routines
#include <cassert>
#include <cmath>
#include <vector>
#define ensure(x) assert(x)
#include "../bootstrap.h"
//#include "../instrument.h"

using namespace fixed_income;
using namespace pwflat;

template<class T>
void
test_bootstrap(void)
{
	T eps = std::numeric_limits<T>::epsilon();
	T t[] = {1, 2, 3, 4};
//	size_t n = sizeof(t)/sizeof(*t);

	std::vector<T> f;

	// present_value of (0, -1), (1, e), ..., (n, 1+e) is 0 for f(t) = f0.
	T f0 = (T)0.04;
	T e = exp(f0) - 1;

	// cd with empty forward curve
	f.push_back(bootstrap1<T>(1., 1 + e, forward_curve<T>()));
	ensure (fabs(f.back() - f0) < eps);

	// fra
	f.push_back(bootstrap2<T>(1., -1., 2., 1 + e, forward_curve<T>(1, t, &f[0])));
	ensure (fabs(f.back() - f0) < eps);

	// generic
	T u[]  = {0, 1, 2, 3, 4};
	T c3[] = {-1, e, e, 1 + e};
	// initial guess 0.02
	f.push_back(bootstrap(instrument<T>(4, u, c3), forward_curve<T>(2, t, &f[0]), (T)0.02)); 
	ensure (fabs(f.back() - f0) < eps);

	T c4[] = {-1, e, e, e, 1 + e};
	f.push_back(bootstrap(instrument<T>(5, u, c4), forward_curve<T>(3, t, &f[0]), (T).02)); 
	ensure (fabs(f.back() - f0) < eps);
}

void
fms_test_bootstrap(void)
{
	test_bootstrap<double>();
	test_bootstrap<float>();
}
