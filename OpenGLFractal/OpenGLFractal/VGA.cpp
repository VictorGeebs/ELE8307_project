#include "VGA.h"
#include "io.h"

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
