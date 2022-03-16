void mult64to128(const unsigned long long u, const unsigned long long v, unsigned long long& h, unsigned long long& l)
{
	// clk 1
	const unsigned long long uL = (u & 0xffffffff);
	const unsigned long long vL = (v & 0xffffffff);
	const unsigned long long uH = (u >> 32);
	const unsigned long long vH = (v >> 32);
	
	// clk 2
	const unsigned long long z1 = uL * vL;
	const unsigned long long z2 = uH * vL;
	const unsigned long long z3 = uL * vH;
	const unsigned long long z4 = uH * vH;

	// clk 3
	const unsigned long long s = (z1 >> 32) + (z2 & 0xffffffff) + (z3 & 0xffffffff);
	
	// clk 4
	h = (s >> 32) + (z2 >> 32) + (z3 >> 32) + z4;
	l = (s << 32) | (z1 & 0xffffffff);
}

long long my_multiply_shift(long long a, long long b, int shift_value) {
	unsigned long long u; 
	unsigned long long v;
	unsigned long long uL;
	unsigned long long vL;
	unsigned long long uH;
	unsigned long long vH;
	unsigned long long z1;
	unsigned long long z2;
	unsigned long long z3;
	unsigned long long z4;
	unsigned long long tmp;
	unsigned long long high;
	unsigned long long low
	long long result;
	bool sign;

	// clk 1
	u = (a < 0) ? -a : a;
	v = (b < 0) ? -b : b;
	sign = (a < 0) ^ (b < 0);

	// clk 2
	uL = (u & 0xffffffff);
	vL = (v & 0xffffffff);
	uH = (u >> 32);
	vH = (v >> 32);
	
	// clk 3
	z1 = uL * vL;
	z2 = uH * vL;
	z3 = uL * vH;
	z4 = uH * vH;

	// clk 4
	tmp = (z1 >> 32) + (z2 & 0xffffffff) + (z3 & 0xffffffff);
	
	// clk 5
	high = (tmp >> 32) + (z2 >> 32) + (z3 >> 32) + z4;
	low = (tmp << 32) | (z1 & 0xffffffff);
	
	// clk 6
	result = sign ? -((low >> 49) + (high << 15)) : ((low >> 49) + (high << 15));

	return result;
}

void my_multiply_shift(unsigned int dataa, unsigned int datab) { // dataa: base address, datab: length
    static unsigned int* table;
    static unsigned int* MAR;
	static unsigned int MDATA;
	static unsigned long long uL;
	static unsigned long long vL;
	static unsigned long long uH;
	static unsigned long long vH;
	static unsigned long long z1;
	static unsigned long long z2;
	static unsigned long long z3;
	static unsigned long long z4;
	static unsigned long long tmp;
	static unsigned long long high;
	static unsigned long long low
	static bool sign;

	INIT: if (start == 0) { valid = 0; goto INIT; }
	      else { valid = 0; table = (unsigned int*) dataa; length = datab; MAR = (unsigned int*) dataa; i = 0; goto S1; }
          
	S1:   if (i < length) { uL = *MAR; MAR = table + 1; i++; goto S2; }
          else { valid = 1; goto INIT; }	
		  
	S2:   if (1) { uH = *MAR; MAR = table + 2; goto S3; }
	      
	S3:   if (1) { vL = *MAR; MAR = table + 3; goto S4; }
	      
	S4:   if (1) { vH = *MAR; goto S5; }
	
	//  for S5 and S6:
    // 	sign = (a < 0) ^ (b < 0);
	//  u = (a < 0) ? -a : a;
	//  v = (b < 0) ? -b : b;
	S5:   if ((uH:uL) < 0) { (uH:uL) = -(uH:uL); sign = 1; goto S6; }
	      else { sign = 0; goto S6; }
	
	S6:   if ((vH:vL) < 0) { (vH:vL) = -(vH:vL); sign = !sign; goto S7; }
	      else { goto S7; }
		  
	S7:   if (1) { z1 = uL * vL; z2 = uH * vL; z3 = uL * vH; z4 = uH * vH; goto S8; }

    S8:   if (1) { tmp = (z1 >> 32) + (z2 & 0xffffffff) + (z3 & 0xffffffff); goto S9; }
	
	S9:   if (1) { high = (tmp >> 32) + (z2 >> 32) + (z3 >> 32) + z4; low = (tmp << 32) | (z1 & 0xffffffff); goto S10; }
	
	// for S10: result = sign ? -((low >> 49) + (high << 15)) : ((low >> 49) + (high << 15));
	S10:  if (sign == 1) { (high:low) = -((low >> 49) + (high << 15)) : ((low >> 49) + (high << 15)); goto S11; }
	      else { (high:low) = ((low >> 49) + (high << 15)) : ((low >> 49) + (high << 15)); goto S11; }
	
	S11:  if (1) { MAR = table; MDATA = low; goto S12; }
	
	S12:  if (1) { MAR = table + 1; MDATA = high; table += 4; goto S1; }
	
	S13:  if (1) { MAR = table; goto S1; }
}





