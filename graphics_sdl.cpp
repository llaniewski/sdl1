#include "graphics_sdl.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <string>
#include <tuple>
#include <map>


template <class A, class B>
class priority_cache {
	typedef std::pair<A,B> pair_t;
	typedef std::list<pair_t> list_t;
	typedef std::map<A, typename list_t::iterator> map_t;
	list_t list;
	map_t map;
public:
	B& get(const A& val) {
		typename list_t::iterator lit;
		typename map_t::iterator mit;
		mit = map.find(val);
		if (mit == map.end()) {
			printf("not found, emplacing\n");
			list.emplace_front(std::make_pair(val,val));
		} else {
			printf("found\n");
			lit = mit->second;
			if (lit != list.begin()) {
				printf("splicing\n");
				list.splice(list.begin(), list, lit, lit++);
			}
		}
		lit = list.begin();
		map[val] = lit;
		return lit->second;
	}
	~priority_cache() {
		printf("deleting priority_cache\n");
	}
};

const double pi = 4*atan(1.0);
const double twopi = 2*pi;

window::sdl_global window::global;

window::window() {
    initialised = false;
	to_set = false;
	is_slow = false;
	sdl_window = NULL;
	sdl_renderer = NULL;
}

window::~window() {
    if (initialised) {
        SDL_Quit();
    }
};

TTF_Font* cached_Font(const std::string& font, int size) {
	typedef std::tuple< std::string, int > input;
	struct output {
		TTF_Font* font;
		output(const input& in) {
			printf("Loading font: %s (%d)\n", std::get<0>(in).c_str(), std::get<1>(in));
			font = TTF_OpenFont(std::get<0>(in).c_str(), std::get<1>(in));
			if (font == NULL) {
				fprintf(stderr, "Failed to load font: %s\n", SDL_GetError());
			}
		}
		~output() {
			printf("destroy font\n");
			TTF_CloseFont(font);
		}
	};
	static priority_cache<input, output> cache;
	output& out = cache.get(input(font,size));
	return out.font;
}

bool operator<(const SDL_Color& A, const SDL_Color& B) {
	if (A.r < B.r) return true;
	if (A.r > B.r) return false;
	if (A.g < B.g) return true;
	if (A.g > B.g) return false;
	if (A.b < B.b) return true;
	if (A.b > B.b) return false;
	if (A.a < B.a) return true;
	return false;
}

class sprite {
public:
	SDL_Surface* surface;
	SDL_Texture* texture;
	int nx, ny;
	int w, h;
	sprite(SDL_Surface* surface_, SDL_Texture* texture_, int nx_=1, int ny_=1) :
		surface(surface_), texture(texture_), nx(nx_), ny(ny_) {
		w = surface->w;
		h = surface->h;
	}
	inline SDL_Rect getRect(int ix=0, int iy=0) {
		int ix1 = ix % nx;
		int iy1 = (iy + (ix-ix1)/nx) % ny;
		int x1 = w * ix1 / nx;
		int y1 = h * iy1 / ny;
		int x2 = w * (ix1+1) / nx;
		int y2 = h * (iy1+1) / ny;
		SDL_Rect rect;
		rect.x = x1;
		rect.y = y1;
		rect.w = x2 - x1;
		rect.h = y2 - y1;
		return rect;
	}
};

sprite cached_RenderedText(const std::string& font, int size, const SDL_Color& color, SDL_Renderer* const renderer, const std::string& text) {
	typedef std::tuple< std::string, int, SDL_Color, SDL_Renderer*, std::string > input;
	struct output {
		SDL_Surface* textSurface;
		SDL_Texture* textTexture;
		output(const input& in) {
			printf("Getting Font for rendering text: %s\n", std::get<4>(in).c_str());
			TTF_Font* font = cached_Font(std::get<0>(in), std::get<1>(in));
			printf("Rendering text: %s\n", std::get<4>(in).c_str());
			textSurface = TTF_RenderText_Blended(font, std::get<4>(in).c_str(), std::get<2>(in));
			textTexture = SDL_CreateTextureFromSurface(std::get<3>(in), textSurface);
		}
		~output() {
			printf("destroy rendered text\n");
			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(textTexture);
		}
	};
	static priority_cache<input, output> cache;
	output& out = cache.get(input(font,size,color,renderer,text));
	return sprite(out.textSurface, out.textTexture);
}


window::sdl_global::sdl_global() {
	TTF_Init();
	for (int i = 0; i < 256; i++) {
		kbd_tab[i] = false;
	}
}

window::sdl_global::~sdl_global() {

}


void window::init(int sx, int sy) {
    sdl_window = SDL_CreateWindow("Graphical Window", 10, 10, sx, sy, false);
	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
	SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
	initialised = true;
	global.windows.push_back(this);
	global.active = this;
}

int window::sdl_global::animate(double fps) {
	for( window* w : global.windows) {
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
	SDL_SetRenderDrawColor(sdl_renderer, 128, 0, 0, 255);
	SDL_RenderClear(sdl_renderer);
	SDL_SetRenderDrawColor(sdl_renderer, r, g, b, a);
}

void window::_text(double x, double y, const std::string& font, int size, const std::string& text, double adjx, double adjy) {
	SDL_Color color = {255,255,255,255};
	sprite ren = cached_RenderedText(font, size, color, sdl_renderer, text);
	SDL_Rect src_rect = ren.getRect();
	SDL_Rect dst_rect = src_rect;
	dst_rect.x = x - src_rect.w*adjx;
	dst_rect.y = y - src_rect.h*adjy;
	SDL_RenderCopy(sdl_renderer, ren.texture, &src_rect, &dst_rect);
}

void window::text(double x, double y, int size, const std::string& text) {
	return _text(x,y,"OpenSans-Medium.ttf", size, text);
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


void window::sdl_global::wait() {
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

