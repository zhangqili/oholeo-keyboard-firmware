#include "filter.h"

float adaptive_schimidt_filter(AdaptiveSchimidtFilter *filter, float value)
{
    if (value - TOLERANCE > filter->state)
        filter->state = value - TOLERANCE;
    if (value + TOLERANCE < filter->state)
        filter->state = value + TOLERANCE;
    return filter->state;
}