#include "Fractal.h"
#include "my_very_long_int.h"
#include "stdio.h"

#ifndef USE_OPEN_GL
#include <sys/alt_alarm.h>
#endif

double cRe = -0.88; // -0.78;
double cIm = 0.258;

double X_position, Y_position, Angle, Scale;
double sinAngle;
double cosAngle;

double X_Ball[NB_BALL];
double Y_Ball[NB_BALL];
double R_Ball[NB_BALL];

int found_pearl, time_ms;



/********************************************************************/
int get_color(int level) {
	if (level > 63) level = 63;
	return ((level << 2) & 0xE0) + level / 2;
}



/********************************************************************/
int get_time() {
#ifdef USE_OPEN_GL
	return glutGet(GLUT_ELAPSED_TIME);
#else
	return alt_nticks();
#endif
}

/*******************************************************************
newZ = oldZ^2+C

or oldZ = oldRe + j*oldIm
   newZ = newRe + j*newIm
	  C =   cRe + j*cIm

donc, newZ = (oldRe + j*oldIm)^2 + cRe + j*cIm
		   = oldRe^2 - oldIm^2 + 2*j*oldRe*oldIm + cRe + j*cIm
		   = oldRe^2 - oldIm^2 + cRe j*(2*oldRe*oldIm + cIm)

donc, newRe = oldRe^2 - oldIm^2 + cRe
	  newIm = 2*oldRe*oldIm + cIm

pour v�rifier si la suite diverge, on va mesurer si le module de oldZ>2,
... ou plus simplement si le carr� de son module (oldRe^2 + oldIm^2)>4

*******************************************************************/

#ifdef STANDARD_DOUBLE
/********************************************************************/
int Get_Fractal_Level(double newRe, double newIm) {
	int i;
	//DEBUG
	//printf("\ndouble ");

	for (i = 0; i < 64; i++) {
//		printf("%f, %f\n", newRe, newIm);	//DEBUG
		double oldRe = newRe;
		double oldIm = newIm;
		double sqRe = oldRe * oldRe;
		double sqIm = oldIm * oldIm;
		if ((sqRe + sqIm) > 4) break;
		newRe = sqRe - sqIm + cRe;
		newIm = 2 * oldRe * oldIm + cIm;

		//DEBUG
		//printf("(%f,%f)",newRe,newIm);
	}
	return i;
}

#endif

#ifdef FIXED_POINT
#define fixed_precision 26
/********************************************************************/
int Get_Fractal_Level(double newRe, double newIm) {
	long long fp_cRe = (cRe * (1LL << fixed_precision));
	long long fp_cIm = (cIm * (1LL << fixed_precision));
	long long fp_4 = (4 * (1LL << fixed_precision));
	double conv_fixed_precision = (1LL << fixed_precision);

	int i;

	//DEBUG
	//printf("\nlong ");

	long long fp_newRe = (newRe * conv_fixed_precision);
	long long fp_newim = (newIm * conv_fixed_precision);

	for (i = 0; i < 64; i++) {
		long long fp_oldRe = fp_newRe;
		long long fp_oldIm = fp_newim;
		long long fp_sqRe = fp_oldRe * fp_oldRe; fp_sqRe >>= fixed_precision;
		long long fp_sqIm = fp_oldIm * fp_oldIm; fp_sqIm >>= fixed_precision;

		if ((fp_sqRe + fp_sqIm) > fp_4) break;

		fp_newRe = fp_sqRe - fp_sqIm; fp_newRe += fp_cRe;
		fp_newim = fp_oldRe * fp_oldIm; fp_newim >>= (fixed_precision - 1); fp_newim += fp_cIm;

		//DEBUG
		//newRe=new_re; newRe/=conv_fixed_precision;
		//newIm=new_im; newIm/=conv_fixed_precision;
		//printf("(%f,%f)",newRe,newIm);
	}
	return i;
}
#endif

