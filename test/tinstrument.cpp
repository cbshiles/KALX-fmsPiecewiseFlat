// tinstrument.cpp - test instrument classes
#ifndef ensure
#include <cassert>
#define ensure(x) assert(x)
#endif
#include "../bootstrap.h"

using namespace fixed_income;

template<class T>
void
test_instrument(void)
{
	T t[] = {20120101, 20130101, 20140101};
	T c[] = {1.2, 2.3, 3.4};
	instrument<T> i(3, t, c);
}

void fms_test_instrument(void)
{
	test_instrument<double>();
}