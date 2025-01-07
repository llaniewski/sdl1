#include "graphics_sdl.h"
#include <SDL2/SDL.h>

const double pi = 4*atan(1.0);
const double twopi = 2*pi;

struct sdl_wrapper_t {
	SDL_Window* window;
	SDL_Renderer* renderer;

	bool kbd_tab[256];
	bool& is_pressed(char ch) {
		return kbd_tab[(unsigned char)ch];
	}

    bool initialised;

    sdl_wrapper_t() {
        initialised = false;
		window = NULL;
		renderer = NULL;
		for (int i = 0; i < 256; i++) {
			kbd_tab[i] = false;
		}
    }

    ~sdl_wrapper_t() {
        if (initialised) {
            SDL_Quit();
        }
    };

	void graphics(int sx, int sy) {
        initialised = true;
		window = SDL_CreateWindow("Graphical Window", 10, 10, sx, sy, false);
		renderer = SDL_CreateRenderer(window, -1, 0);
	}

	int animate(double fps) {
		SDL_RenderPresent( renderer );
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

	void clear() {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
	}

	void line(double x1, double y1, double x2, double y2) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	}

	void circle(double x0, double y0, int r) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		double da = 3.0/r;
		double x = x0 + r;
		double y = y0;
		for(double a=0; a<twopi; a+=da) {
			double nx = x0+cos(a)*r;
			double ny = y0+sin(a)*r;
			SDL_RenderDrawLine(renderer, x,y,nx,ny);
			x = nx;
			y = ny;
		}
	}

	void setrgbcolor(int r, int g, int b)
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	}

	void point(double x, double y) {
		SDL_RenderDrawPoint(renderer, x,y);
	}

};

static sdl_wrapper_t sdl_wrapper;

void setrgbcolor(double rd, double gd, double bd)
{
	int r=rd, g=gd, b=bd;
	if (rand() <= (rd-r)*RAND_MAX) r += 1;
	if (rand() <= (gd-g)*RAND_MAX) g += 1;
	if (rand() <= (bd-b)*RAND_MAX) b += 1;
	sdl_wrapper.setrgbcolor(r,g,b);
}


void setcolor(double c)
{
	c *= 4;
	if (c < 0) c = 0;
	if (c < 1) { setrgbcolor(255,sin(c*pi/2)*255,0); return; }
	c -= 1;
	if (c < 1) { setrgbcolor(cos(c*pi/2)*255,255,0); return; }
	c -= 1;
	if (c < 1) { setrgbcolor(0,255,sin(c*pi/2)*255); return; }
	c -= 1;
	if (c > 1) c=1;
	{ setrgbcolor(0,cos(c*pi/2)*255,255); return; }
}

void setgray(double c)
{
	if (c < 0) c = 0;
	if (c > 1) c = 1;
	setrgbcolor(c*255,c*255,c*255);
}

int is_pressed(char ch) {
	return sdl_wrapper.is_pressed(ch);
}

void graphics(int sx, int sy) { return sdl_wrapper.graphics(sx,sy); }
int animate(double fps) { return sdl_wrapper.animate(fps); }
void clear() { return sdl_wrapper.clear(); }
void line(double x1, double y1, double x2, double y2) { return sdl_wrapper.line(x1,y1,x2,y2); }
void circle(double x, double y, int r) { return sdl_wrapper.circle(x,y,r); }
void point(double x, double y) { return sdl_wrapper.point(x,y); }
