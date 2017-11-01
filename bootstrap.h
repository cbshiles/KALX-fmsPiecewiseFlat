// bootstrap.h - bootstrap a piecewise-flat forward curve
// Copyright (c) 2013 KALX, LLC. All rights reserved.
#pragma once
#include "ensure.h"
#include "fixed_income.h"
#include "newton.h"
#include "pwflat.h"

namespace pwflat {

	// cash deposit
	template<class T>
	inline T bootstrap1(T u, T c, size_t n, const T* t, const T* f)
	{
		ensure (u > 0);
		ensure (c > 0);

		T t0 = n ? t[n-1] : 0;
		T D0 = discount(t0, n, t, f);

		return log(c*D0)/(u - t0);
	}
	template<class T>
	inline T bootstrap1(T u, T c, const forward_curve<T>& f)
	{
		return bootstrap1(u, c, f.n, f.t, f.f);
	}

	// forward rate agreement
	template<class T>
	inline T bootstrap2(T u0, T c0, T u1, T c1, size_t n, const T* t, const T* f)
	{
		T _f;
		T t0 = n ? t[n-1] : 0;
		T D0 = discount(t0, n, t, f);
		T d = -c1/c0; // works if c0 != -1

		if (u0 < t0) { // overlap or cash deposit
			T Dm = discount(u1, n, t, f);
			_f = static_cast<T>(log(d*D0/Dm)/(u1 - u0));
		}
		else { // underlap
			_f = static_cast<T>(log(d)/(u1 - t0));
		}

		return _f;
	}
	template<class T>
	inline T bootstrap2(T u0, T c0, T u1, T c1, const forward_curve<T>& f)
	{
		return bootstrap2(u0, c0, u1, c1, f.n, f.t, f.f);
	}

	template<class T>
	inline T bootstrap(size_t m, const T* u, const T* c, size_t n, const T* t, const T* f, T _f = 0, T p = 0)
	{
		ensure (m && (n == 0 || u[m-1] > t[n-1]));

		// cd
		if (m == 1) {
			return bootstrap1<T>(u[0], c[0], n, t, f);
		}

		// fra
		if (m == 2 && p == 0) {
			return bootstrap2<T>(u[0], c[0], u[1], c[1], n, t, f);
		}
		
		T t0 = n ? t[n-1] : 0;
		const T* um = std::upper_bound(u, u + m, t0);
		ensure (um != u + m); // already checked above
		T p0 = present_value<T>(um - u, u, c, n, t, f);

		m -= um - u;
		c += um - u;
		u += um - u;

		auto F = [p,p0,m,u,c,n,t,f](T _f) 
		{ 
			return -p + p0 + present_value<T>(fixed_income::instrument<T>(m, u, c), forward_curve<T>(n, t, f, _f)); 
		};
		auto dF = [t0,m,u,c,n,t,f](T _f) 
		{ 
			return duration<T>(fixed_income::instrument<T>(m, u, c), forward_curve<T>(n, t, f, _f), t0);
		};

		if (_f == 0)
			_f = n ? f[n-1] : static_cast<T>(0.01);

		return root1d::newton(_f, F, dF);
	}
	template<class T>
	inline T bootstrap(const fixed_income::instrument<T>& i, const forward_curve<T>& f, T _f = 0, T p = 0)
	{
		return bootstrap(i.n, i.t, i.c, f.n, f.t, f.f, _f, p);
	}

 } // namespace pwflat