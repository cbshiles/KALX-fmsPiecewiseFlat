// tvaluation.cpp - test valuation routines
#include "../ensure.h"
#include "../bootstrap.h"

#define dimof(x) sizeof(x)/sizeof(*x)

using namespace fixed_income;
using namespace pwflat;

const double eps = std::numeric_limits<double>::epsilon();

void
fms_test_valuation(void)
{
	double t[] = {1, 2, 3};
	double f[] = {0.1, 0.2, 0.3};
	pwflat::forward_curve<> F(3, t, f, 0.4);

	double u[] = {0, 1, 2, 3, 4};
	double c[] = {-1, .1, .1, .1, 1.1};

	double pc0(0), pv0(0), dur0(0);
	for (int i = 0; i < dimof(u); ++i) {
		if (i > 0)
			pc0 += discount(u[i], F);
		pv0 += c[i] * discount(u[i], F);
		dur0 += -u[i]*c[i] * discount(u[i], F);
	}

	double pv = present_value(instrument<>(dimof(u), u, c), F);
	ensure (fabs(pv0 - pv) < eps);

	double dur = duration(instrument<>(dimof(u), u, c), F);
	ensure (fabs(dur0 - dur) < eps);

	double c0 = (discount(u[0], F) - discount(u[4], F))/pc0;
	double c_[5] = {c0, 1, 1, 1, 1};
	ensure (fabs(present_value(fixed_leg<>(5, u, c_), F) - present_value(float_leg<>(5, u), F)) < eps);

	auto S = [](double t) { return exp(-0.01*t); };
	double p = 0;
	for (int i = 0; i < dimof(u); ++i)
		p += c[i] * discount(u[i], F)*0.5*(1 + S(u[i]));

	pv = present_value<double>(instrument<>(5, u, c), F, 0.5, S);
	ensure (fabs(p - pv) < eps);
/*
	c_[0] = c[1];
	double pvi = present_value<double>(fixed_leg<>(5, u, c_), F, 0.5, S);
	double pvl = present_value<double>(float_leg<>(5, u), F, 0.5, S);
	p = pvi - pvl;
	ensure (fabs(pv - (pvi - pvl)) < eps);
*/
}