#ifdef FIXED_POINT64
#define fixed_precision 49
//********************************************************************************************************
// Source : https://www.codeproject.com/Tips/618570/UInt-Multiplication-Squaring 
//********************************************************************************************************
void mult64to128(unsigned long long u, unsigned long long v, unsigned long long& h, unsigned long long& l)
{
	unsigned long long u1 = (u & 0xffffffff);
	unsigned long long v1 = (v & 0xffffffff);
	unsigned long long t = (u1 * v1);
	unsigned long long w3 = (t & 0xffffffff);
	unsigned long long k = (t >> 32);

	u >>= 32;
	t = (u * v1) + k;
	k = (t & 0xffffffff);
	unsigned long long w1 = (t >> 32);

	v >>= 32;
	t = (u1 * v) + k;
	k = (t >> 32);

	h = (u * v) + w1 + k;
	l = (t << 32) + w3;
}

long long my_multiply_shift(long long a, long long b, int shift_value) {
	unsigned long long high, low;
	long long result;
	long sign = 1;

	if (a < 0) {
		a = -a; sign = -sign;
	}

	if (b < 0) {
		b = -b; sign = -sign;
	}

	mult64to128((unsigned long long) a, (unsigned long long) b, high, low);
	low >>= shift_value;
	low += high << (64 - shift_value);
	result = low;
	result *= sign;

	return result;
}

int Get_Fractal_Level(double newRe, double newIm) {
	long long fp_cRe = (cRe * (1LL << fixed_precision));
	long long fp_cIm = (cIm * (1LL << fixed_precision));
	long long fp_4 = (4 * (1LL << fixed_precision));
	double conv_fixed_precision = (1LL << fixed_precision);

	int i;

	//DEBUG
	//printf("\nlong ");

	long long fp_newRe = (newRe * conv_fixed_precision);
	long long fp_newim = (newIm * conv_fixed_precision);

	for (i = 0; i < 64; i++) {
		long long fp_oldRe = fp_newRe;
		long long fp_oldIm = fp_newim;
		long long fp_sqRe = my_multiply_shift(fp_oldRe, fp_oldRe, fixed_precision);
		long long fp_sqIm = my_multiply_shift(fp_oldIm, fp_oldIm, fixed_precision);

		if ((fp_sqRe + fp_sqIm) > fp_4) break;

		fp_newRe = fp_sqRe - fp_sqIm; fp_newRe += fp_cRe;
		fp_newim = my_multiply_shift(fp_oldRe, fp_oldIm, fixed_precision - 1); fp_newim += fp_cIm;

		//DEBUG
		//newRe=new_re; newRe/=conv_fixed_precision;
		//newIm=new_im; newIm/=conv_fixed_precision;
		//printf("(%f,%f)",newRe,newIm);
	}
	return i;
}

void draw_fractal(VGA& vga) {
	int x, y;

	double offset = 1.0 / 240.0;
	double ratio = 320 / 240.0;
	double conv_fixed_precision = (1LL << fixed_precision);

	double xLoopOffX = offset * cosAngle / Scale;
	double xLoopOffY = -offset * sinAngle / Scale;

	double yLoopOffX = -640 * (offset * cosAngle) / Scale + offset * sinAngle / Scale;
	double yLoopOffY = 640 * (offset * sinAngle) / Scale + offset * cosAngle / Scale;

	double currX = X_position - (ratio * cosAngle + sinAngle) / Scale;
	double currY = Y_position + (ratio * sinAngle - cosAngle) / Scale;
	
    long long fp_xLoopOffX = (xLoopOffX * (1LL << fixed_precision));
	long long fp_xLoopOffY = (xLoopOffY * (1LL << fixed_precision));
	long long fp_yLoopOffX = (yLoopOffX * (1LL << fixed_precision));
	long long fp_yLoopOffY = (yLoopOffY * (1LL << fixed_precision));
	long long fp_currX = (currX * (1LL << fixed_precision));
	long long fp_currY = (currY * (1LL << fixed_precision));

	for (y = 0; y < 480; y++) {
		for (x = 0; x < 640; x++) {
			if ((x == 320) || (y == 240)) vga.Set_Pixel_Color(x, y, 31);
			else {
				currX = fp_currX / conv_fixed_precision;
				currY = fp_currY / conv_fixed_precision;
				int level = Get_Fractal_Level(currX, currY);
				vga.Set_Pixel_Color(x, y, get_color(level));
			}
			fp_currX += fp_xLoopOffX;
			fp_currY += fp_xLoopOffY;
		}
		fp_currX += fp_yLoopOffX;
		fp_currY += fp_yLoopOffY;
	}
#ifdef USE_OPEN_GL
	glFlush();
#endif
}


