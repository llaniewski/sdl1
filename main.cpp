// g++ main.cpp `pkg-config --cflags --libs sdl2`
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "graphics_sdl.h"

int main() {
	graphics(300,300);
	clear();
	line(0,0,300,300);
	line(0,300,300,0);
	circle(150,150,150);
	while(animate(15)) {};
	return 0;
}










