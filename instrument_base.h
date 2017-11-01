// instrument_base.h - minimal instrument class
#pragma once
#include <iterator>

namespace fixed_income {

	template<class IT = double*, class IF = double*>
	struct instrument_base {
		typedef typename std::iterator_traits<IT>::value_type T;
		typedef typename std::iterator_traits<IF>::value_type F;
		instrument_base() { }
		virtual ~instrument_base() { }

		// number of cash flows
		virtual size_t size() const = 0;
		
		// begin iterator for time
		virtual IT time(void) const = 0;
		virtual const T& time(size_t i) const = 0;

		// begin iterator for amounts corresponding to times
		virtual IF flow(void) const = 0;
		virtual const F& flow(size_t i) const = 0;
	};

} // namespace fixed_income