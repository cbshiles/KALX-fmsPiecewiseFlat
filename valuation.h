// valuation.h - use forward curves to value intruments
// Copyright (c) 2011-2012 KALX, LLC. All rights reserved.
#pragma once
#include "forward_curve.h"
#include "instrument_base.h"

namespace fixed_income {

	template<class IT, class IF>
	inline typename std::iterator_traits<IF>::value_type 
	present_value(const fixed_income::forward_curve<
			typename std::iterator_traits<IT>::value_type,
			typename std::iterator_traits<IF>::value_type>& f, 
		const fixed_income::instrument_base<IT,IF>& i)
	{
		typename std::iterator_traits<IF>::value_type pv(0);

		for (size_t j = 0; j < i.size(); ++j) {
			pv += i.flow(j) * discount(f, i.time(j));
		}

		return pv;
	}

	// pv given recovery, r, and survival S(t) = P(T > t)
	template<class IT, class IF, class S>
	inline typename std::iterator_traits<IF>::value_type 
	present_value(const fixed_income::forward_curve<
			typename std::iterator_traits<IT>::value_type,
			typename std::iterator_traits<IF>::value_type>& f, 
		const fixed_income::instrument_base<IT,IF>& i,
		typename std::iterator_traits<IF>::value_type r, S s)
	{
		typedef typename std::iterator_traits<IT>::value_type T;
		typedef typename std::iterator_traits<IF>::value_type F;

		F pv(0);

		for (size_t j = 0; j < i.size(); ++j) {
			T tj = i.time(j);

			pv += i.flow(j) * discount(f, tj) * (r + (1 - r)*s(tj));
		}

		return pv;
	}

} // namespace fixed_income
