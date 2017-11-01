// fixed_income.h - fixed income routines
// Copyright (c) 2013 KALX, LLC. All rights reserved. No warranty made.
#pragma once

namespace fixed_income {

	template<class T = double, class D = void*>
	struct instrument {
		size_t n;
		const T* t;
		const T* c;
		instrument(size_t n_ = 0, const T* t_ = 0, const T* c_ = 0)
			: n(n_), t(t_), c(c_)
		{ }
		virtual ~instrument()
		{ }
		const instrument<T,D>& set(size_t n_, const T* t_, const T* c_)
		{
			n = n_;
			t = t_;
			c = c_;

			return *this;
		}
		// typically valuation date and coupon
		virtual const instrument& fix(const D&, T)
		{
			return *this;
		}
	};
	// c[0] is coupon, c[i] is daycount fraction for i > 0
	template<class T = double, class D = void*>
	struct fixed_leg : public instrument<T,D> {
		fixed_leg(size_t m, const T* t_, const T* c_)
			: instrument<T,D>(m, t_, c_)
		{
		}
	};
	template<class T = double, class D = void*>
	struct float_leg : public instrument <T,D> {
		float_leg(size_t m, const T* t_)
			: instrument<T,D>(m, t_, 0)
		{
		}
	};
} // namespace fixed_income
