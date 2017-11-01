// eurodollar_futures.h - Eurodollar futures contracts.
// Copyright (c) 2011 KALX, LLC. All rights reserved.
// See http://cme.com/
#pragma once
#include "../fmsdatetime/calendar.h"
#include "forward_rate_agreement.h"

namespace fixed_income {

	class eurodollar_futures : public forward_rate_agreement {
	protected:
		unsigned int ordinal_;
	public:
		// CME Group eurodollar futures 
		eurodollar_futures(unsigned int ordinal)
			: ordinal_(ordinal), forward_rate_agreement(3, datetime::UNIT_MONTHS, datetime::DCB_ACTUAL_360, datetime::ROLL_MODIFIED_FOLLOWING, datetime::CALENDAR_CMM)
		{
			ensure (ordinal > 0);
		}

		eurodollar_futures& fix(const datetime::date& set, double price) //???include convexity???
		{
			// convexity???
			int y, m, d;
			set.localtime(&y, &m, &d);

			// round up to month divisible by 3
			datetime::date eff(y, 3*(1 + (m - 1)/3), 15, 0, 0, 0); // 11AM???
			eff.imm(3, datetime::DAY_WED);

			// contracts start trading 2 days before Last Trading Day of prior contract
			if (set < datetime::date(eff).incr(-2, datetime::UNIT_DAYS, datetime::CALENDAR_CMM)) {
				// eff is front contract
				if (ordinal_ > 1) {
					eff.incr(3*(ordinal_ - 1), datetime::UNIT_MONTHS);
					eff.imm(3, datetime::DAY_WED);
				}
			}
			else {
				// eff is previous contract LastTradingDay
				eff.incr(3*ordinal_, datetime::UNIT_MONTHS);
				eff.imm(3, datetime::DAY_WED);
			}

			forward_rate_agreement::valuation(set, eff, 1 - price/100);

			return *this;
		}
	};

} // namespace instrument