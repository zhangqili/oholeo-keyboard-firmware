/*
 * lefl_animation.c
 */

/*
 * The MIT License (MIT)
 *
 * Copyright (c) .NET Foundation and Contributors
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */


#include "lefl.h"
#include "stdarg.h"

#define PI         3.14159265358979323846
#define max(a,b)   (a>b?a:b)
#define min(a,b)   (a<b?a:b)

#define FLOAT_EPS  1e-6
#define DOUBLE_EPS 1e-15
#define iszero(x)  (fabs(x)<10.0*FLOAT_EPS)
#define isone(x)  (fabs(x-1.0)<10.0*FLOAT_EPS)

#pragma GCC diagnostic ignored "-Wvarargs"

void lefl_animation_init(lefl_animation_base_t *a,
        lefl_animation_float_t (*easing_func)(lefl_animation_float_t f, ...),
        lefl_animation_mode_t mode)
{
    a->easing_func = easing_func;
    a->mode = mode;
}

void lefl_animation_bind(lefl_animation_base_t *a,lefl_animation_float_t *f)
{
    a->target=f;
}
void lefl_animation_begin(lefl_animation_base_t *a)
{
    a->tick=0;
}
lefl_animation_float_t lefl_animation_tick(lefl_animation_base_t *a)
{
    if(a->tick>=a->end)
    {
        return 0.0;
    }
    else
    {
        a->tick++;
    }
    lefl_animation_float_t normalized_time = lefl_animation_normalize(a);
    switch (a->mode)
    {
        case LEFL_ANIMATION_EASE_IN:
            *(a->target) = a->from+(a->to-a->from)*a->easing_func(normalized_time,a->parameter1,a->parameter2);
            break;
        case LEFL_ANIMATION_EASE_OUT:
            // EaseOut is the same as EaseIn, except time is reversed & the result is flipped.
            *(a->target) = a->from+(a->to-a->from)*(1.0 - a->easing_func(1.0 - normalized_time,a->parameter1,a->parameter2));
            break;
        case LEFL_ANIMATION_EASE_INOUT:
        default:
            // EaseInOut is a combination of EaseIn & EaseOut fit to the 0-1, 0-1 range.
            *(a->target) = a->from+(a->to-a->from)*((normalized_time < 0.5) ?
                   a->easing_func(normalized_time  * 2.0,a->parameter1,a->parameter2) * 0.5 :
                   (1.0 - a->easing_func((1.0 - normalized_time) * 2.0,a->parameter1,a->parameter2)) * 0.5 + 0.5);
            break;
    }
    return 0.0;
}

lefl_animation_float_t lefl_animation_normalize(lefl_animation_base_t *a)
{
    return (lefl_animation_float_t)a->tick/(lefl_animation_float_t)a->end;
}


