#ifndef DRAW_H
#define DRAW_H

#include <math.h>
#include "VGA.h"

void drawCircle(VGA & vga, int x, int y, int radius, int rgb);
void drawBall3D(VGA & vga, int x, int y, int radius, int rgb);
void drawTriangle(VGA & vga, int x0, int yo, int x1, int y1, int x2, int y2, int rgb, int intensity);

#endif //DRAW_H
