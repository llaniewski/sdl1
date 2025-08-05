// g++ main.cpp `pkg-config --cflags --libs sdl2`
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "graphics_sdl.h"

int main() {
	graphics(400,400);
	slow();
	line(50,50,350,350);
	line(50,350,350,50);
	circle(200,200,150);
	wait();
	return 0;
}
