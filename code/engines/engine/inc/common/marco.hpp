#pragma once

#include <cinttypes>

#define ONE_SEC_MARCO (1)
#define ONE_MIN_MACRO (ONE_SEC_MARCO * 60)
#define ONE_HOUR_MACRO (ONE_MIN_MACRO * 60)
#define ONE_DAY_MACRO (ONE_HOUR_MACRO * 24)
#define ONE_MONTH_MACRO(total_day_count) (ONE_HOUR_MACRO * total_day_count)

#define ONE_SEC_MILL_MARCO (1000)
#define ONE_MIN_MILL_MACRO (ONE_SEC_MILL_MARCO * 60)
#define ONE_HOUR_MILL_MACRO (ONE_MIN_MILL_MACRO * 60)
#define ONE_DAY_MILL_MACRO (ONE_HOUR_MILL_MACRO * 24)
#define ONE_MONTH_MILL_MACRO(total_day_count) (ONE_DAY_MILL_MACRO * total_day_count)
#define NOVALID_DELAY_MILL_MARCO (0XFFFFFFFFFFFFFFFF)