#endif

#ifdef MY_VERY_LONG_INT
int Get_Fractal_Level(double newRe, double newIm) {
	my_very_long_int m_cRe; my_init_from_double(m_cRe, cRe);
	my_very_long_int m_cIm; my_init_from_double(m_cIm, cIm);
	my_very_long_int m_4; my_init_from_double(m_4, 4.0);

	int i;

	my_very_long_int new_re; my_init_from_double(new_re, newRe);
	my_very_long_int new_im; my_init_from_double(new_im, newIm);

	for (i = 0; i < 64; i++) {
		//double dre = my_to_double(new_re);
		//double dim = my_to_double(new_im);
		//printf("%f, %f\n", dre, dim);

		my_very_long_int old_re;  my_copy(old_re, new_re);
		my_very_long_int old_im;  my_copy(old_im, new_im);
		my_very_long_int sq_re; my_multiply_shift (sq_re, old_re, old_re, my_verly_long_precision);
		my_very_long_int sq_im; my_multiply_shift (sq_im, old_im, old_im, my_verly_long_precision);
			
		my_very_long_int sum; 
		my_addition(sum, sq_re, sq_im); 
		my_subtraction(sum, sum, m_4);
		if (my_positive(sum)) break;

		my_subtraction(new_re, sq_re, sq_im); 
		my_addition(new_re, new_re, m_cRe);
		my_multiply_shift(new_im, old_re, old_im, my_verly_long_precision - 1);
		my_addition(new_im, new_im, m_cIm);

	}
	return i;
}
#endif

/********************************************************************/
/*
void draw_fractal(VGA& vga) {
	int x, y;

	double offset = 1.0 / 240.0;
	double ratio = 320 / 240.0;

	double xLoopOffX = offset * cosAngle / Scale;
	double xLoopOffY = -offset * sinAngle / Scale;

	double yLoopOffX = -640 * (offset * cosAngle) / Scale + offset * sinAngle / Scale;
	double yLoopOffY = 640 * (offset * sinAngle) / Scale + offset * cosAngle / Scale;

	double my_currX = X_position - (ratio * cosAngle + sinAngle) / Scale;
	double my_currY = Y_position + (ratio * sinAngle - cosAngle) / Scale;

	for (y = 0; y < 480; y++) {
		for (x = 0; x < 640; x++) {
			if ((x == 320) || (y == 240)) vga.Set_Pixel_Color(x, y, 31);
			else {
				int level = Get_Fractal_Level(my_currX, my_currY);
				vga.Set_Pixel_Color(x, y, get_color(level));
			}
			my_currX += xLoopOffX;
			my_currY += xLoopOffY;
		}
		my_currX += yLoopOffX;
		my_currY += yLoopOffY;
	}
#ifdef USE_OPEN_GL
	glFlush();
#endif
}
*/

/********************************************************************/
void draw_balls(VGA& vga) {
	int i;
	for (i = 0; i < NB_BALL; i++) {
		if (R_Ball[i] == 0) continue;	// the pearl has already been found
		double screen_X = X_Ball[i];
		double screen_Y = Y_Ball[i];

		screen_X -= X_position;
		screen_Y -= Y_position;
		screen_X *= Scale;
		screen_Y *= Scale;

		double screen_Radius = R_Ball[i] * Scale;
		if ((screen_X * screen_X + screen_Y * screen_Y <= screen_Radius * screen_Radius)) {
			//La perle est trouvee
			R_Ball[i] = 0;
			int time = get_time();
			printf("Found %i pearl(s) at time %i\n", ++found_pearl, time);
		}

		double tmp_X = screen_X;
		double tmp_Y = screen_Y;
		screen_X = (tmp_X * cosAngle - tmp_Y * sinAngle) * 240 + 320;
		screen_Y = (tmp_X * sinAngle + tmp_Y * cosAngle) * 240 + 240;
		screen_Radius *= 240;

		if (screen_X + screen_Radius < 0) continue;
		if (screen_X - screen_Radius >= 640) continue;
		if (screen_Y + screen_Radius < 0) continue;
		if (screen_Y - screen_Radius >= 480) continue;

		drawBall3D(vga, screen_X, screen_Y, screen_Radius, i % 7);
	}
#ifdef USE_OPEN_GL
	glFlush();
#endif
}
