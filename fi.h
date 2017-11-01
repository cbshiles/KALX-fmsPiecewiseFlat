// fi.h - Fixed Income routines
// Copyright (c) 2013 KALX, LLC. All rights reserved.
#pragma once
#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>

#pragma warning(disable: 4100)

namespace functional {

	// iterator with function applied to each item
	template<class F, class I>
	class apply_iterator : public std::iterator<std::input_iterator_tag, typename std::iterator_traits<I>::value_type> {
	protected:
		typedef typename std::iterator_traits<I>::value_type value_type;
		F f_;
		I i_;
	public:
		apply_iterator(F f, I i) 
			: f_(f), i_(i) 
		{ }
		apply_iterator(const apply_iterator& i)
			: f_(i.f_), i_(i.i_)
		{
		}
		apply_iterator& operator=(const apply_iterator& i)
		{
			if (this != &i) {
				f_ = i.f_;
				i_ = i.i_;
			}

			return *this;
		}
		~apply_iterator()
		{ }
		operator I(void)
		{
			return i_;
		}
		operator I(void) const
		{
			return i_;
		}
		value_type operator*() const
		{
			return f_(*i_);
		}
		apply_iterator& operator++(void)
		{
			++i_;

			return *this;
		}
		apply_iterator operator++(int)
		{
			apply_iterator i(f_, i_);

			++i_;

			return i;
		}
	};
	template<class F, class I>
	inline apply_iterator<F,I> apply(const F& f, I i)
	{
		return apply_iterator<F,I>(f, i);
	}
	template<class F>
	inline auto domain_max(const F& f) -> decltype(f(0))
	{
		return std::numeric_limits<decltype(f(0))>::max();
	}

	template<class F, class G>
	inline auto extrapolate(const F& f, const G& g) -> std::function<decltype(f(0))(decltype(f(0)))> 
	{
		typedef typename decltype(f(0)) T;

		return [f,g](T t) -> T { return t <= domain_max(f) ? f(t) : g(t); };
	}

	// trapezoidal rule
	template<class F>
	inline auto integral(const F& f, size_t n = 100) -> std::function<decltype(f(0))(decltype(f(0)))> 
	{
		typedef typename decltype(f(0)) T;

		return [f,n](T t) -> T
		{
			T I(0), f0(f(0));

			for (size_t i = 1; i <= n; ++i) {
				T fi = f(t*i/100);
				I += (t/100)*(fi + f0)/2;
				f0 = fi;
			}

			return I;
		};
	}

	template<class T>
	struct constant {
		T c;
		constant(T _c)
			: c(_c)
		{ }
		T operator()(T) const
		{
			return c;
		}
	};
	template<class T>
	std::function<T(T)> integral(const constant<T>& c)
	{
		return [c](T t) -> T { return t*c(0); };
	}

	template<class T>
	struct identity {
		T operator()(T t) const
		{
			return t;
		}
	};
	template<class T>
	std::function<T(T)> integral(const identity<T>& f)
	{
		return [f](T t) -> T { return t*t/2; };
	}

	template<class T>
	struct piecewise_constant {
		size_t n;
		const T* t;
		const T* f;
		piecewise_constant(size_t _n = 0, const T* _t = 0, const T* _f = 0)
			: n(_n), t(_t), f(_f)
		{ }
		T operator()(T u) const
		{
			const T* ti = std::lower_bound(t, t + n, u);

			return ti != t + n ? f[ti - t] : std::numeric_limits<T>::quiet_NaN();
		}
	};
	template<class T>
	inline T domain_max(const piecewise_constant<T>& f)
	{
		return f.n ? f.t[f.n - 1] : std::numeric_limits<T>::max();
	}

	template<class T>
	std::function<T(T)> integral(const piecewise_constant<T>& F, T _f = 0)
	{
		return [F](T u) -> T {
			size_t n = F.n;
			const T* t = F.t;
			const T* f = F.f;

			T I(0), t_(0);

			while (n-- && *t <= u) {
				I += *f++ * (*t++ - t_);
				t_ = *t;
			}

			I += (n != -1 ? *f : _f)*(u - t_);

			return I;
		};
	}

} // namespace functional

template<class F, class G, class Op>
inline auto operator_op(const F& f, const G& g, Op op) -> std::function<decltype(op(f(0),g(0)))(decltype(f(0)))>
{
	typedef typename decltype(f(0)) T;

	return [f,g,op](T t) -> T { return op(f(t), g(t)); };
}

template<class F, class G> 
inline auto operator+(const F& f, const G& g) -> std::function<decltype(f(0) + g(0))(decltype(f(0)))>
{
	return operator_op(f, g, std::plus<decltype(f(0))>()); 
}
template<class F, class G> 
inline auto operator-(const F& f, const G& g) -> std::function<decltype(f(0) - g(0))(decltype(f(0)))>
{
	return operator_op(f, g, std::minus<decltype(f(0))>()); 
}
template<class F, class G> 
inline auto operator*(const F& f, const G& g) -> std::function<decltype(f(0) * g(0))(decltype(f(0)))>
{
	return operator_op(f, g, std::multiplies<decltype(f(0))>()); 
}
template<class F, class G> 
inline auto operator/(const F& f, const G& g) -> std::function<decltype(f(0) / g(0))(decltype(f(0)))>
{
	return operator_op(f, g, std::divides<decltype(f(0))>()); 
}


namespace fi {

	// spot given forward
	template<class F>
	inline auto spot(const F& f) -> std::function<decltype(f(0))(decltype(f(0)))>
	{
		typedef typename decltype(f(0)) T;

		return [f](T t) -> T { return 1 + t == 1 ? f(t) : functional::integral(f)(t)/t; };
	}
	// discount given forward
	template<class F>
	inline auto discount(const F& f) -> std::function<decltype(f(0))(decltype(f(0)))>
	{
		typedef typename decltype(f(0)) T;

		return [f](T t) -> T { return exp(-functional::integral(f)(t)); };
	}

	// present value of cash flows given discount
	template<class D>
	inline auto present_value(const D& d) -> std::function<decltype(d(0))(size_t, const decltype(d(0))*, const decltype(d(0))*)>
	{
		typedef typename decltype(d(0)) T;

		return [d](size_t n, const T* t, const T* c) { 
			return std::inner_product(c, c + n, functional::apply(d, t), (T)0);
		};
	}

	// d(pv)/df for parallel shift past t0
	template<class D>
	inline auto duration(const D& d, decltype(d(0)) t0 = 0) -> std::function<decltype(d(0))(size_t, const decltype(d(0))*, const decltype(d(0))*)>
	{
		typedef typename decltype(d(0)) T;

		return [d,t0](size_t n, const T* t, const T* c) -> T { 
			T dur(0);

			while (n && *t <= t0) {
				--n;
				++t;
				++c;
			}
			while (n--) {
				dur -= (*t - t0)*(*c)*d(*t);
				++t;
				++c;
			}
			
			return dur;
		};
	}

} // namespace fi