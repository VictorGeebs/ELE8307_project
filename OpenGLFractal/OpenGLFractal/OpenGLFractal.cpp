#ifdef USE_OPEN_GL
#include "GL/glut.h"
#define VGA_0_BASE 0
#else
#include "altera_avalon_pio_regs.h"
#include "system.h"
#define PIO_0_BASE 0x4041080 //quick patch
#endif

#include "stdio.h"
#include <stdlib.h> 
#include "math.h"

#include "math_constants.h"
#include "VGA.h"
#include "draw.h"
#include "Fractal.h"

unsigned char VGA_Memory[VGA_WIDTH * VGA_HEIGHT * 2 + 15]; //must be aligned on 128 bits (16 bytes) so reserve 15 more bytes
unsigned char* VGA1 = (unsigned char*)((int)(VGA_Memory + 15) & 0xFFFFFF0);
unsigned char* VGA2 = VGA1 + VGA_WIDTH * VGA_HEIGHT;
VGA display1(VGA_0_BASE, (int)VGA1);
VGA display2(VGA_0_BASE, (int)VGA2);
VGA* current_VGA = &display1;

double Speed;
int button;

/********************************************************************/
double random_double() {
	double result = rand(); result /= RAND_MAX;
	result += rand(); result /= RAND_MAX;
	result += rand(); result /= RAND_MAX;
	return result;
}

/********************************************************************/
//Attention, cette fonction sera modifiee juste avant la demonstration
/********************************************************************/
void init() {
	int i;
	current_VGA->clr();
	current_VGA->send_to_display();

	double ball_scale = 0.10;

	srand(12345);

	for (i = 0; i < NB_BALL; i++) {
		X_Ball[i] = random_double() - 0.5;
		Y_Ball[i] = random_double() - 0.5;
		R_Ball[i] = random_double() * ball_scale;
		ball_scale *=0.8;

		printf("Put a ball at [%f,%f,%f]\n", X_Ball[i], Y_Ball[i], R_Ball[i]);
	}

	X_Ball[0] = 0;
	Y_Ball[0] = -0.6;

	found_pearl = 0;
	X_position = 0;
	Y_position = 0;
	Angle = 0;
	sinAngle = sin(Angle);
	cosAngle = cos(Angle);
	Scale = 1;
	Speed = 2.0/VGA_HEIGHT*2;
	time_ms = get_time();

	initPreCalculation();
	printf("Init done\n");
}

/********************************************************************/
void update_position() {
#ifndef USE_OPEN_GL
	button = IORD_ALTERA_AVALON_PIO_DATA(PIO_0_BASE);
#endif
	printf("Button is %x\n", button);

	if ((button & 8) == 0) {
		Angle += PI / 80;
		printf("Turn left\n");
	}
	if ((button & 4) == 0) {
		Scale *= 1.1;
		printf("Scale factor is %f\n", Scale);
	}
	if ((button & 2) == 0) {
		Scale /= 1.1;
		printf("Scale factor is %f\n", Scale);
	}
	if ((button & 1) == 0) {
		Angle -= PI / 80;
		printf("Turn right\n");
	}
	button = 0xFF;

	sinAngle = sin(Angle);
	cosAngle = cos(Angle);
	X_position -= sinAngle * Speed / Scale;
	Y_position -= cosAngle * Speed / Scale;
}

/********************************************************************/
void iterate() {
	static int step = 0;
	update_position();
	draw_fractal(*current_VGA);
	draw_balls(*current_VGA);

#ifdef USE_OPEN_GL
	glFlush();
#else
	current_VGA->send_to_display();
	if (current_VGA == &display1) current_VGA = &display2;
	else  current_VGA = &display1;
#endif

	printf("Step %i\n", step++);
}

#ifdef USE_OPEN_GL
/********************************************************************
 processNormalKeys
 ********************************************************************/
void processNormalKeys(unsigned char key, int x, int y) {
}

/********************************************************************
	processSpecialKeys()
 ********************************************************************/
void processSpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		button = 0xF7;	// PIO 8
		printf("Left \n");
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		button = 0xFE;	// PIO 1
		printf("Right \n");
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		button = 0xFB;	// PIO 4
		printf("Up \n");
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		button = 0xFD;	// PIO 2
		printf("Down \n");
		glutPostRedisplay();
		break;
	}
}

/********************************************************************
	display()
	Fonction d'affichage openGL
 ********************************************************************/
void display(void)
{
	if (glutGet(GLUT_ELAPSED_TIME) - time_ms > 100) {
		time_ms = glutGet(GLUT_ELAPSED_TIME);
		printf("time is %i ms \n", glutGet(GLUT_ELAPSED_TIME));
		iterate();
	}
}

/********************************************************************
	run_loop()
 ********************************************************************/
void run_loop(void) {
	glutPostRedisplay();
}
#endif

/********************************************************************/
int main(int argc, char** argv)
{
#ifdef USE_OPEN_GL
	//on initialise glut
	glutInit(&argc, argv);

	//on spécifie l'emploi du modèle chromatique et du double tampon
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);		// Single buffering

	//on spécifie la taille de la fenêtre
	glutInitWindowSize(640, 480);

	//on spécifie la position de la fenêtre
	glutInitWindowPosition(100, 100);

	//on crée une fenêtre
	glutCreateWindow("ELE8307 : Fractales 2022");

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 640, 480, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_FLAT);
	glClear(GL_COLOR_BUFFER_BIT);

	glutDisplayFunc(display);
	glutIdleFunc(run_loop);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	button = 0xFF;
#endif

	//on initialise
	init();

	printf("Entering main\n");
	printf("VGA buffer 1 address : %x\n", (int)VGA1);
	printf("VGA buffer 2 address : %x\n", (int)VGA2);

#ifdef USE_OPEN_GL
	glutMainLoop();
#else
	int i;
	for (i = 0; i < 1000; i++) {
		iterate();
	}
	printf("Exiting main()... \n");
#endif
	return 0;
}
