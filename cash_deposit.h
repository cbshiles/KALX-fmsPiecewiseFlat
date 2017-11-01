// cash_deposit.h - cash deposit indicative data and cash flows
// Copyright (c) 2011 KALX, LLC. All rights reserved.
#pragma once
#include "../fmsdatetime/datetime.h"
#include "fixed_income.h"

using datetime::date;
using datetime::holiday_calendar;

namespace fixed_income {

	template<class T = double>
	class cash_deposit : public instrument<T,date> {
		std::vector<T> t_;
		std::vector<T> c_;
	public:
		// indicative data
		int eff_; // number of days until settlement
		int count_; time_unit unit_; // e.g., 2, UNIT_WEEKS
		day_count_basis dcb_;
		roll_convention roll_;
		holiday_calendar cal_;

		// typical cash deposit conventions
		cash_deposit() 
		:   t_(2), c_(2),
			eff_(2) // T+2
		  	count_(1), unit_(UNIT_DAYS), 
			dcb_(DCB_ACTUAL_360), 
			roll_(ROLL_MODIFIED_FOLLOWING), 
			cal_(0)
		{ }
		cash_deposit(
			int eff,
			int count, time_unit unit,
			day_count_basis dcb = DCB_30U_360,
			roll_convention roll = ROLL_MODIFIED_FOLLOWING,
			const holiday_calendar& cal = CALENDAR_NONE)
		:   t_(2), c_(2),
			eff_(eff), count_(count), unit_(unit),
		    dcb_(dcb), roll_(roll), cal_(cal)
		{
			ensure (count > 0);
		}

		// compiler generated copy constructor, assignment, destructor

		//!!! return fixed_income::instrument
		// create cash flows given valuation and rate
		const cash_deposit& fix(const datetime::date& val, T rate)
		{
			datetime::date d0(val);
			d0.incr(eff_, UNIT_DAYS).adjust(roll_, cal_);
			t_[0] = d0.diffyears(val);
			c_[0] = -1;

			datetime::date d1(val);
			d1.incr(count_, unit_).adjust(roll_, cal_);
			t_[1] = d1.diffyears(val);
			c_[1] = 1 + rate*d1.diff_dcb(d0, dcb_);

			ensure (c_[1] > 0); // otherwise arbitrage exists

			set(2, &t_[0], &c_[0]);

			return *this;
		}

	};

} // namespace fixed_income

