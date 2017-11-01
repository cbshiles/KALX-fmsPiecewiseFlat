// main.cpp - test driver
// Copyright (c) 2011 KALX, LLC. All rights reserved. No warranty made.
#include <cassert>
#include <iostream>

using namespace std;

//#define ensure(x) assert(x)

void fms_test_fi(void);

void fms_test_newton();
void fms_test_forward();
void fms_test_instrument();
void fms_test_valuation();
void fms_test_bootstrap();
//void fms_test_fixed_income();
void fms_test_pwflat();


int
main(void)
{
	try {
		fms_test_fi();

		fms_test_newton();
		fms_test_forward();
		fms_test_instrument();
		fms_test_valuation();
		fms_test_bootstrap();
//		fms_test_fixed_income();
		fms_test_pwflat();
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;

		return -1;
	}

	return 0;
}
