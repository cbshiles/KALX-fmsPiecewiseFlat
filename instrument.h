// instrument.h - generic fixed income instrument inderface
// Copyright (c) 2011-2012 KALX, LLC. All rights reserved.
// Parameterized by time and flow
#pragma once
#include <algorithm>
#include <iterator>
#include <vector>
#include "ensure.h"
#include "bootstrap.h"

namespace fixed_income {

	// instrument having fixed cash flows
	template<class IT = double*, class IF = double*, class D = void*>
	class instrument : public instrument_base<IT,IF> {
	protected:
		typedef typename std::iterator_traits<IT>::value_type T;
		typedef typename std::iterator_traits<IF>::value_type F;

		IT tb_, te_;
		IF cb_;
	public:
		instrument()
			: tb_(0), te_(0), cb_(0)
		{ }
		// generic fixed income instrument
		instrument(IT tb, IT te, IF cb)
			: tb_(tb), te_(te), cb_(cb)
		{ }
		instrument(size_t n, IT tb, IF cb)
			: tb_(tb), te_(tb), cb_(cb)
		{
			std::advance(te_, n);
		}
		virtual ~instrument()
		{ }


		// fix cash flows based on valuation date and coupon
		virtual instrument& fix(D, F)
		{ 
			return *this;
		}

		// hook up iterators
		void set(IT tb, IT te, IF cb)
		{
			tb_ = tb;
			te_ = te;
			cb_ = cb;
		}
		void set(size_t n, IT tb, IF cb)
		{
			tb_ = tb;
			te_ = tb;
			std::advance(te_, n);
			cb_ = cb;
		}

		size_t size() const
		{
			return std::distance(tb_, te_);
		}
		// begin iterator for time
		IT time(void) const
		{
			return tb_;
		}
		const T& time(size_t i) const
		{
			ensure (i < size());
			IT ti(tb_);

			std::advance(ti, i);

			return *ti;
		}
		T& time(size_t i)
		{
			ensure (i < size());
			IT ti(tb_);

			std::advance(ti, i);

			return *ti;
		}

		// amounts corresponding to dates
		IF flow(void) const
		{
			return cb_;
		}
		const F& flow(size_t i) const
		{
			ensure (i < size());
			IF fi(cb_);

			std::advance(fi, i);

			return *fi;
		}
		F& flow(size_t i)
		{
			ensure (i < size());
			IF fi(cb_);

			std::advance(fi, i);

			return *fi;
		}

		// flows on or after t
		instrument& advance(const T& t)
		{
			size_t n = std::lower_bound(tb_, te_, t) - tb_;

			std::advance(cb_, n);
			std::advance(tb_, n);

			return *this;
		}
		instrument advance(const T& t) const
		{
			instrument i(*this);

			i.advance(t);

			return i;
		}

	};

	template<class IT = double*, class IF = double*>
	class instrument_generic : public instrument<IT,IF> {
		typedef typename std::iterator_traits<IT>::value_type T;
		typedef typename std::iterator_traits<IF>::value_type F;

		std::vector<T> t_;
		std::vector<F> f_;
	public:
		instrument_generic(IT tb, IT te, IF fb)
			: t_(tb, te), f_(fb, fb + std::distance(tb, te))
		{
			set(&t_[0], &t_[0] + t_.size(), &f[0]);
		}
		instrument_generic(size_t n, IT tb, IF fb)
			: t_(tb, tb + n), f_(fb, fb + n)
		{
			set(&t_[0], &t_[0] + n, &f[0]);
		}
	};
	// global functions
	template<class IT, class IF>
	inline fixed_income::instrument<IT,IF>
	make_instrument(IT tb, IT te, IF fb)
	{
		return fixed_income::instrument<IT,IF>(tb, te, fb);
	}
	template<class IT, class IF>
	inline fixed_income::instrument<IT,IF>
	make_instrument(size_t n, IT tb, IF fb)
	{
		IT te(tb);

		std::advance(te, n);

		return fixed_income::instrument<IT,IF>(tb, te, fb);
	}

} // namespace instrument