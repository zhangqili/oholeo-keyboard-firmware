/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef FILTER_H_
#define FILTER_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "keyboard_conf.h"
#include "advanced_key.h"

#ifndef TOLERANCE
#define TOLERANCE 3
#endif

typedef struct __Filter
{

} Filter;

typedef struct __AdaptiveSchimidtFilter
{
    AnalogRawValue state;
} AdaptiveSchimidtFilter;

AnalogRawValue adaptive_schimidt_filter(AdaptiveSchimidtFilter*filter,AnalogRawValue value);

#endif