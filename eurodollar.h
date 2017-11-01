// eurodollar.h - Eurodollar related functions.
// Copyright (c) 2011 KALX, LLC. All rights reserved.
// See http://cme.com/
#pragma once
#include "../fmsdatetime/datetime.h"

// index of closest contract past given days
inline int 
eurodollar_first_contract(const datetime::date& d, int days)
{
	ensure (days >= 0);
	int ordinal = 1;

	datetime::date d_(d);
	d_ = datetime::date(d_.year(), 3*(1 + (d_.month() - 1)/3), 15);
	d_.imm(3, datetime::DAY_WED);
	if (d_.diffdays(d) < 0) {
		d_.incr(3, datetime::UNIT_MONTH);
		d_.imm(3, datetime::DAY_WED);
	}

	ensure (d_.diffdays(d) > 0);
	
	while (d_.diffdays(d) < days) {
		d_.incr(3, datetime::UNIT_MONTH);
		d_.imm(3, datetime::DAY_WED);
		if (d_.month() % 3 == 0)
			++ordinal;
	}

	return ordinal;
}

