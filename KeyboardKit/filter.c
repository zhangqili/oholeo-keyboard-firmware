#include "filter.h"

AnalogValue adaptive_schimidt_filter(AdaptiveSchimidtFilter *filter, AnalogValue value)
{
    if (value - TOLERANCE > filter->state)
        filter->state = value - TOLERANCE;
    if (value + TOLERANCE < filter->state)
        filter->state = value + TOLERANCE;
    return filter->state;
}