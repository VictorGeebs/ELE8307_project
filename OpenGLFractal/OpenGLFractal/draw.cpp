#include "draw.h"

void drawCircle(VGA & vga, int x, int y, int radius, int rgb) {
   int dx = 0;
   int dy = radius;
   int p = 1 - radius;

   rgb=(rgb<<5) + 31;

   while (dx < dy) {
       int xhi = x + dx;
       int xlo = x - dx;
       int yhi = y + dy;
       int ylo = y - dy;

       vga.Set_Pixel_Color(xhi,yhi,rgb);
       vga.Set_Pixel_Color(xhi,ylo,rgb);
       vga.Set_Pixel_Color(xlo,yhi,rgb);
       vga.Set_Pixel_Color(xlo,ylo,rgb);

       xhi = x + dy;
       xlo = x - dy;
       yhi = y + dx;
       ylo = y - dx;

       vga.Set_Pixel_Color(xhi,yhi,rgb);
       vga.Set_Pixel_Color(xhi,ylo,rgb);
       vga.Set_Pixel_Color(xlo,yhi,rgb);
       vga.Set_Pixel_Color(xlo,ylo,rgb);

       dx++;
       if (p < 0) p += ((dx << 1) + 1);
       else {
          dy--;
          p += ((dx - dy + 1) << 1);
       }
   }
}

void drawBall3D(VGA & vga, int x, int y, int radius, int rgb) {
   int dx = 0;
   int dy = radius;
   int d2z = 0;
   int dz = 0;
   int color;
   
   int nrgb = rgb << 5;
   
   if (radius <2) return;

   while (dx <= dy) {
      int back_d2z = d2z;
      int back_dy = dy;
      while (dx <= dy) {
         dz = (int)(sqrtf(d2z)*31.0f)/(float)radius;
         
         if (dz > 5) {
             color = nrgb + dz;
                
             int xhi = x + dx;
             int xlo = x - dx;
             int yhi = y + dy;
             int ylo = y - dy;
               
             vga.Set_Pixel_Color(xhi,yhi,color);
             vga.Set_Pixel_Color(xhi,ylo,color);
             vga.Set_Pixel_Color(xlo,yhi,color);
             vga.Set_Pixel_Color(xlo,ylo,color);
               
             xhi = x + dy;
             xlo = x - dy;
             yhi = y + dx;
             ylo = y - dx;
               
             vga.Set_Pixel_Color(xhi,yhi,color);
             vga.Set_Pixel_Color(xhi,ylo,color);
             vga.Set_Pixel_Color(xlo,yhi,color);
             vga.Set_Pixel_Color(xlo,ylo,color);
         }         
         d2z += (dy+dy-1);
         dy--;
      }
      d2z = back_d2z;
      dy = back_dy;
      dx++; d2z -= (dx+dx-1);
      if (d2z < 0) {
         dy--; d2z += (dy+dy+1);
      }
   }    
}

void drawTriangle(VGA & vga, int x0, int y0, int x1, int y1, int x2, int y2, int rgb, int intensity) {
    int color = (rgb << 5) + intensity;

    int a0 = y0-y1; int b0 = x1-x0; int c0 = -(x0*a0+y0*b0);
    int a1 = y1-y2; int b1 = x2-x1; int c1 = -(x1*a1+y1*b1);
    int a2 = y2-y0; int b2 = x0-x2; int c2 = -(x2*a2+y2*b2);
    
    int center_x = (x0+x1+x2)/3;
    int center_y = (y0+y1+y2)/3;
    
    int delta0 = a0*center_x+b0*center_y+c0;
    if (delta0 == 0) return; 
    else if (delta0 < 0) {
        a0 = -a0; b0 = -b0; c0 = -c0; delta0 = -delta0;
    } 
    
    int delta1 = a1*center_x+b1*center_y+c1;
    if (delta1 == 0) return; 
    else if (delta1 < 0) {
        a1 = -a1; b1 = -b1; c1 = -c1; delta1 = -delta1;
    }
    
    int delta2 = a2*center_x+b2*center_y+c2;
    if (delta2 == 0) return; 
    else if (delta2 < 0) {
        a2 = -a2; b2 = -b2; c2 = -c2; delta2 = -delta2;
    }

    int x_min = x0; if (x_min>x1) x_min = x1; if (x_min>x2) x_min = x2;
    int x_max = x0; if (x_max<x1) x_max = x1; if (x_max<x2) x_max = x2;
    int y_min = y0; if (y_min>y1) y_min = y1; if (y_min>y2) y_min = y2;
    int y_max = y0; if (y_max<y1) y_max = y1; if (y_max<y2) y_max = y2;
    
    if (x_min < 0) x_min = 0;
    if (x_max > VGA_WIDTH-1) x_max = VGA_WIDTH-1;
    if (y_min < 0) y_min = 0;
    if (y_max > VGA_HEIGHT-1) y_max = VGA_HEIGHT-1;
    
    if (x_min > x_max) return;
    if (y_min > y_max) return;

    int delta_ref0 = a0/2; if (delta_ref0 > 0) delta_ref0 = -delta_ref0;
    int delta_ref1 = a1/2; if (delta_ref1 > 0) delta_ref1 = -delta_ref1;
    int delta_ref2 = a2/2; if (delta_ref2 > 0) delta_ref2 = -delta_ref2;

    int x = x_min;
    int y = y_min;
    
    delta0 = a0*x+b0*y+c0;
    delta1 = a1*x+b1*y+c1;
    delta2 = a2*x+b2*y+c2;

    for (;;) {
        for (;;) {
            if ((delta0 >= delta_ref0) && (delta1 >= delta_ref1) && (delta2 >= delta_ref2)) {
                vga.Set_Pixel_Color(x,y,color);
            }
            if (x==x_max) break;
            x++;
            delta0+=a0;
            delta1+=a1;
            delta2+=a2;
        }
        if (y == y_max) break;
        y++;
        delta0+=b0;
        delta1+=b1;
        delta2+=b2;
        for (;;) {
            if ((delta0 >= delta_ref0) && (delta1 >= delta_ref1) && (delta2 >= delta_ref2)) {
                vga.Set_Pixel_Color(x,y,color);
            }
            if (x==x_min) break;
            x--;
            delta0-=a0;
            delta1-=a1;
            delta2-=a2;
        }
        if (y == y_max) break;
        y++;
        delta0+=b0;
        delta1+=b1;
        delta2+=b2;
    }
}
