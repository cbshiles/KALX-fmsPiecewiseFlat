// tnewton.cpp
#include <cmath>
#include <iostream>
#include <functional>
#include <random>
#include "../ensure.h"
#include "../newton.h"

using namespace std;

static std::default_random_engine eng;
static std::uniform_real_distribution<> urd(0, 1);
static auto u = std::bind(urd, eng);
static double eps = numeric_limits<double>::epsilon();

void
fms_test_newton(void)
{
	for (int i = 0; i < 10000; ++i) {
		double a = u();
		double b = u();
		double c = u();
		if (b > c)
			swap(b, c);
//		cout << "a = " << a << " b = " << b << " c = " << c << endl;

		auto f = [a,b,c](double x) { return a*(x - b)*(x - c); };
		auto df = [a,b,c](double x) { return a*((x - b) + (x - c)); };

		double r;
		
		r = root1d::newton((b + c)/3, f, df);
//		cout << b - r << endl;
		ensure (fabs(b - r)*a*(c - b) <= eps);
		r = root1d::newton((b + c)/1.5, f, df);
//		cout << c - r << endl;
		ensure (fabs(c - r)*a*(c - b) <= eps);

	}
}