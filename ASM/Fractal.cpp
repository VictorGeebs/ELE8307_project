#include "Fractal.h"
#include "my_very_long_int.h"
#include "stdio.h"
#include "math_constants.h"
#include "Simulator.h"

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

long long x_cos_fxp[VGA_WIDTH][160];
long long x_sin_fxp[VGA_WIDTH][160];
long long y_cos_fxp[VGA_HEIGHT][160];
long long y_sin_fxp[VGA_HEIGHT][160];

//long long m_x_cos_fxp[640][160];
//long long m_x_sin_fxp[640][160];
//long long m_y_cos_fxp[640][160];
//long long m_y_sin_fxp[640][160];

long long x_ball_fxp[NB_BALL];
long long y_ball_fxp[NB_BALL];
long long r_ball_fxp[NB_BALL];

//#include <iostream>
//#include <fstream>
//using namespace std;
//ofstream myfile;
//bool oneTimeLog = true;

void initPreCalculation() {
    for (int i = 0; i < 640; ++i) {
        for (int angle = 0; angle < 160; ++angle) {
            x_cos_fxp[i][angle] = ((i - 320) * cos(double(angle) * PI / 80.0) / 240.0) * (1LL << fixed_precision);
            x_sin_fxp[i][angle] = ((i - 320) * sin(double(angle) * PI / 80.0) / 240.0) * (1LL << fixed_precision);
        }
    }

    for (int i = 0; i < 480; ++i) {
        for (int angle = 0; angle < 160; ++angle) {
            y_cos_fxp[i][angle] = ((i - 240) * cos(double(angle) * PI / 80.0) / 240.0) * (1LL << fixed_precision);
            y_sin_fxp[i][angle] = ((i - 240) * sin(double(angle) * PI / 80.0) / 240.0) * (1LL << fixed_precision);
        }
    }

    //for (int i = 0; i < 640; ++i) {
    //	for (int angle = 0; angle < 160; ++angle) {
    //		m_x_cos_fxp[i][angle] = ((i - 320) * cos(double(angle) * PI / 80.0)) * (1LL << fixed_precision);
    //		m_x_sin_fxp[i][angle] = ((i - 320) * sin(double(angle) * PI / 80.0)) * (1LL << fixed_precision);
    //	}
    //}

    //for (int i = 0; i < 480; ++i) {
    //	for (int angle = 0; angle < 160; ++angle) {
    //		m_y_cos_fxp[i][angle] = ((i - 240) * cos(double(angle) * PI / 80.0)) * (1LL << fixed_precision);
    //		m_y_sin_fxp[i][angle] = ((i - 240) * sin(double(angle) * PI / 80.0)) * (1LL << fixed_precision);
    //	}
    //}

    for (int i = 0; i < NB_BALL; ++i) {
        x_ball_fxp[i] = X_Ball[i] * (1LL << fixed_precision);
        y_ball_fxp[i] = Y_Ball[i] * (1LL << fixed_precision);
        r_ball_fxp[i] = R_Ball[i] * (1LL << fixed_precision);
    }

    Simulator::init();
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

pour vérifier si la suite diverge, on va mesurer si le module de oldZ>2,
... ou plus simplement si le carré de son module (oldRe^2 + oldIm^2)>4

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
//********************************************************************************************************
// Source : https://www.codeproject.com/Tips/618570/UInt-Multiplication-Squaring 
//********************************************************************************************************
void mult64to128(const unsigned long long u, const unsigned long long v, unsigned long long& h, unsigned long long& l)
{
    const unsigned long long uL = (u & 0xffffffff);
    const unsigned long long vL = (v & 0xffffffff);
    const unsigned long long uH = (u >> 32);
    const unsigned long long vH = (v >> 32);

    const unsigned long long z1 = uL * vL;
    const unsigned long long z2 = uH * vL;
    const unsigned long long z3 = uL * vH;
    const unsigned long long z4 = uH * vH;

    const unsigned long long s = (z1 >> 32) + (z2 & 0xffffffff) + (z3 & 0xffffffff);
    h = (s >> 32) + (z2 >> 32) + (z3 >> 32) + z4;
    l = (s << 32) | (z1 & 0xffffffff);
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

const long long fp_cRe = (cRe * (1LL << fixed_precision));
const long long fp_cIm = (cIm * (1LL << fixed_precision));
const long long fp_4 = (4 * (1LL << fixed_precision));
const long long conv_fixed_precision = (1LL << fixed_precision);

int Get_Fractal_Level(long long fp_newRe, long long fp_newIm) {
    int i;
    for (i = 0; i < 64; i++) {
        long long fp_oldRe = fp_newRe;
        long long fp_oldIm = fp_newIm;
        long long fp_sqRe = my_multiply_shift(fp_oldRe, fp_oldRe, fixed_precision);
        long long fp_sqIm = my_multiply_shift(fp_oldIm, fp_oldIm, fixed_precision);

        if ((fp_sqRe + fp_sqIm) > fp_4) break;

        fp_newRe = fp_sqRe - fp_sqIm; fp_newRe += fp_cRe;
        fp_newIm = my_multiply_shift(fp_oldRe, fp_oldIm, fixed_precision - 1); fp_newIm += fp_cIm;
    }
    return i;
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
int get_color(int level) {
    if (level > 63) level = 63;
    return ((level << 2) & 0xE0) + level / 2;
}

//void Get_Fractal_Step(long long& newRe, long long& newIm, int &level, int i, bool& flag)
//{
//	if (flag)
//		return;
//
//    long long oldRe = newRe;
//    long long oldIm = newIm;
//	long long sqRe = my_multiply_shift(oldRe, oldRe, fixed_precision);
//	long long sqIm = my_multiply_shift(oldIm, oldIm, fixed_precision);
//
//	if ((sqRe + sqIm) > fp_4) { level = i;  flag = true; return; }
//
//	newRe = sqRe - sqIm; newRe += fp_cRe;
//	newIm = my_multiply_shift(oldRe, oldIm, fixed_precision - 1); newIm += fp_cIm;
//}

void Get_Fractal_Step_ASM(long long& newRe, long long& newIm, int& level, int i, bool& flag)
{
    if (!flag) {
        long long sqRe = my_multiply_shift(newRe, newRe, fixed_precision);
        long long sqIm = my_multiply_shift(newIm, newIm, fixed_precision);
        newIm = my_multiply_shift(newRe, newIm, fixed_precision - 1) + fp_cIm;
        newRe = sqRe - sqIm + fp_cRe;

        if ((sqRe + sqIm) > fp_4) { level = i;  flag = true; }
        else { level = 64; flag = false; }
    }
}

//void Get_Fractal_Level_Vector(long long* vXY, int* vLevel)
//{
//    long long newRe[8];
//    long long newIm[8];
//
//    // INIT: 1 clk, S1: 32 clk for reading 32 * 32-bit
//    Simulator::addClock(FunctionType::GET_FRACTAL_LEVEL_VECTOR, 33);
//    for (int i = 0; i < 8; i++) {
//        newRe[i] = vXY[i * 2];
//        newIm[i] = vXY[i * 2 + 1];
//        vLevel[i] = 64;
//    }
//
//    bool f[8] = { 0 };
//    int i;
//    for (i = 0; i < 64; i++) {
//        Get_Fractal_Step(newRe[0], newIm[0], vLevel[0], i, f[0]); // instance 0
//        Get_Fractal_Step(newRe[1], newIm[1], vLevel[1], i, f[1]); // instance 1
//        Get_Fractal_Step(newRe[2], newIm[2], vLevel[2], i, f[2]); // instance 2
//        Get_Fractal_Step(newRe[3], newIm[3], vLevel[3], i, f[3]); // instance 3
//        Get_Fractal_Step(newRe[4], newIm[4], vLevel[4], i, f[4]); // instance 4
//        Get_Fractal_Step(newRe[5], newIm[5], vLevel[5], i, f[5]); // instance 5
//        Get_Fractal_Step(newRe[6], newIm[6], vLevel[6], i, f[6]); // instance 6
//        Get_Fractal_Step(newRe[7], newIm[7], vLevel[7], i, f[7]); // instance 7
//        if (f[0] & f[1] & f[2] & f[3] & f[4] & f[5] & f[6] & f[7])
//            break;
//    }
//    Simulator::addClock(FunctionType::GET_FRACTAL_LEVEL_VECTOR, i);
//}

void Get_Fractal_Level_Vector_ASM(unsigned int* dataa, unsigned int* datab)
{
    int start = 1;
    int valid = 0;
    long long newRe[8];
    long long newIm[8];
    int newLevel[8];
    int* outBaseAddr;
    long long* MAR;
    int rd, wr;
    int i, len, c, count;
    bool f[8] = {false};

    int clk_cycles = 0;

    INIT:   if (start == 0) { valid = 0; goto INIT; }
            else { valid = 0; MAR = (long long*)dataa; rd = 1; wr = 0; len = 64; i = 0; count = 8; c = 0; outBaseAddr = (int*)datab; clk_cycles++; goto S1; }

    S1:     if (c < count) { newRe[c] = *MAR; newLevel[c] = 64; MAR++; rd = 1; clk_cycles+=2; goto S2; }
            else { clk_cycles++; goto S3; }

    S2:     if (1) { newIm[c] = *MAR; MAR++; rd = 1; c++; clk_cycles+=2; goto S1; }

    S3:     if ((i < len) && !(f[0] & f[1] & f[2] & f[3] & f[4] & f[5] & f[6] & f[7])) {
                Get_Fractal_Step_ASM(newRe[0], newIm[0], newLevel[0], i, f[0]); // instance 0
                Get_Fractal_Step_ASM(newRe[1], newIm[1], newLevel[1], i, f[1]); // instance 1
                Get_Fractal_Step_ASM(newRe[2], newIm[2], newLevel[2], i, f[2]); // instance 2
                Get_Fractal_Step_ASM(newRe[3], newIm[3], newLevel[3], i, f[3]); // instance 3
                Get_Fractal_Step_ASM(newRe[4], newIm[4], newLevel[4], i, f[4]); // instance 4
                Get_Fractal_Step_ASM(newRe[5], newIm[5], newLevel[5], i, f[5]); // instance 5
                Get_Fractal_Step_ASM(newRe[6], newIm[6], newLevel[6], i, f[6]); // instance 6
                Get_Fractal_Step_ASM(newRe[7], newIm[7], newLevel[7], i, f[7]); // instance 7
                i++;
                clk_cycles++; 
                goto S3;
            }
            else { *outBaseAddr = newLevel[0]; outBaseAddr++;/*MAR = outBaseAddr; MDATA_IN = newLevel[0];*/ rd = 0; wr = 1; c = 1; clk_cycles++; goto S4; }

    S4:     if (c < count) { *outBaseAddr = newLevel[c]; outBaseAddr++; /*MAR += 4; MDATA_IN = newLevel[c];*/ rd = 0; wr = 1; c++; clk_cycles++; goto S4; }
            else { valid = 1; clk_cycles++; /*goto INIT;*/ Simulator::addClock(FunctionType::GET_FRACTAL_LEVEL_VECTOR, clk_cycles); return; }
}

/********************************************************************/
void draw_fractal(VGA& vga) {

    //if (oneTimeLog) {
    //	myfile.open("frame_color.csv");
    //}

    const double offset = 1.0 / 240.0;
    const double ratio = 320 / 240.0;

    const double xLoopOffX = offset * cosAngle / Scale;
    const double xLoopOffY = -offset * sinAngle / Scale;
    const long long fp_xLoopOffX = (xLoopOffX * (1LL << fixed_precision));
    const long long fp_xLoopOffY = (xLoopOffY * (1LL << fixed_precision));

    const double yLoopOffX = -640 * (offset * cosAngle) / Scale + offset * sinAngle / Scale;
    const double yLoopOffY = 640 * (offset * sinAngle) / Scale + offset * cosAngle / Scale;
    const long long fp_yLoopOffX = (yLoopOffX * (1LL << fixed_precision));
    const long long fp_yLoopOffY = (yLoopOffY * (1LL << fixed_precision));

    const double currX = X_position - (ratio * cosAngle + sinAngle) / Scale;
    const double currY = Y_position + (ratio * sinAngle - cosAngle) / Scale;
    long long fp_currX = (currX * (1LL << fixed_precision));
    long long fp_currY = (currY * (1LL << fixed_precision));
    
    /* -------- Original optimized code -------- */
    //for (int y = 0; y < 480; y++) {
    //	for (int x = 0; x < 640; x++) {
    //		if ((x == 320) || (y == 240)) vga.Set_Pixel_Color(x, y, 31);
    //		else {
    //			int level = Get_Fractal_Level(fp_currX, fp_currY);
    //			//if (oneTimeLog) myfile << level << ",";
    //			vga.Set_Pixel_Color(x, y, get_color(level));
    //		}
    //		fp_currX += fp_xLoopOffX;
    //		fp_currY += fp_xLoopOffY;
    //	}
    //	//if (oneTimeLog) myfile << ";\n";
    //	fp_currX += fp_yLoopOffX;
    //	fp_currY += fp_yLoopOffY;
    //}
    /* -------- Original optimized code -------- */
    
    /* -------- ASM optimized code -------- */
    long long vXY[16];
    int vLevel[640] = { 64 };
    unsigned int pixelAddr[640];

    for (int y = 0; y < 480; y++) {
        for (int x = 0; x < 640; x += 8) {
            for (int k = 0; k < 2 * 8; k += 2) {
                vXY[k] = fp_currX;
                vXY[k + 1] = fp_currY;
                fp_currX += fp_xLoopOffX;
                fp_currY += fp_xLoopOffY;
                pixelAddr[x + (k >> 1)] = x + (k >> 1);
            }
            Get_Fractal_Level_Vector_ASM((unsigned int*)vXY, (unsigned int*)&vLevel[x]);
            //Get_Fractal_Level_Vector(vXY, vLevel);

            //if (oneTimeLog) {
            //    for (int p = 0; p < 8; p++) {
            //        myfile << vLevel[p] << ",";
            //    }
            //}
        }
        vga.CI_Set_Pixel_Color(y, (unsigned int*)vLevel, pixelAddr);
        
        fp_currX += fp_yLoopOffX;
        fp_currY += fp_yLoopOffY;

        //if (oneTimeLog) myfile << ";\n";
    }

    for (int y = 0; y < 480; y++) {
        for (int x = 0; x < 640; x++) {
            if ((x == 320) || (y == 240))
                vga.Set_Pixel_Color(x, y, 31);
        }
    }

    //if (oneTimeLog) {
    //	myfile.close();
    //	oneTimeLog = false;
    //}
    
#ifdef USE_OPEN_GL
    glFlush();
#endif
}

/********************************************************************/

void parallel_multiply(unsigned int* dataa, unsigned int* datab)
{
	int start = 1;
	int valid = 0;
	long long inArray[6];
	long long outArray[8];
	long long tmpArray[3];
	long long* outBaseAddr;
	long long* MAR;
	int rd, wr;
	int i, len;

	int clk_cycles = 0;

    INIT:   if (start == 0) { valid = 0; goto INIT; }
            else { valid = 0; MAR = (long long*)dataa; rd = 1; wr = 0; len = 6; i = 0; outBaseAddr = (long long*)datab; clk_cycles++; goto S1; }

    S1:     if (i < len) { inArray[i] = *MAR; MAR++; rd = 1; clk_cycles += 2; i++; goto S1; }
            else { clk_cycles++; goto S2; }

    S2:     if (1) {
	            tmpArray[0] = my_multiply_shift(inArray[0], inArray[3], fixed_precision);
                tmpArray[1] = my_multiply_shift(inArray[1], inArray[3], fixed_precision);
                tmpArray[2] = my_multiply_shift(inArray[2], inArray[3], fixed_precision);
                clk_cycles++;
                goto S3;
            }
    S3:     if (1) {
	            outArray[0] = my_multiply_shift(tmpArray[0], tmpArray[0], fixed_precision);
                outArray[1] = my_multiply_shift(tmpArray[1], tmpArray[1], fixed_precision);
                outArray[2] = my_multiply_shift(tmpArray[2], tmpArray[2], fixed_precision);
                outArray[3] = my_multiply_shift(tmpArray[0], inArray[4], fixed_precision);
                outArray[4] = my_multiply_shift(tmpArray[1], inArray[5], fixed_precision);
                outArray[5] = my_multiply_shift(tmpArray[0], inArray[5], fixed_precision);
                outArray[6] = my_multiply_shift(tmpArray[1], inArray[4], fixed_precision);
                outArray[7] = tmpArray[2];
                clk_cycles++;
                len = 8;
                i = 0;
                goto S4;
            }

    S4:     if (i < len) { *outBaseAddr = outArray[i]; outBaseAddr++; /*MAR += 8; MDATA_IN = outArray[i];*/ rd = 0; wr = 1; i++; clk_cycles += 2; goto S4; }
            else { valid = 1; clk_cycles++; /*goto INIT;*/ Simulator::addClock(FunctionType::GET_FRACTAL_LEVEL_VECTOR, clk_cycles); return; }
}

void draw_balls(VGA& vga) {
    const int ang = round(80 * Angle / PI);
    const long long scale_fxp = Scale * (1LL << fixed_precision);
    const long long xPos_fxp = X_position * (1LL << fixed_precision);
    const long long yPos_fxp = Y_position * (1LL << fixed_precision);
    const long long cos_fxp = cosAngle * (1LL << fixed_precision);
    const long long sin_fxp = sinAngle * (1LL << fixed_precision);

    long long inputArray[6];
    long long outputArray[8];

    for (int i = 0; i < NB_BALL; i++) {
        if (r_ball_fxp[i] == 0) continue;	// the pearl has already been found
        //const long long x_fxp = my_multiply_shift(x_ball_fxp[i] - xPos_fxp, scale_fxp, fixed_precision);
        //const long long y_fxp = my_multiply_shift(y_ball_fxp[i] - yPos_fxp, scale_fxp, fixed_precision);
        //const long long r_fxp = my_multiply_shift(r_ball_fxp[i], scale_fxp, fixed_precision);

        //const long long sqX_fxp = my_multiply_shift(x_fxp, x_fxp, fixed_precision);
        //const long long sqY_fxp = my_multiply_shift(y_fxp, y_fxp, fixed_precision);
        //const long long sqR_fxp = my_multiply_shift(r_fxp, r_fxp, fixed_precision);

        //const long long i1 = my_multiply_shift(x_fxp, cos_fxp, fixed_precision);
        //const long long i2 = my_multiply_shift(y_fxp, sin_fxp, fixed_precision);
        //const long long j1 = my_multiply_shift(x_fxp, sin_fxp, fixed_precision);
        //const long long j2 = my_multiply_shift(y_fxp, cos_fxp, fixed_precision);

        inputArray[0] = x_ball_fxp[i] - xPos_fxp;
        inputArray[1] = y_ball_fxp[i] - yPos_fxp;
        inputArray[2] = r_ball_fxp[i];
        inputArray[3] = scale_fxp;
        inputArray[4] = cos_fxp;
        inputArray[5] = sin_fxp;

        parallel_multiply((unsigned int *)inputArray, (unsigned int*)outputArray);

        const long long sqX_fxp = outputArray[0];
        const long long sqY_fxp = outputArray[1];
        const long long sqR_fxp = outputArray[2];
        const long long i1 = outputArray[3];
        const long long i2 = outputArray[4];
        const long long j1 = outputArray[5];
        const long long j2 = outputArray[6];
        const long long r_fxp = outputArray[7];

        if ((sqX_fxp + sqY_fxp <= sqR_fxp)) {
            //La perle est trouvee
            r_ball_fxp[i] = 0;
            R_Ball[i] = 0;
            int time = get_time();
            printf("Found %i pearl(s) at time %i\n", ++found_pearl, time);
        }

        const long long xi = i1 - i2;
        const long long yi = j1 + j2;
        const long long ri = r_fxp;
        
        const int x_map = int((xi - (xi >> 4)) >> (fixed_precision - 8)) + 320; // int x_map = int(((xi << 8) - (xi << 4)) >> fixed_precision) + 320;
        const int y_map = int((yi - (yi >> 4)) >> (fixed_precision - 8)) + 240; // int y_map = int(((yi << 8) - (yi << 4)) >> fixed_precision) + 240;
        const int r_map = int((ri - (ri >> 4)) >> (fixed_precision - 8));       // int r_map = int(((ri << 8) - (ri << 4)) >> fixed_precision);      

        if (x_map + r_map < 0) continue;
        if (x_map - r_map >= 640) continue;
        if (y_map + r_map < 0) continue;
        if (y_map - r_map >= 480) continue;

        drawBall3D(vga, x_map, y_map, r_map, i % 7);
    }

    Simulator::printTotal();
#ifdef USE_OPEN_GL
    glFlush();
#endif
}

