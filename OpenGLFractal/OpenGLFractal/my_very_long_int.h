#define NB_32BITS_WORDS 4				//Number of 32-bit words to represent the fixed point number
#define my_verly_long_precision 49		//Number of bits after the point
typedef unsigned int my_very_long_int[NB_32BITS_WORDS];
void my_addition(my_very_long_int RESULT, my_very_long_int X, my_very_long_int Y);
void my_subtraction(my_very_long_int RESULT, my_very_long_int X, my_very_long_int Y);
void my_copy(my_very_long_int RESULT, my_very_long_int X);
void my_negate(my_very_long_int RESULT, my_very_long_int X);
unsigned int my_positive(my_very_long_int X);
void my_asr(my_very_long_int RESULT, my_very_long_int X, int shift_value, int LOCAL_NB_32BITS_WORDS);
void my_multiply_shift(my_very_long_int temp_RESULT, my_very_long_int temp_X, my_very_long_int temp_Y, int shift_value);
void my_init_from_double(my_very_long_int X, double d);
double my_to_double(my_very_long_int X);
