// forward_rate_agreement.h - cash deposit indicative data and cash flows
// Copyright (c) 2011 KALX, LLC. All rights reserved.
#pragma once
#include "../fmsdatetime/datetime.h"
#include "fixed_income.h"

using datetime::date;
using datetime::holiday_calendar;

namespace fixed_income {

	template<class T = double>
	struct forward_rate_agreement : public instrument<T,date> {
		std::vector<T> t_;
		std::vector<T> c_;
		// indicative data
		date eff_;
		int count_; time_unit unit_; // e.g., 2, UNIT_WEEKS
		day_count_basis dcb_;
		roll_convention roll_;
		holiday_calendar cal_;
		
		// typical cash deposit conventions
/*		forward_rate_agreement()
		:   fixed_income_vector<IF>(2),
			fixed_income<IF,IF>(&t_[0], &t_[0] + t_.size(), &c_[0]),
			count_(3), unit_(UNIT_MONTHS), 
			dcb_(DCB_ACTUAL_360), 
			roll_(ROLL_MODIFIED_FOLLOWING), 
			cal_(CALENDAR_NONE)
		{ }
*/		forward_rate_agreement(
			const date& eff,
			int count, time_unit unit,
			day_count_basis dcb = DCB_ACTUAL_360,
			roll_convention roll = ROLL_MODIFIED_FOLLOWING,
			const holiday_calendar& cal = CALENDAR_NONE)
		:   t_(2), c_(2),
			eff_(eff), count_(count), unit_(unit),
			dcb_(dcb), roll_(roll), cal_(cal)
		{
//			d_[0] = eff;
		}
		virtual ~forward_rate_agreement()
		{ }

		// compiler generated copy constructor, assignment, destructor

		// create cash flows given settlement, effective, and forward rate
		const forward_rate_agreement<T>& fix(const date& val, T forward)
		{
			t_[0] = eff_.diffyears(val);
			ensure(t_[0] >= 0);
			c_[0] = -1;

			datetime::date d(eff_);
			d.incr(count_, unit_).adjust(roll_, cal_);
			t_[1] = d.diffyears(eff_);
			c_[1] = 1 + forward*d.diff_dcb(eff_, dcb_);

			ensure (c_[1] > 0); // otherwise arbitrage exists

			set(2, &t_[0], &c_[0]);

			return *this;
		}

	};

} // namespace instrument


