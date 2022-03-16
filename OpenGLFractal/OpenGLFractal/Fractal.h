#ifndef FRACTAL_H
#define FRACTAL_H

#include "VGA.h"
#include "draw.h"

//Please uncomment one and only one of the 3 following defines
//#define STANDARD_DOUBLE
//#define FIXED_POINT
#define FIXED_POINT64
//#define MY_VERY_LONG_INT

#define NB_BALL 10

extern double conv_fixed_precision;

extern double X_position, Y_position, Angle, Scale;
extern double sinAngle;
extern double cosAngle;

extern double X_Ball[];
extern double Y_Ball[];
extern double R_Ball[];

extern int found_pearl, time_ms;

int get_time();
void draw_fractal(VGA& vga);
void draw_balls(VGA& vga);
#endif
