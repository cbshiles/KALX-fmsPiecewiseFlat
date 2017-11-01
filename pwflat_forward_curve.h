// forward.h - piecewise flat forward curves
// Copyright (c) 2012 KALX, LLC. All rights reserved. No warranty made.
#pragma once
#include <algorithm>
#include <iterator>
#include <vector>
#include "forward_curve.h"

namespace fixed_income {
namespace pwflat {

	// base pwflat forward curve for preallocated memory
	template<class IT = double*, class IF = double*>
	class forward_curve 
		: public fixed_income::forward_curve<
			typename std::iterator_traits<IT>::value_type,
			typename std::iterator_traits<IF>::value_type
		> 
	{
	protected:
		typedef typename std::iterator_traits<IT>::value_type T;
		typedef typename std::iterator_traits<IF>::value_type F;

		IT tb_;
		IT te_;
		IF fb_;
		mutable F _f; // extrapolation value
	public:
		forward_curve(const F& f = 0)
			: fixed_income::forward_curve<T,F>(), tb_(0), te_(0), fb_(0), _f(f)
		{
		}
		forward_curve(size_t n, IT tb, IF fb, const F& f = 0)
			: fixed_income::forward_curve<T,F>(), tb_(tb), te_(tb_), fb_(fb), _f(f)
		{
			std::advance(te_, n);
		}
		forward_curve(IT tb, IT te, IF fb, const F& f = 0)
			: fixed_income::forward_curve<T,F>(), tb_(tb), te_(te), fb_(fb), _f(f)
		{
		}
		~forward_curve()
		{
		}

		// implement pure virtual base class functions
		F value(const T& t) const
		{
			IT ti = std::lower_bound(tb_, te_, t); // t <= *ti

			if (ti == te_)
				return _f;

			IF fb(fb_);
			std::advance(fb, std::distance(tb_, ti));

			return *fb;
		}
		F integral(const T& t) const
		{
			F I(0);
			T t0 = 0;
			IT tj = std::lower_bound(tb_, te_, t); // ti[-1] < t <= *ti[0]
			IF fi = fb_;
			for (IT ti = tb_; ti < tj; ++ti, ++fi) {
				I += static_cast<F>(*fi * (*ti - t0));
				t0 = *ti; 
			}
			I += static_cast<F>((tj == te_ ? _f : *fi)*(t - t0));

			return I;
		}

		size_t size(void) const
		{
			return std::distance(tb_, te_);
		}
		IT time(void) const
		{
			return tb_;
		}
		T time(size_t i) const
		{
			IT ti(tb_);

			std::advance(ti, i);

			return *ti;
		}

		F rate(size_t i) const
		{
			IF fi(fb_);

			std::advance(fi, i);

			return *fi;
		}
		F operator[](size_t i) const
		{
			return rate(i);
		}
		IF rate(void) const
		{
			return fb_;
		}

		// rate at back of curve
		F back(void) const
		{
			IF fi(fb_);
			
			std::advance(fi, size() - 1);

			return *fi;
		}
		// last time in curve
		T last(void) const
		{
			IT ti(te_);

			std::advance(ti, -1);

			return *ti;
		}

		F extrapolate(void) const
		{
			return _f;
		}
		// used for bootstrapping the curve
		const forward_curve& extrapolate(F f) const // _f is mutable
		{
			_f = f;

			return *this;
		}
	};

	template<class IT, class IF>
	inline forward_curve<IT,IF> make_forward_curve(IT db, IT de, IF fb, 
		const typename std::iterator_traits<IF>::value_type& f = 0)
	{
		return pwflat::forward_curve<IT,IF>(db, de, fb, f);
	}
	template<class IT, class IF>
	inline forward_curve<IT,IF> make_forward_curve(size_t n, IT db, IF fb, 
		const typename std::iterator_traits<IF>::value_type& f = 0)
	{
		return pwflat::forward_curve<IT,IF>(n, db, fb, f);
	}

} // namespace pwflat
} // namespace fixed_income
