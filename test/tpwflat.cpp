// tpwflat.cpp - test the piecewise flat forward model
// Copyright (c) 2011 KALX, LLC. All rights reserved. No warranty made.
#include "../pwflat_yield_curve.h"
#include "../cash_deposit.h"
#include "../forward_rate_agreement.h"
#include "../interest_rate_swap.h"

#define dimof(x) sizeof(x)/sizeof(*x)

using namespace pwflat;
//using namespace pwflat;

void
test_pwflat_yield_curve(void)
{
	yield_curve<> yc;
	double f = 0.04;
	// pv of (0, -1), (1, e), ..., (n, 1+e) is 0 for f(t) = f.
	double e = exp(f) - 1;
	double t[10], c[10];

	t[0] = 0;
	c[0] = 0;
	for (int i = 1; i < 10; ++i ) {
		c[i - 1] -= 1;
		t[i] = i;
		c[i] = 1 + e;
		yc.add(i + 1, t, c);
	}

	auto fc = yc.forward_curve();
	for (size_t i = 0; i < fc.n; ++i ) {
		ensure (fabs(fc.f[i] - f) < std::numeric_limits<double>::epsilon());
	}

	yc.reset();
	t[0] = 0;
	c[0] = 0;
	for (int i = 1; i < 10; ++i ) {
		c[i - 1] -= 1;
		t[i] = i;
		c[i] = 1 + e;
		yc.add(i + 1, t, c, 0.02);
	}
	fc = yc.forward_curve();
	for (size_t i = 0; i < fc.n; ++i ) {
		ensure (fabs(fc.f[i] - f) < std::numeric_limits<double>::epsilon());
	}

	yc.reset();
	date val(2012, 11, 11);

	fixed_income::cash_deposit<> cd0(0, 1, UNIT_MONTH, DCB_ACTUAL_360, ROLL_MODIFIED_FOLLOWING);
	yc.add(cd0.fix(val, .01));
	yc.add(fixed_income::cash_deposit<>(2, 2, UNIT_MONTH, DCB_ACTUAL_360, ROLL_MODIFIED_FOLLOWING).fix(val, 0.02));

	fixed_income::forward_rate_agreement<> fra0(date(val).incr(2,UNIT_MONTHS), 3, UNIT_MONTHS, DCB_ACTUAL_360, ROLL_MODIFIED_FOLLOWING);
	yc.add(fra0.fix(val, 0.03));

	fixed_income::interest_rate_swap<> irs0(date(val).incr(2,UNIT_DAYS), 1, UNIT_YEAR, FREQ_SEMIANNUALLY, DCB_30U_360, ROLL_MODIFIED_FOLLOWING);
	yc.add(irs0.fix(val, 0.04));

	yield_curve<> yc1;
	yc1.add(cd0)
		.add(fixed_income::cash_deposit<>(2, 2, UNIT_MONTH, DCB_ACTUAL_360, ROLL_MODIFIED_FOLLOWING).fix(val, 0.02))
		.add(fra0)
		.add(irs0);

	forward_curve<> fwd = yc.forward_curve();
	forward_curve<> fwd1 = yc1.forward_curve();

	for (double t = 0; t <= yc.maturity(); t += 0.1)
		ensure (fwd(t) == fwd1(t));

}
/*
void
test_eurodollar_first_contract(void)
{
	datetime::date imm(2011, 9, 1);
		
	imm.imm(3, DAY_WED);
	ensure (imm.day() == 21);

	datetime::date d(imm);
	d.incr(-30, UNIT_DAYS);

	int ordinal;
	ordinal = eurodollar_first_contract(d, 30);
	ensure (ordinal == 1);
	ordinal = eurodollar_first_contract(d, 31);
	ensure (ordinal == 2);
}
*/

void
fms_test_pwflat(void)
{
	test_pwflat_yield_curve();
//	test_eurodollar_first_contract();
}
