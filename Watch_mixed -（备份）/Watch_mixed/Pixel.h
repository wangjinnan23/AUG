#ifndef  PIXEL_H
#define  PIXEL_H

typedef float Pixel_32f_1;
typedef ushort Pixel_16u_1;
typedef uchar Pixel_8u_1;
typedef struct _Pixel_32f_3 {
	float r, g, b;
} Pixel_32f_3;
typedef struct _Pixel_16u_3 {
	ushort r, g, b;
} Pixel_16u_3;
typedef struct _Pixel_8u_3 {
	uchar r, g, b;
} Pixel_8u_3;

typedef Pixel_32f_1 Pixelf;
typedef Pixel_8u_1 Pixel;

#endif
