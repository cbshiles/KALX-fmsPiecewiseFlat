// secant.h - 1-d secant root finding
#pragma once

namespace root1d {

	// F:X -> Y
	template<class F, class X, class Y>
	class secant {
		F f_;
		X lo_, hi_;
		Y eps_;
		size_t iter_;
		X x0_, x1_;
		Y y0_, y1_;
	public:
		secant(F f, 
			Y eps = std::numeric_limits<X>::epsilon(),
			size_t iter = 100,
			X lo = -std::numeric_limits<X>::max(), 
			X hi = std::numeric_limits<X>::max()) 
			: f_(f), lo_(lo), hi_(hi), eps_(eps), iter_(iter)
		{ }
		void init(X x0, X x1)
		{
			x0_ = std::min(hi_, std::max(lo_, x0));
			x1_ = std::min(hi_, std::max(lo_, x1));
			y0_ = f_(x0);
			y1_ = f_(x1);
		}
		Y step(void)
		{
			ensure (iter_-- != 0);
			ensure (fabs(x1_ - x0_) > std::numeric_limits<X>::epsilon());

			Y m = (y1_ - y0_)/(x1_ - x0_);
			X x_ = x1_ - y1_*m;

			x0_ = x1_;
			y0_ = y1_;

			x1_ = std::min(hi_, std::max(lo_, x_));
			y1_ = f_(x_);

			return y1_;
		}
		X root(void) {
			while (fabs(y1_) > eps_) {
				step();
			}

			return x1_;
		}
	};

} // namespace root1d