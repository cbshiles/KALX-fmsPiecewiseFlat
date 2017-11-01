// interest_rate_swap.h - cash deposit indicative data and cash flows
// Copyright (c) 2011 KALX, LLC. All rights reserved.
#pragma once
#include "../fmsdatetime/datetime.h"
#include "fixed_income.h"

using datetime::date;
using datetime::holiday_calendar;

namespace fixed_income {

	template<class T = double>
	struct interest_rate_swap : public instrument<T,date> {
		std::vector<T> t_;
		std::vector<T> c_;
		// indicative data
		datetime::date eff_;
		int count_; time_unit unit_; // e.g., 10, UNIT_YEARS
		payment_frequency freq_;
		day_count_basis dcb_;
		roll_convention roll_;
		holiday_calendar cal_;
		payment_frequency float_freq_;
		day_count_basis float_dcb_;

/*		// typical cash deposit conventions
		interest_rate_swap()
		:	count_(3), unit_(UNIT_MONTHS), 
			freq_(FREQ_SEMIANNUALLY),
			dcb_(DCB_30U_360), 
			roll_(ROLL_MODIFIED_FOLLOWING), 
			cal_(CALENDAR_NONE),
			float_freq_(FREQ_QUARTERLY),
			float_dcb_(DCB_ACTUAL_360)
		{ }
*/		interest_rate_swap(
			const date& eff,
			int count, time_unit unit,
			payment_frequency freq = FREQ_SEMIANNUALLY,
			day_count_basis dcb = DCB_30U_360,
			roll_convention roll = ROLL_MODIFIED_FOLLOWING,
			holiday_calendar cal = CALENDAR_NONE,
			payment_frequency float_freq = FREQ_QUARTERLY,
			day_count_basis float_dcb= DCB_ACTUAL_360)
		: 
		  eff_(eff), count_(count), unit_(unit), freq_(freq),
		  dcb_(dcb), roll_(roll), cal_(cal),
		  float_freq_(float_freq), float_dcb_(float_dcb)
		{
			ensure (0 < freq_ && freq_ <= FREQ_MONTHLY);
		}
		virtual ~interest_rate_swap()
		{ }

		// create cash flows given settlement date and fixed coupon
		const interest_rate_swap<T>& fix(const date& val, double coupon)
		{
			date mat(eff_);
			mat.incr(count_, unit_);
			mat.adjust(roll_, cal_);

			t_.push_back(eff_.diffyears(val));
			c_.push_back(-1.);

			date d0(eff_);
			for (int i = 1; d0 < mat; ++i) {
				date d1(eff_);
				d1.incr(12*i/freq_, UNIT_MONTHS).adjust(roll_, cal_);
				t_.push_back(d1.diffyears(val));
				c_.push_back(coupon*d1.diff_dcb(d0, dcb_));
				d0 = d1;
			}

			// principal
			if (d0 == mat)
				c_.back() += 1.0;
			else {
				t_.push_back(mat.diffyears(val));
				c_.push_back(1. + coupon*mat.diff_dcb(d0, dcb_));
			}

			set(t_.size(), &t_[0], &c_[0]);

			return *this;
		}
	};

} // namespace instrument
