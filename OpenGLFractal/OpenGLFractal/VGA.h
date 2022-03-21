#ifndef   VGA_H
#define   VGA_H

#ifdef USE_OPEN_GL
#include "GL/glut.h"
#else
#include "system.h"
#endif
#include "io.h"

//  VGA Parameter
#define VGA_WIDTH     640
#define VGA_HEIGHT    480

#define COLOR_WHITE 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_BLUE 3
#define COLOR_YELLOW 4
#define COLOR_BROWN 5
#define COLOR_CYAN 6
#define COLOR_PURPLE 7

#define VGA_LEFT ((VGA_WIDTH-VGA_HEIGHT)/2)
#define VGA_RIGHT (VGA_WIDTH - VGA_LEFT - 1)
#define VGA_TOP 0
#define VGA_BOT (VGA_HEIGHT - 1)

//-------------------------------------------------------------------------
class VGA {
	int system_base_address;
	int memory_address;

public:
	void ecran2d_setColorOpenGL(int color);
	VGA(int system_base_address, int memory_address) : system_base_address(system_base_address), memory_address(memory_address) {};

	inline void Set_memory_address(int current_display){
		memory_address = current_display;
	}

	inline void Set_Pixel_Color(unsigned int x, unsigned int y, unsigned int color) {
		if ( (x>=640) || (y>=480) ) return;
#ifdef USE_OPEN_GL
		ecran2d_setColorOpenGL(color);
		glBegin(GL_POINTS);
		glVertex2f(x, y);
		glEnd();
#else
		int address = 640 * y + x;
		IOWR_8DIRECT(memory_address, address, color);
#endif
	}

	inline void CI_Set_Pixel_Color(int n, unsigned int dataa, unsigned int datab) {
		int start = 1;		// Not really used, just for simulation purpose in C/C++
		int valid = 0;		// Not really used, just for simulation purpose in C/C++
		static unsigned int* colors; 	//idem
		static unsigned int* pixels; 	//idem
		static unsigned int* MAR; 		//idem
		static unsigned int WR;
		static unsigned int MDATA;
		static int length; 				//idem
		static int i;					//idem

		INIT:   if (start == 0) { valid = 0; goto INIT; }
		else { valid = 0; colors = (unsigned int*)dataa; pixels = (unsigned int*)datab; length = 480;  i = 0;
			   MAR = (unsigned int*)dataa; WR = 0; goto S1;
		}

	S1:     if (i < length) { MAR = pixels + i; WR = 1; i++; goto S2; }
		else { valid = 1;}

		S2:     if (1) { MAR = colors + i; WR = 0; goto S1; }

	}

	inline void Set_Pixel(unsigned int x, unsigned int y) {
		Set_Pixel_Color(x, y, 255);
	}

	inline void Clr_Pixel(unsigned int x, unsigned int y) {
		Set_Pixel_Color(x, y, 192);
	}

	inline void send_to_display() {
#ifndef USE_OPEN_GL
		IOWR(system_base_address,0,memory_address);
#endif
	}

	void clr();
};

#endif //VGA_H
