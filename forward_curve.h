// forward_curve.h - instantaneous forward rate curve
// Copyright (c) 2011-2012 KALX, LLC. All rights reserved. No warranty made.
#pragma once
#include <cmath>
#include <limits>

namespace fixed_income {

	// parameterized by time in years and forward rate
	template<class T = double, class F = double>
	class forward_curve {
	public:
		virtual ~forward_curve() 
		{ }

		// instantaneous forward rate
		virtual F value(const T& d) const = 0;
		F operator()(const T& d) const 
		{ 
			return value(d); 
		}
		// int_0^t f(s) ds
		virtual F integral(const T& d) const = 0;
	};

	// global functions
	template<class T, class F>
	inline F discount(const fixed_income::forward_curve<T,F>& f, const T& t)
	{
		return exp(-f.integral(t));
	}

	template<class T, class F>
	inline F spot(const fixed_income::forward_curve<T,F>& f, const T& t)
	{
		return fabs(t) < sqrt(std::numeric_limits<double>::epsilon()) ? f(t) : f.integral(t)/t;
	}

} // namespace fixed_income
