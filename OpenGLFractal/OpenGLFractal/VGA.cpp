#include "VGA.h"
#include "io.h"

void VGA::ci_Set_Pixel_Color(int n, unsigned int dataa, unsigned int datab)
{
	int start = 1;		// Not really used, just for simulation purpose in C/C++
	int valid = 0;		// Not really used, just for simulation purpose in C/C++
	static unsigned int* pixels;
	static unsigned int* colors;
	static unsigned int* MAR;
	static unsigned int MDATA_IN;
	static unsigned int MDATA_OUT;
	static unsigned int WR;
	
	static int i;
	static int length;
	static unsigned int curr_color;

INIT:   if (start == 0) { valid = 0; goto INIT; }
else { valid = 0; pixels = (unsigned int*)dataa; colors = (unsigned int*)datab; MAR = (unsigned int*)datab; WR = 0; i = 0; length = 480;  goto S1; }

S1:     if (i < length) { curr_color = MDATA_IN; MAR = pixels + i; WR = 1; i++; goto S2; }
	else { valid = 1;}

S2:     if (1) { MDATA_OUT = curr_color; MAR = colors + i; WR = 0; goto S1; }

}

//-------------------------------------------------------------------------
void VGA::clr() {
	int x, y;
    for (x=0;x<640;x++) {
	  for (y=0;y<480;y++) {
		  Set_Pixel_Color(x,y,x%256);
	  }
  }
}

/*********************************************************
	ecran2d_setColorOpenGL()

 *********************************************************/
void VGA::ecran2d_setColorOpenGL(int color) {
#ifdef USE_OPEN_GL
	float intensite;
	intensite = (color % 32) / (31.0f);

	switch (color / 32) {
	case COLOR_WHITE:
		glColor3f(intensite, intensite, intensite);
		break;
	case COLOR_RED:
		glColor3f(intensite, 0.0, 0.0);
		break;
	case COLOR_GREEN:
		glColor3f(0.0, intensite, 0.0);
		break;
	case COLOR_BLUE:
		glColor3f(0.0, 0.0, intensite);
		break;
	case COLOR_YELLOW:
		glColor3f(intensite, intensite, 0.0);
		break;
	case COLOR_BROWN:
		glColor3f(intensite * 0.5, intensite * 0.25, intensite * 0.0625);
		break;
	case COLOR_CYAN:
		glColor3f(0.0, intensite, intensite);
		break;
	case COLOR_PURPLE:
		glColor3f(intensite, 0.0, intensite);
		break;
	default:
		glColor3f(1.0, 1.0, 1.0);
		break;
	}
#endif
}