lefl_animation_float_t lefl_animation_sine_ease(lefl_animation_float_t f, ...)
{
    return 1.0 - sin(PI*0.5*(1.0-f));
}
lefl_animation_float_t lefl_animation_pow_ease(lefl_animation_float_t f, ...)
{
    va_list v;
    va_start(v,1);
    lefl_animation_float_t power = (lefl_animation_float_t)va_arg(v,double);
    power = max(0.0,power);
    va_end(v);
    return pow(f,power);
}
lefl_animation_float_t lefl_animation_quad_ease(lefl_animation_float_t f, ...)
{
    return f*f;
}
lefl_animation_float_t lefl_animation_cubic_ease(lefl_animation_float_t f, ...)
{
    return f*f*f;
}
lefl_animation_float_t lefl_animation_quart_ease(lefl_animation_float_t f, ...)
{
    return f*f*f*f;
}
lefl_animation_float_t lefl_animation_quint_ease(lefl_animation_float_t f, ...)
{
    return f*f*f*f*f;
}
lefl_animation_float_t lefl_animation_expo_ease(lefl_animation_float_t f, ...)
{
    va_list v;
    va_start(v,1);
    lefl_animation_float_t expo = (lefl_animation_float_t)va_arg(v,double);
    va_end(v);
    if(iszero(expo))
        return f;
    else
        return (exp(expo*f)-1.0)/(exp(expo)-1.0);
}
lefl_animation_float_t lefl_animation_circ_ease(lefl_animation_float_t f, ...)
{
    f = max(0.0,min(1.0,f));
    return 1.0 - sqrt(1.0 - f * f);
}
lefl_animation_float_t lefl_animation_back_ease(lefl_animation_float_t f, ...)
{
    va_list v;
    va_start(v,1);
    lefl_animation_float_t amp = (lefl_animation_float_t)va_arg(v,double);
    va_end(v);
    return pow(f,3.0)-f*amp*sin(PI*f);
}
lefl_animation_float_t lefl_animation_elastic_ease(lefl_animation_float_t f, ...)
{
    va_list v;
    va_start(v,2);
    lefl_animation_float_t oscillations = (lefl_animation_float_t)va_arg(v,double);
    lefl_animation_float_t springiness = (lefl_animation_float_t)va_arg(v,double);
    va_end(v);
    lefl_animation_float_t oscillations1 = max(0.0,(lefl_animation_float_t)oscillations);
    lefl_animation_float_t expo;
    springiness = max(0.0,springiness);
    if(iszero(springiness))
        expo=f;
    else
        expo = (exp(springiness*f)-1.0)/(exp(springiness)-1.0);

    return expo*(sin((PI*2.0*oscillations1+PI*0.5)*f));
}
lefl_animation_float_t lefl_animation_bounce_ease(lefl_animation_float_t f, ...)
{
    // The math below is complicated because we have a few requirements to get the correct look for bounce:
    //  1) The bounces should be symetrical
    //  2) Bounciness should control both the amplitude and the period of the bounces
    //  3) Bounces should control the number of bounces without including the final half bounce to get you back to 1.0
    //
    //  Note: Simply modulating a expo or power curve with a abs(sin(...)) wont work because it violates 1) above.
    //

    // Constants
    va_list v;
    va_start(v,2);
    lefl_animation_float_t bounces = (lefl_animation_float_t)va_arg(v,double);
    lefl_animation_float_t bounciness = (lefl_animation_float_t)va_arg(v,double);
    va_end(v);
    bounces = max(0.0, bounces);

    // Clamp the bounciness so we dont hit a divide by zero
    if (bounciness < 1.0 || isone(bounciness))
    {
        // Make it just over one.  In practice, this will look like 1.0 but avoid divide by zeros.
        bounciness = 1.001;
    }

    lefl_animation_float_t power = pow(bounciness, bounces);
    lefl_animation_float_t oneMinusBounciness = 1.0 - bounciness;

    // 'unit' space calculations.
    // Our bounces grow in the x axis exponentially.  we define the first bounce as having a 'unit' width of 1.0 and compute
    // the total number of 'units' using a geometric series.
    // We then compute which 'unit' the current time is in.
    lefl_animation_float_t sumOfUnits = (1.0 - power) / oneMinusBounciness + power * 0.5; // geometric series with only half the last sum
    lefl_animation_float_t unitAtT = f * sumOfUnits;

    // 'bounce' space calculations.
    // Now that we know which 'unit' the current time is in, we can determine which bounce we're in by solving the geometric equation:
    // unitAtT = (1 - bounciness^bounce) / (1 - bounciness), for bounce.
    lefl_animation_float_t bounceAtT = log(-unitAtT * (1.0-bounciness) + 1.0)/log(bounciness);
    lefl_animation_float_t start = floor(bounceAtT);
    lefl_animation_float_t end = start + 1.0;

    // 'time' space calculations.
    // We then project the start and end of the bounce into 'time' space
    lefl_animation_float_t startTime = (1.0 - pow(bounciness, start)) / (oneMinusBounciness * sumOfUnits);
    lefl_animation_float_t endTime = (1.0 - pow(bounciness, end)) / (oneMinusBounciness * sumOfUnits);

    // Curve fitting for bounce.
    lefl_animation_float_t midTime = (startTime + endTime) * 0.5;
    lefl_animation_float_t timeRelativeToPeak = f - midTime;
    lefl_animation_float_t radius = midTime - startTime;
    lefl_animation_float_t amplitude = pow(1.0 / bounciness, (bounces - start));

    // Evaluate a quadratic that hits (startTime,0), (endTime, 0), and peaks at amplitude.
    return (-amplitude / (radius * radius)) * (timeRelativeToPeak - radius) * (timeRelativeToPeak + radius);
}

