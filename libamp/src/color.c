/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "color.h"
#include "string.h"

void rgb_to_hsv(ColorHSV *hsv, ColorRGB *rgb)
{
	float max;
	float min;
	max=rgb->r;
	min=rgb->r;
	max=rgb->g>max?rgb->g:max;
	min=rgb->g<min?rgb->g:min;
	max=rgb->b>max?rgb->b:max;
	min=rgb->b<min?rgb->b:min;
	if(max==min)
	{
		hsv->h=0;
	}
	if(max==rgb->r&&rgb->g>=rgb->b)
	{
		hsv->h=60*(rgb->g-rgb->b)/(max-min);
	}
	else
	{
		hsv->h=60*(rgb->g-rgb->b)/(max-min)+360;
	}
	if(max==rgb->g)
	{
		hsv->h=60*(rgb->b-rgb->r)/(max-min)+120;
	}
	if(max==rgb->b)
	{
		hsv->h=60*(rgb->r-rgb->g)/(max-min)+240;
	}
	if(max==0)
	{
		hsv->s=0;
	}
	else
	{
		hsv->s=100*(1-min/max);
	}
	hsv->v=max*100/255;
}

void hsv_to_rgb(ColorRGB *rgb, ColorHSV *hsv)
{
    float c = 0;
    float x = 0;
    float y = 0;
    float z = 0;
    float h = (float)(hsv->h);
    float s = ((float)(hsv->s))/100.0;
    float v = ((float)(hsv->v))/100.0;
    if(s < 1e-6)
    {
        rgb->r=hsv->v*255.0/100.0;
        rgb->g=hsv->v*255.0/100.0;
        rgb->b=hsv->v*255.0/100.0;
    }
    else
    {
        h=h/60;
        c=h-(int)h;

        x = v*(1-s);
        y = v*(1-s*c);
        z = v*(1-s*(1-c));
        switch (hsv->h/60)
        {
            case 0:
                rgb->r = hsv->v*255.0/100.0;
                rgb->g = z*255;
                rgb->b = x*255;
                break;
            case 1:
                rgb->r = y*255;
                rgb->g = hsv->v*255.0/100.0;
                rgb->b = x*255;
                break;
            case 2:
                rgb->r = x*255;
                rgb->g = hsv->v*255.0/100.0;
                rgb->b = z*255;
                break;
            case 3:
                rgb->r = x*255;
                rgb->g = y*255;
                rgb->b = hsv->v*255.0/100.0;
                break;
            case 4:
                rgb->r = z*255;
                rgb->g = x*255;
                rgb->b = hsv->v*255.0/100.0;
                break;
            case 5:
                rgb->r = hsv->v*255.0/100.0;
                rgb->g = x*255;
                rgb->b = y*255;
                break;
            default:
                break;
        }
    }
}

void color_get_rgb(Color*color, ColorRGB*rgb)
{
    memcpy(rgb,color,sizeof(Color));
}
void color_set_rgb(Color*color, ColorRGB*rgb)
{
    memcpy(color,rgb,sizeof(Color));
}

void color_get_hsv(Color*color, ColorHSV*hsv)
{
    rgb_to_hsv(hsv, color);
}
void color_set_hsv(Color*color, ColorHSV*hsv)
{
    hsv_to_rgb(color, hsv);
}

void color_mix(Color *dest, Color *source)
{
	if (dest->r + source->r < 256)
	{
		dest->r += source->r;
	}
	else
	{
		dest->r = 255;
	}
	if (dest->g + source->g < 256)
	{
		dest->g += source->g;
	}
	else
	{
		dest->g = 255;
	}
	if (dest->b + source->b < 256)
	{
		dest->b += source->b;
	}
	else
	{
		dest->b = 255;
	}
}