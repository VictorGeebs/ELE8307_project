#include "VGA.h"
#include "io.h"

int set_pixel_clk_cycles = 0;

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

void VGA::CI_Set_Pixel_Color(int n, unsigned int* dataa, unsigned int* datab) {
	int start = 1;		// Not really used, just for simulation purpose in C/C++
	int valid = 0;		// Not really used, just for simulation purpose in C/C++
	static int* colors;
	static unsigned int* pixels;
	static unsigned int* MAR;
	static unsigned int WR;
	static unsigned int MDATA;
	static int length;
	static int i;
	int y;
	int x;

	int set_pixel_clk_cycles = 0;

    INIT:   if (start == 0) { valid = 0; goto INIT; }
            else { valid = 0; colors = (int *)dataa; pixels = datab; length = 640;  i = 0;
	MAR = (unsigned int*)dataa; WR = 0; set_pixel_clk_cycles++; y = n; goto S1;
	}

    S1:     if (i < length) { MAR = pixels + i; WR = 1; x = pixels[i]; ecran2d_setColorOpenGL(get_color__(colors[i]));
                              glBegin(GL_POINTS); glVertex2f(x, y); glEnd(); i++; set_pixel_clk_cycles++; goto S2; }
            else { valid = 1; Simulator::addClock(FunctionType::SET_PIXEL, set_pixel_clk_cycles); return; }

     S2:    if (1) { MAR = (unsigned int *)colors + i; WR = 0; set_pixel_clk_cycles++; goto S1; }

}
