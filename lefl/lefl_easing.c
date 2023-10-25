/*
 * lefl_easing.c
 */
#include "lefl.h"
#include "math.h"

void lefl_easing_pid(float *f,float target)
{
    //if(!(fabs(*f+0.5-target)<=0.5))
        *f=*f+LEFL_ANIMATION_SPEED*(target+0.5-*f);
    //else
        //*f=target;
}
