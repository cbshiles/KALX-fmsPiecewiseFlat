// tforward.cpp - Test pwflat::forward
#include <limits>
#include "../ensure.h"
#include "../bootstrap.h"

#define dimof(x) sizeof(x)/sizeof(*x)

using namespace pwflat;

double eps = std::numeric_limits<double>::epsilon();

void
test_forward_constructors(void)
{
	forward_curve<> F;
	ensure (F.n == 0);
	ensure (F(-1) == 0);
	ensure (F(1) == 0);

	F._f = 2;
	ensure (F(-1) == 2);
	ensure (F(1) == 2);

	forward_curve<> F2(F);
	ensure (F2(-1) == 2);
	ensure (F2(1) == 2);

	F2._f = 3;
	F = F2;
	ensure (F(-1) == 3);
	ensure (F(1) == 3);

	double t[] = {1, 2, 3};
	double f[] = {.1, .2, .3};
	forward_curve<> F3(3, t, f);
	ensure (F3.n == 3);

	forward_curve<> F4(F3);
	ensure (F4.n == 3);

	F3 = F4;
	ensure (F3.n == 3);
}

void
test_forward_extrapolate(void)
{
	double t[] = {1,2,3};
	double f[] = {.1,.2,.3};
	size_t n = dimof(t);

	forward_curve<> F(n, t, f);
	F._f = .4;
	ensure (F(4) == .4);

	F = F._f = .5;
	ensure (F(4) == .5);
}

void
test_forward_value(void)
{
	double t[] = {1,2,3};
	double f[] = {.1,.2,.3};
	size_t n = dimof(t);
	ensure (n == dimof(f));

	forward_curve<> F(n, t, f);

	ensure (f[0] == F(-1));
	ensure (f[0] == F(1));
	ensure (f[1] == F(1.1));
	ensure (f[0] == F(0.9));
	ensure (f[1] == F(2));
	ensure (f[2] == F(3));
	F._f = 1.23;
	ensure (1.23 == F(4));

	ensure (F.n == n);

	for (size_t i = 0; i < n; ++i) {
		ensure (F(t[i]) == f[i]);
	}
}

void
test_forward_integral(void)
{
	double t[] = {1,2,3};
	double f[] = {.1,.2,.3};
	size_t n = dimof(t);
	ensure (n == dimof(f));

	forward_curve<> F(n, t, f);

	ensure (f[0] == F.integral(1));
	ensure (f[0] + f[1]*.5 == F.integral(1.5));
	ensure (f[0] + f[1] == F.integral(2));
	ensure (fabs(f[0] + f[1] + f[2]*.5 - F.integral(2.5)) < eps);
	ensure (fabs(f[0] + f[1] + f[2] - F.integral(3)) < eps);
	ensure (fabs(f[0] + f[1] + f[2] - F.integral(4)) < eps);
	F._f = .4;
	ensure (fabs(f[0] + f[1] + f[2] + .4*.5 - F.integral(3.5)) < eps);
}

void
test_forward_global(void)
{
	double f(0.1);
	forward_curve<> F(f); // constant f
	ensure (exp(-f*1) == discount(1., F));
	ensure (f == spot(1., F));
}

void
fms_test_forward(void)
{
	test_forward_constructors();
	test_forward_extrapolate();
	test_forward_value();
	test_forward_integral();
	test_forward_global();
}
