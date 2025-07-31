// g++ main.cpp `pkg-config --cflags --libs sdl2`
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "graphics_sdl.h"

int main() {
	graphics(400,400);
	slow();
	clear();
	// line(50,50,350,350);
	// line(50,350,350,50);
	// circle(200,200,50);
	// circle(200,200,100);
	// circle(200,200,150);
	text(200,200,20,"Some text");
	text(250,220,20,"Some text");
	text(300,240,20,"Some text");
	text(350,260,20,"Some text");
	wait();
	return 0;
}










