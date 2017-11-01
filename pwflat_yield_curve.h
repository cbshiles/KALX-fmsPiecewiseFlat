// pwflat_yield_curve.h - piecewise flat continuously compounded forward bootstrap yield curve
// Copyright (c) 2011-2012 KALX, LLC. All rights reserved. No warranty made.
#pragma once
#include <cmath>
#include <algorithm>
#include <vector>
#include "bootstrap.h"

namespace pwflat {

	/// <summary>Bootstrap a yield curve with piece-wise flat forward rates.</summary>
	template<class T = double>
	class yield_curve {
		std::vector<T> t_;
		std::vector<T> f_;
		void push_back(const T& t, const T& f)
		{
			t_.push_back(t);
			f_.push_back(f);
		}
	public:
		/// <summary>Construct an empty yield curve.</summary>
		yield_curve()
		{ }
		virtual ~yield_curve()
		{ }

		size_t size(void) const
		{
			return t_.size();
		}
		void reset(void)
		{
			t_.resize(0);
			f_.resize(0);
		}
		T maturity(void) const
		{
			return t_.back();
		}

		::pwflat::forward_curve<T> forward_curve() const
		{
			return size() == 0? ::pwflat::forward_curve<T>() : ::pwflat::forward_curve<T>(t_.size(), &t_[0], &f_[0]);
		}

		/// <summary>Add a cash deposit.</summary>
		/// <param name="t0">The maturity of the cash deposit.</param>
		/// <param name="c0">The cash flow at maturity.</param>
		/// <remarks>
		/// A cash deposit is equvalent to a spot starting forward rate agreement
		/// and also to a zero coupon bond. The initial cash flow is assumed
		/// to be -1 at time 0. In general, c0 = 1 + r*dcf, where r is a rate
		/// and dcf is the day count fraction for the period.
		/// </remarks>
		yield_curve& add(T t0, T c0)
		{
			push_back(t0, bootstrap(t0, c0, forward_curve()));

			return *this;
		}
		/// <summary>Add a forward rate agreement.</summary>
		/// <param name="t0">The time of the first cash flow.</param>
		/// <param name="c0">The amount of the first cash flow.</param>
		/// <param name="t1">The time of the last cash flow.</param>
		/// <param name="c1">The amount of the last cash flow.</param>
		/// <remarks>
		/// A forward rate agreement consists of two cash flows having
		/// opposite signs. In general, c0 = -1 and c1 = 1 + f*dcf, where
		/// f is a rate and dcf is the day count fraction for the period
		/// from t0 to t1.
		/// </remarks>
		yield_curve& add(T t0, T c0, T t1, T c1)
		{
			push_back(t1, bootstrap(t0, c0, t1, c1, forward_curve()));

			return *this;
		}
		/// <summary>Add a general cash flow stream to a curve.</summary>
		/// <param name="n">Pointer to the first cash flow time in years.</param>
		/// <param name="tb">Pointer to one past the last cash flow time in years.</param>
		/// <param name="cb">Pointer to the first cash flow amount.</param>
		/// <param name="_f">Optional initial guess for boostrap.</param>
		/// <param name="p">Optional price of instrument. Default is 0.</param>
		yield_curve& add(size_t n, const T* tb, const T* cb, T _f = 0, T p = 0)
		{
 			push_back(tb[n - 1], bootstrap(fixed_income::instrument<T>(n, tb, cb), forward_curve(), _f, p));

			return *this;
		}
		template<class D>
		yield_curve& add(const fixed_income::instrument<T,D>& i, T _f = 0, T p = 0)
		{
			return add(i.n, i.t, i.c, _f, p);
		}
	};

} // namespace pwflat
