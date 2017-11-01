// eurodollar_futures.h - Eurodollar futures contracts.
// Copyright (c) 2011 KALX, LLC. All rights reserved.
// See http://cme.com/
#pragma once
#include "../fmsdatetime/datetime.h"

enum euribor_futures_enum {
	EDH0, // March 2020
	EDM0,
	EDU0,
	EDZ0,
	EDH1, // March 2020
	EDM1, // June 2020
	EDU1, // September 2011 as of 7/16/2011
	EDZ1,
	EDH2,
	EDM2,
	EDU2,
	EDZ2,
	EDH3,
	EDM3,
	EDU3,
	EDZ3,
	EDH4,
	EDM4,
	EDU4,
	EDZ4,
	EDH5,
	EDM5,
	EDU5,
	EDZ5,
	EDH6,
	EDM6,
	EDU6,
	EDZ6,
	EDH7,
	EDM7,
	EDU7,
	EDZ7,
	EDH8,
	EDM8,
	EDU8,
	EDZ8,
	EDH9,
	EDM9,
	EDU9,
	EDZ9,
};

static struct  {
	month_of_year month;
	unsigned int year; // last digit of year
} euribor_futures_data[] = {
	{MONTH_MAR,0},
	{MONTH_JUN,0},
	{MONTH_SEP,0},
	{MONTH_DEC,0},
	{MONTH_MAR,1},
	{MONTH_JUN,1},
	{MONTH_SEP,1},
	{MONTH_DEC,1},
	{MONTH_MAR,2},
	{MONTH_JUN,2},
	{MONTH_SEP,2},
	{MONTH_DEC,2},
	{MONTH_MAR,3},
	{MONTH_JUN,3},
	{MONTH_SEP,3},
	{MONTH_DEC,3},
	{MONTH_MAR,4},
	{MONTH_JUN,4},
	{MONTH_SEP,4},
	{MONTH_DEC,4},
	{MONTH_MAR,5},
	{MONTH_JUN,5},
	{MONTH_SEP,5},
	{MONTH_DEC,5},
	{MONTH_MAR,6},
	{MONTH_JUN,6},
	{MONTH_SEP,6},
	{MONTH_DEC,6},
	{MONTH_MAR,7},
	{MONTH_JUN,7},
	{MONTH_SEP,7},
	{MONTH_DEC,7},
	{MONTH_MAR,8},
	{MONTH_JUN,8},
	{MONTH_SEP,8},
	{MONTH_DEC,8},
	{MONTH_MAR,9},
	{MONTH_JUN,9},
	{MONTH_SEP,9},
	{MONTH_DEC,9},
};
