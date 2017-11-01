// pwflat.h - piecewise flat curve
// Copyright (c) 2013 KALX, LLC. All rights reserved.
#pragma once
#include <algorithm>
#include <functional>
#include <limits>
#include "fixed_income.h"

namespace pwflat {

	// f(u) = f[i], t[i-1] < u <= t[i]; f(u) = _f, u > t[n-1]
	template<class T = double>
	struct forward_curve {
		size_t n;
		const T* t;
		const T* f;
		T _f; // extrapolate
		forward_curve(T f)
			: n(0), t(0), f(0), _f(f)
		{ }
		forward_curve(size_t n_ = 0, const T* t_ = 0, const T* f_ = 0, T _f_ = 0)
			: n(n_), t(t_), f(f_), _f(_f_)
		{ }
		virtual ~forward_curve()
		{ }
		void set(size_t n_, const T* t_, const T* f_, T _f_ = 0)
		{
			n = n_;
			t = t_;
			f = f_;
			_f = _f_;
		}
		T operator()(T u) const
		{
			return pwflat::value(u, n, t, f, _f);
		}
		T value(T u) const
		{
			return pwflat::value(u, n, t, f, _f);
		}
		T integral(T u) const
		{
			return pwflat::integral(u, n, t, f, _f);
		}
	};

	// note value(t[i]) = f[i]
	template<class T>
	inline T value(T u, size_t n, const T* t, const T* f, T _f = 0)
	{
		const T* ti = std::lower_bound(t, t + n, u); // left continuous

		return ti != t + n ? f[ti - t] : _f;
	}
	template<class T>
	inline T value(T u, const forward_curve<T>& f)
	{
		return value(u, f.n, f.t, f.f, f._f);
	}

	// int_0^u f(s) ds
	template<class T>
	inline T integral(T u, size_t n, const T* t, const T* f, T _f = 0)
	{
		T I(0), t0(0);

		while (n-- && *t <= u) {
			I += *f++ * (*t - t0);
			t0 = *t++;
		}

		I += (n != -1 ? *f : _f)*(u - t0);

		return I;
	}
	template<class T>
	inline T integral(T u, const forward_curve<T>& f)
	{
		return f.integral(u);
	}

	template<class T>
	inline T discount(T u, size_t n, const T* t, const T* f, T _f = 0)
	{
		return exp(-integral(u, n, t, f, _f));
	}
	template<class T>
	inline T discount(T u, const forward_curve<T>& f)
	{
		return discount(u, f.n, f.t, f.f, f._f);
	}

	template<class T>
	inline T spot(T u, size_t n, const T* t, const T* f, T _f = 0)
	{
		return 1 + u == 1 ? (n == 0 ? _f : f[0]) : integral(u, n, t, f, _f)/u;
	}
	template<class T>
	inline T spot(T u, const forward_curve<T>& f)
	{
		return spot(u, f.n, f.t, f.f, f._f);
	}

	template<class T>
	inline T present_value(size_t m, const T* u, const T* c, size_t n, const T* t, const T* f, T _f = 0)
	{
		T pv(0);

		while (m--) {
			pv += *c++ * discount(*u++, n, t, f, _f);
		}

		return pv;
	}
	template<class T>
	inline T present_value(const fixed_income::instrument<T>& i, const forward_curve<T>& f)
	{
		return present_value(i.n, i.t, i.c, f.n, f.t, f.f, f._f);
	}
	// present value given fractional recovery R and survival S(t) = P(T > t)
	template<class T>
	inline T present_value(const fixed_income::instrument<T>& i, const forward_curve<T>& f, T r, std::function<T(T)> s)
	{
		T pv(0);
		size_t m = i.n;
		const T* u = i.t;
		const T* c = i.c;

		while (m--) {
			pv += *c * discount(*u, f) * (r + (1 - r)*s(*u));
			++u;
			++c;
		}

		return pv;
	}

	// fixed coupon legs
	// assumes c[0] is fixed coupon, c[i] are day count fractions
	template<class T>
	inline T present_value(const fixed_income::fixed_leg<T>& i, const forward_curve<T>& f)
	{
		T pv(0);
		size_t m = i.n;
		const T* u = i.t;
		const T* c = i.c;

		for (size_t i = 1; i < m; ++i) {
			pv += c[0] * c[i] * discount(u[i], f);
		}

		return pv;
	}
	template<class T>
	inline T present_value(const fixed_income::fixed_leg<T>& i, const forward_curve<T>& f, T r, std::function<T(T)> s)
	{
		T pv(0);
		size_t m = i.n;
		const T* u = i.t;
		const T* c = i.c;

		for (size_t i = 1; i < m; ++i) {
			pv += c[0] * c[i] * discount(u[i], f) * (r + (1 - r)*s(u[i]));
		}

		return pv;
	}

	// floating coupon legs
	template<class T>
	inline T present_value(const fixed_income::float_leg<T>& i, const forward_curve<T>& f)
	{
		ensure (i.n > 1);

		return discount(i.t[0], f) - discount(i.t[i.n-1], f);
	}
	template<class T>
	inline T present_value(const fixed_income::float_leg<T>& i, const forward_curve<T>& f, T r, std::function<T(T)> s)
	{
		T pv(0);
		size_t m = i.n;
		const T* u = i.t;

		for (size_t i = 1; i < m; ++i) {
			pv += (discount(u[i-1], f) - discount(u[i], f)) * (r + (1 - r)*s(u[i]));
		}

		return pv;
	}

	// d(pv)/df for parallel shift past u0
	template<class T>
	inline T duration(size_t m, const T* u, const T* c, size_t n, const T* t, const T* f, T _f = 0, T u0 = 0)
	{
		T dur(0);

		while (m && *u <= u0) {
			--m;
			++u;
			++c;
		}
		while (m--) {
			dur += - (*u - u0) * (*c) * discount(*u, n, t, f, _f);
			++u;
			++c;
		}

		return dur;
	}
	template<class T>
	inline T duration(const fixed_income::instrument<T>& i, const forward_curve<T>& f, T u0 = 0)
	{
		return duration(i.n, i.t, i.c, f.n, f.t, f.f, f._f, u0);
	}

} // namespace pwflat