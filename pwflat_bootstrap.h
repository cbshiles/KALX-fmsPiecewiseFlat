// bootstrap.h - bootstrap a piecewise flat forward curve
#pragma once
#include <limits>
#include "secant.h"
#include "valuation.h"
#include "pwflat_forward_curve.h"

namespace fixed_income {
namespace pwflat {

	/// cash deposit.
	template<class IT, class IF>
	inline typename std::iterator_traits<IF>::value_type
	bootstrap(const forward_curve<IT,IF>& f,
		const typename std::iterator_traits<IT>::value_type& t0, 
		const typename std::iterator_traits<IF>::value_type& c0) 
	{
		typedef typename std::iterator_traits<IF>::value_type F;

		if (f.size() == 0)
			return static_cast<F>(log(c0)/t0);

		ensure (t0 > f.last());

		F D = discount(f, f.last());
		F _f = static_cast<F>(log(c0*D)/(t0 - f.last()));

		return _f;
	}

	/// Bootstrap a forward rate agreement.
	template<class IT, class IF>
	inline typename std::iterator_traits<IF>::value_type
	bootstrap(const forward_curve<IT,IF>& f,
		const typename std::iterator_traits<IT>::value_type& t0, 
		const typename std::iterator_traits<IF>::value_type& c0,
		const typename std::iterator_traits<IT>::value_type& t1, 
		const typename std::iterator_traits<IF>::value_type& c1) 
	{
		typedef typename std::iterator_traits<IT>::value_type T;
		typedef typename std::iterator_traits<IF>::value_type F;
		ensure (c0*c1 < 0);

		if (f.size() == 0)
			return static_cast<F>(log(-c1/c0)/(t1 - t0));

		T tm = f.last();

		ensure (t0 < t1);
		ensure (tm < t1);

		F _f;
		F d = -c1/c0; // works if c0 != -1

		if (t0 < tm) { // overlap or cash deposit
			F D0 = discount(f, t0);
			F Dm = discount(f, tm);
			_f = static_cast<F>(log(d*D0/Dm)/(t1 - t0));
		}
		else { // underlap
			_f = static_cast<F>(log(d)/(t1 - tm));
		}

		return _f;
	}

	/// bootstrap a generic set of cash flows
	template<class IT, class IF>
	inline typename std::iterator_traits<IF>::value_type
	bootstrap(const forward_curve<IT,IF>& f, const fixed_income::instrument_base<IT,IF>& i, 
		const typename std::iterator_traits<IF>::value_type& p = 0,  // price
		const typename std::iterator_traits<IF>::value_type& _f = 0,  // initial guess
		const typename std::iterator_traits<IF>::value_type& eps 
			= std::numeric_limits<typename std::iterator_traits<IF>::value_type>::epsilon(), 
		size_t iter = 100,
		const typename std::iterator_traits<IF>::value_type& df = 101/100.) // bump multiplier for secant method
	{
		typedef typename std::iterator_traits<IT>::value_type T;
		typedef typename std::iterator_traits<IF>::value_type F;

//		ensure (i.size() > 2); // othewise use routines above

		// using one dimensional root finding.
		F f0 = _f ? _f : f.back();

		// 1-d root finding
		F p0 = fixed_income::present_value(f.extrapolate(f0), i) - p;
		if (fabs(p0) < eps)
			return f0;

		// bump forward in the right direction
		F f1 = f0*(p0 > 0 ? df : 1/df);
		if (f1 == f0)
			f1 += (df - 1);

		auto pv = [=](F f_) -> F { return fixed_income::present_value(f.extrapolate(f_), i) - p; };
		root1d::secant<decltype(pv), F, F> rs(pv, eps, iter);
		rs.init(f0, f1);

		return rs.root();
	}

} // namespace pwflat
} // namespace fixed_income
