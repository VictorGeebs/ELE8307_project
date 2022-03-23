// Get_Fractal_Step: C code
void Get_Fractal_Step(long long& newRe, long long& newIm, int &level, int i, bool& flag)
{
	if (flag)
		return;

    long long oldRe = newRe;
    long long oldIm = newIm;
	long long sqRe = my_multiply_shift(oldRe, oldRe, fixed_precision);
	long long sqIm = my_multiply_shift(oldIm, oldIm, fixed_precision);

	if ((sqRe + sqIm) > fp_4) { level = i;  flag = true; return; }

	newRe = sqRe - sqIm; newRe += fp_cRe;
	newIm = my_multiply_shift(oldRe, oldIm, fixed_precision - 1); newIm += fp_cIm;
}

// Get_Fractal_Step: ASM
void Get_Fractal_Step_ASM(long long& newRe, long long& newIm, int &level, int i, bool& flag)
{
    if (flag) {
        long long sqRe = my_multiply_shift(newRe, newRe, fixed_precision);
        long long sqIm = my_multiply_shift(newIm, newIm, fixed_precision);
        newIm = my_multiply_shift(newRe, newIm, fixed_precision - 1) + fp_cIm;
        newRe = sqRe - sqIm + fp_cRe;
        
        if ((sqRe + sqIm) > fp_4) { level = i;  flag = true; }
        else { level = 64; flag = false; }
    }
}

// Get_Fractal_Level_Vector: C code
void Get_Fractal_Level_Vector(long long* vXY, int* vLevel)
{
    long long newRe[8];
    long long newIm[8];

    // INIT: 1 clk, S1: 32 clk for reading 32 * 32-bit
    for (int i = 0; i < 8; i++) {
        newRe[i] = vXY[i * 2];
        newIm[i] = vXY[i * 2 + 1];
        vLevel[i] = 64;
    }

    bool f[8] = { 0 };
    int i;
    for (i = 0; i < 64; i++) {
        Get_Fractal_Step(newRe[0], newIm[0], vLevel[0], i, f[0]); // instance 0
        Get_Fractal_Step(newRe[1], newIm[1], vLevel[1], i, f[1]); // instance 1
        Get_Fractal_Step(newRe[2], newIm[2], vLevel[2], i, f[2]); // instance 2
        Get_Fractal_Step(newRe[3], newIm[3], vLevel[3], i, f[3]); // instance 3
        Get_Fractal_Step(newRe[4], newIm[4], vLevel[4], i, f[4]); // instance 4
        Get_Fractal_Step(newRe[5], newIm[5], vLevel[5], i, f[5]); // instance 5
        Get_Fractal_Step(newRe[6], newIm[6], vLevel[6], i, f[6]); // instance 6
        Get_Fractal_Step(newRe[7], newIm[7], vLevel[7], i, f[7]); // instance 7
        if (f[0] & f[1] & f[2] & f[3] & f[4] & f[5] & f[6] & f[7])
            break;
    }
}

// Get_Fractal_Level_Vector: ASM
void Get_Fractal_Level_Vector_ASM(unsigned int *dataa, unsigned int *datab)
{
    long long newRe[8];
    long long newIm[8];
    int newLevel[8];
    int *outBaseAddr;

    INIT: if (start == 0) { valid = 0; goto INIT; }
          else { valid = 0; MAR = dataa; rd = 1; wr = 0; len = 64; i = 0; count = 8; c = 1; outBaseAddr = datab; goto S1; }
          
    S1:   if (c <= count) { newRe[c] = MDATA_OUT(63:0); newIm[c] = MDATA_OUT(127:64); newLevel[c] = 64; MAR += 8; rd = 1; c++; goto S2; }
          else { goto S1; }
    
    S2:   if (i < len && f != 0xff) { 
              Get_Fractal_Step_ASM(newRe[0], newIm[0], newLevel[0], i, f[0]); // instance 0
              Get_Fractal_Step_ASM(newRe[1], newIm[1], newLevel[1], i, f[1]); // instance 1
              Get_Fractal_Step_ASM(newRe[2], newIm[2], newLevel[2], i, f[2]); // instance 2
              Get_Fractal_Step_ASM(newRe[3], newIm[3], newLevel[3], i, f[3]); // instance 3
              Get_Fractal_Step_ASM(newRe[4], newIm[4], newLevel[4], i, f[4]); // instance 4
              Get_Fractal_Step_ASM(newRe[5], newIm[5], newLevel[5], i, f[5]); // instance 5
              Get_Fractal_Step_ASM(newRe[6], newIm[6], newLevel[6], i, f[6]); // instance 6
              Get_Fractal_Step_ASM(newRe[7], newIm[7], newLevel[7], i, f[7]); // instance 7
              i++; 
              goto S2; }
          else { MAR = outBaseAddr; MDATA_IN = newLevel[0]; rd = 0; wr = 1; c = 1; goto S3; }
    
    S3:   if (c <= count) { MAR += 4; MDATA_IN = newLevel[c]; rd = 0; wr = 1; c++; goto S3; }
          else { valid = 1; goto INIT; }
}
