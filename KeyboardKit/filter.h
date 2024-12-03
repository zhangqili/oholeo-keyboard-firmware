#ifndef FILTER_H_
#define FILTER_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

#define TOLERANCE 3

typedef struct __Filter
{

} Filter;

typedef struct __AdaptiveSchimidtFilter
{
    float state;
} AdaptiveSchimidtFilter;

float adaptive_schimidt_filter(AdaptiveSchimidtFilter*filter,float value);

#endif