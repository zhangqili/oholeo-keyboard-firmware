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
    AnalogValue state;
} AdaptiveSchimidtFilter;

AnalogValue adaptive_schimidt_filter(AdaptiveSchimidtFilter*filter,AnalogValue value);

#endif