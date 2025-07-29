#include "graphics_sdl.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
const double pi = 4*atan(1.0);
const double twopi = 2*pi;

window DefaultWindow;

window::window() {
    initialised = false;
	sdl_window = NULL;
	sdl_renderer = NULL;
	for (int i = 0; i < 256; i++) {
		kbd_tab[i] = false;
	}
}

window::~window() {
    if (initialised) {
        SDL_Quit();
    }
};

void window::graphics(int sx, int sy) {
    initialised = true;
	sdl_window = SDL_CreateWindow("Graphical Window", 10, 10, sx, sy, false);
	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
}

int window::animate(double fps) {
	SDL_RenderPresent( sdl_renderer );
	const double ticks_per_second = 1000.;
	double targettime = ticks_per_second/fabs(fps);
	const double maxdelay = 60 * ticks_per_second;
	typedef Uint64 ticks_t;
	static ticks_t prev_time = 0;
	ticks_t time_now = SDL_GetTicks64();
	double time_left = prev_time + targettime - time_now;
	if (time_left > maxdelay) time_left = maxdelay;
	if (time_left > 0) {
		SDL_Delay(time_left);
		prev_time = prev_time + targettime;
	} else {
		prev_time = time_now;
	}

	SDL_Event event;
    int ret = 1;
	while( SDL_PollEvent(&event) )
	{
		if (event.type == SDL_QUIT) {
            exit(0);
			ret = 0;
		}
		if (event.type == SDL_KEYDOWN) {
			int key = event.key.keysym.sym;
			is_pressed(key) = true;
		}
		if (event.type == SDL_KEYUP) {
			int key = event.key.keysym.sym;
			is_pressed(key) = false;
		}
	}
	return ret;
}

void window::clear() {
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
}

void window::line(double x1, double y1, double x2, double y2) {
	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(sdl_renderer, x1, y1, x2, y2);
}

void window::circle(double x0, double y0, int r) {
	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
	double da = 3.0/r;
	double x = x0 + r;
	double y = y0;
	for(double a=0; a<twopi; a+=da) {
		double nx = x0+cos(a)*r;
		double ny = y0+sin(a)*r;
		SDL_RenderDrawLine(sdl_renderer, x,y,nx,ny);
		x = nx;
		y = ny;
	}
}

void window::setcolor(int r, int g, int b)
{
	SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 255);
}

void window::setcolor(unsigned int col)
{
	int b = col & 0xFF;
	col = col >> 8;
	int g = col & 0xFF;
	col = col >> 8;
	int r = col & 0xFF;
	setcolor(r, g, b);
}


void window::point(double x, double y) {
	SDL_RenderDrawPoint(sdl_renderer, x,y);
}

unsigned int floatcolor(double rd, double gd, double bd)
{
	double dcol[3];
	dcol[0] = rd; dcol[1] = gd; dcol[2] = bd;
	int col = 0;
	for (int i = 0; i < 3; i++) {
		double d = dcol[i]*255;
		int c = d;
		if (rand() <= (d - c) * RAND_MAX) c += 1;
		if (c > 255) c = 255;
		if (c < 0) c = 0;
		col = (col << 8) + c;
	}
	return col;
}

unsigned int rainbow(double c)
{
	c *= 4;
	if (c < 0) c = 0;
	if (c < 1) { return floatcolor( 1, sin(c * pi / 2), 0 ); }
	c -= 1;
	if (c < 1) { return floatcolor(cos(c * pi / 2), 1, 0); }
	c -= 1;
	if (c < 1) { return floatcolor(0, 1, sin(c * pi / 2)); }
	c -= 1;
	if (c > 1) c = 1;
	return floatcolor(0, cos(c * pi / 2), 1);
}

unsigned int setgray(double c)
{
	if (c < 0) c = 0;
	if (c > 1) c = 1;
	return floatcolor(c, c, c);
}

