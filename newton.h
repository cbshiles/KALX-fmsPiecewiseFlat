// newton.h - self containted 1d root finding using the Newton method
// Copyright (c) 2013 KALX, LLC. All rights reserved.
#pragma once
#include <limits>

namespace root1d {

	template<class T, class F, class dF>
	inline T newton(T x, const F& f, const dF& df, size_t iter = -1)
	{
		T fx = f(x);
		T dfx = df(x);

		while (iter-- && 1 + fx != 1 && 1 + dfx != 1) {
			T x_ = x - fx/dfx;
			if (x_ == x)
				break;
			x = x_;
			fx = f(x);
			dfx = df(x);
		} 

		return (iter && 1 + dfx != 1) ? x : std::numeric_limits<T>::quiet_NaN();
	}

} // namespace root1d
