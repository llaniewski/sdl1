#include "graphics_sdl.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
const double pi = 4*atan(1.0);
const double twopi = 2*pi;

window DefaultWindow;
std::vector<window*> active;

bool window::kbd_tab[256];

window::window() {
    initialised = false;
	to_set = false;
	is_slow = false;
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
    sdl_window = SDL_CreateWindow("Graphical Window", 10, 10, sx, sy, false);
	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
	initialised = true;
	active.push_back(this);
}

int window::animate(double fps) {
	for( window* w : active) {
		SDL_RenderPresent( w->sdl_renderer );
	}
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
	Uint8 r,g,b,a;
	SDL_GetRenderDrawColor(sdl_renderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
	SDL_SetRenderDrawColor(sdl_renderer, r, g, b, a);
}

double flow(double x) {
	if (x < 0) x = 0;
	if (x > 1) x = 1;
	return x*x*(3-2*x);
	//return x;
}


void window::_resetto() {
	to_set = false;
}

void window::_line(double x1, double y1, double x2, double y2) {
	SDL_RenderDrawLine(sdl_renderer, x1, y1, x2, y2);
}


void window::_lineto(double x, double y) {
	if (to_set) {
		_line(to_x, to_y, x, y);
	}
	to_set = true;
	to_x = x;
	to_y = y;
}


void window::line(double x1, double y1, double x2, double y2) {
	if (is_slow) {
		double d = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
		double m = 80.0/d;
		double t = 0;
		_resetto();
		for (double t = 0; t < 1; t += m) {
			double t1 = flow(t);
			_lineto(x1*(1-t1)+x2*t1, y1*(1-t1)+y2*t1);
			animate(15);
		}
		_lineto(x2, y2);
	} else {
		_line(x1,y1,x2,y2);
	}
}


void window::circle(double x0, double y0, int r) {
	double d = twopi*r;
	double mi = 80.0/d;
	double t2 = 0;
	_resetto();
	if (is_slow) {
		double m = 80.0/d;
		for (double t = 0; t < 1; t += m) {
			double t1 = twopi * flow(t);
			while (t2 <= t1) {
				_lineto(x0+cos(t2)*r,y0+sin(t2)*r);
				t2 += mi;
			}
			animate(15);
		}
	}
	while (t2 < twopi) {
		_lineto(x0+cos(t2)*r,y0+sin(t2)*r);
		t2 += mi;
	}
	_lineto(x0+r,y0);
}

void window::slow() {
	is_slow = true;
}


void window::wait() {
	while (animate(15)) {};
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

