/*  The written ASM is equivalent to the following C++ code  */
       
	const long long x_fxp = my_multiply_shift(x_ball_fxp[i] - xPos_fxp, scale_fxp, fixed_precision);
    const long long y_fxp = my_multiply_shift(y_ball_fxp[i] - yPos_fxp, scale_fxp, fixed_precision);
    const long long r_fxp = my_multiply_shift(r_ball_fxp[i], scale_fxp, fixed_precision);

    const long long sqX_fxp = my_multiply_shift(x_fxp, x_fxp, fixed_precision);
    const long long sqY_fxp = my_multiply_shift(y_fxp, y_fxp, fixed_precision);
    const long long sqR_fxp = my_multiply_shift(r_fxp, r_fxp, fixed_precision);

    const long long i1 = my_multiply_shift(x_fxp, cos_fxp, fixed_precision);
    const long long i2 = my_multiply_shift(y_fxp, sin_fxp, fixed_precision);
    const long long j1 = my_multiply_shift(x_fxp, sin_fxp, fixed_precision);
    const long long j2 = my_multiply_shift(y_fxp, cos_fxp, fixed_precision);
		
// --------------------------- ASM for parallel multiplication in draw_ball ---------------------------
void parallel_multiply(unsigned int *dataa, unsigned int *datab)
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
            else { valid = 0; MAR = (long long*)dataa; rd = 1; wr = 0; len = 6; i = 0; outBaseAddr = (long long*)datab; goto S1; }
    
    S1:     if (i < len) { inArray[i] = *MAR; MAR += 8; rd = 1; i++; goto S1; }
            else { goto S2; }
    
    S2:     if (1) {
                tmpArray[0] = inArray[0] * inArray[3];
                tmpArray[1] = inArray[1] * inArray[3];
                tmpArray[2] = inArray[2] * inArray[3];
                goto S3; }
				
    S3:     if (1) {
                outArray[0] = tmpArray[0] * tmpArray[0];
                outArray[1] = tmpArray[1] * tmpArray[1];
                outArray[2] = tmpArray[2] * tmpArray[2];
                outArray[3] = tmpArray[0] * inArray[4];
                outArray[4] = tmpArray[1] * inArray[5];
                outArray[5] = tmpArray[0] * inArray[5];
                outArray[6] = tmpArray[1] * inArray[4];
                outArray[7] = tmpArray[2];
                len = 8; 
                i = 0;
                goto S4; }
    
    S4:     if (i < len) { MAR += 8; MDATA_IN = outArray[i]; rd = 0; wr = 1; i++; goto S4; }
            else { valid = 1; clk_cycles++; goto INIT; }
}