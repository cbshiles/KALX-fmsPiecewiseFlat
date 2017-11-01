Design criteria:
Lightweight classes.
Simple, well-factored code.
Prefer global functions to member functions.
General data types. (Some GPUs can only handle floats.)
Allow lambda expressions when possible.
Thread safe, to the extent possible.
Allow for passing pointers to pre-allocated memory to avoid copying.
Default to time in years for dates, but allow for arbitrary dates starting from an epoch.


FORWARD CURVE
namespace fixed_income
#include "forward_curve.h" 

Parameterized by time in years and rate.
value() is the instantaneous forward rate. 1 -> exp(f(t)dt) = 1 + f(t) dt from t -> t + dt.
integral() is the integral of the forward rate, int_0^t f(s) ds

Global functions

Discount, spot, and forward are related by

	D(t) = exp(-t r(t)) = exp(-f.integral(t)).

INSTRUMENT
namespace fixed_income
#include "instrument_base.h"

Fixed income instruments are a sequence of dates and corresponding cash flows
size() is the number of cash flows
time(i) is the time in years of i-th cash flow
flow(i) is the amount of i-th cash flow

VALUATION
namespace fixed_income
#include "valuation.h" : "forward_curve.h" "instrument_base.h"

Present value of an instrument is sum_j c_j D(t_j).

Given recovery R as a fraction of remaining pv and survival P(T > t) the risky pv is

E sum_j c_j Pi_j 1(T > t_j) + R (c_j Pi_j 1(T <= t_j)) = sum c_j D_j (R + (1 - R)P(T > t_j)).

This assumes the default time and stochastic discount are independent.

PIECEWISE FLAT CURVE
namespace fixed_income::pwflat
#include "pwflat_forward_curve.h" : "forward_curve.h"

Piecewise-flat forward curves are modeled by arrays t and f, and an extrapolated value _f;

	       { f[0] , 0 <= t <= t[0]
	f(t) = { f[i] , t[i-1] < t <= t[i], i = 1,...,n-1
	       { _f   , t > t[n-1]

|                 _f
|  f0          o---------
*------*  f1   
|      o-------*
|
0______t0______t1________

Note f(t[i]) = f[i].

Use size(), date(i), and rate(i) to get corresponding values for arrays t, and f.
The function last() gives the last time and back() gives the last forward rate.
Only value() and integrate() are implemented. 


BOOTSTRAP
#include "pwflat_bootstrap.h" : "pwflat_forward_curve.h"
The bootstrap algorithm adds instruments of increasing maturity by 1-d root finding on f_ that satisfies

	p = sum c[i] D(t[i]).

One and two cash flow instruments have closed form solutions. Three or more cash flow instruments
use the secant method. The initial value and multiplicative bump are parameters.

#include "instrument.h"

fix(valuation, coupon) - determines cash flows based on valuation date and coupon













#include "fixed_income.h"

An array of time in years, t, and cash flows, c, having price p.
Stub: count, unit, dcb, roll, cal - cd, fra, swaplet

fixed: coupon
float: no coupon

Use the standard theory of default probability and recovery to handle multiple curves.
counterparty: default rate, recovery

Risky floating legs.
D(t) - stochastic discount to time t
D(t, u) = E_t D(u)/D(t) - price at time t of zero maturing at u.
F(t; u, v) = 1/dcf (D(t, u)/D(t, v) - 1) - forward at time t over [u, v] given day count fraction

P^R,T = E[sum_j F_j dcb_j D_j 1(T > t_j) + R {E_j sum_{k > j} F_k D_k) 1(T <= t_j)}],
where F_j = F(t_{j-1}; t_{j-1}, t_j).

BOOTSTRAP ALGORITHM

Given forward curve n, t, f and cash flows m, u, c having price p.

Let pn = sum_{u[j] <= t[n-1]} c[j] D(u[j]).

If #{u[j] > t[n-1]} = 1, then p = pn + c[m-1] D(t[n-1]) exp(-f(u[m-1] - t[n-1])),
so f = -{log (p - pn)/c[m-1]D(t[n-1])}/(u[m-1] - t[n-1]).

If #{u[j] > t[n-1]} = 2, then p = pn + c[m-2] D(t[n-1]) exp(-f(u[m-2] - t[n-1]) + c[m-1]*D(t[n-1]) exp(-f(u[m-1] - t[n-1])),