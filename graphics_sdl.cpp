#include "graphics_sdl.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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
	const size_t max_size = 20;
public:
	B& get(const A& val) {
		typename list_t::iterator lit;
		typename map_t::iterator mit;
		mit = map.find(val);
		if (mit == map.end()) {
			if (list.size() >= max_size) {
				A& idx = list.back().first;
				map.erase(idx);
				list.pop_back();
			}
			list.emplace_front(std::make_pair(val,val));
		} else {
			lit = mit->second;
			if (lit != list.begin()) list.splice(list.begin(), list, lit, std::next(lit));
		}
		lit = list.begin();
		map[val] = lit;
		return lit->second;
	}
	~priority_cache() {	}
};

const double pi = 4*atan(1.0);
const double twopi = 2*pi;

window::sdl_global window::global;

void check_pointer(void* ptr) {
	if (ptr == NULL) {
		fprintf(stderr, "Fatal error: %s\n", SDL_GetError());
		exit(5);
	}
}

window::window() {
    initialised = false;
	to_set = false;
	is_slow = false;
	sdl_window = NULL;
	sdl_renderer = NULL;
	fg.r = 255; fg.g = 255; fg.b = 255; fg.a = SDL_ALPHA_OPAQUE;
	bg.r =   0; bg.g =   0; bg.b =   0; bg.a = SDL_ALPHA_OPAQUE;
	font_name = "assets/OpenSans-Medium.ttf";
	font_size = 20;
	adjx = 0.5;
	adjy = 0.5;
}

window::~window() {
	
};

TTF_Font* cached_Font(const std::string& font, int size) {
	typedef std::tuple< std::string, int > input;
	struct output {
		TTF_Font* font;
		output(const input& in) {
			//printf("Loading font: %s (%d)\n", std::get<0>(in).c_str(), std::get<1>(in));
			font = TTF_OpenFont(std::get<0>(in).c_str(), std::get<1>(in));
			check_pointer(font);
		}
		~output() {
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

sprite::sprite(SDL_Surface* surface_, SDL_Texture* texture_, int nx_, int ny_) :
	surface(surface_), texture(texture_), nx(nx_), ny(ny_) {
	w = surface->w;
	h = surface->h;
}

SDL_Rect getSpriteRect(const sprite& sp, int ix=0, int iy=0) {
	int ix1 = ix % sp.nx;
	int iy1 = (iy + (ix-ix1)/sp.nx) % sp.ny;
	int x1 = sp.w * ix1 / sp.nx;
	int y1 = sp.h * iy1 / sp.ny;
	int x2 = sp.w * (ix1+1) / sp.nx;
	int y2 = sp.h * (iy1+1) / sp.ny;
	SDL_Rect rect;
	rect.x = x1;
	rect.y = y1;
	rect.w = x2 - x1;
	rect.h = y2 - y1;
	return rect;
}

sprite cached_RenderedText(const std::string& font, int size, const SDL_Color& color, SDL_Renderer* const renderer, const std::string& text) {
	typedef std::tuple< std::string, std::string, int, SDL_Color, SDL_Renderer* > input;
	struct output {
		SDL_Surface* textSurface;
		SDL_Texture* textTexture;
		output() : textSurface(NULL), textTexture(NULL) {};
		output(const input& in) {
			// printf("Rendering text: %s\n", std::get<0>(in).c_str());
			TTF_Font* font = cached_Font(std::get<1>(in), std::get<2>(in));
			textSurface = TTF_RenderText_Blended(font, std::get<0>(in).c_str(), std::get<3>(in));
			check_pointer(textSurface);
			textTexture = SDL_CreateTextureFromSurface(std::get<4>(in), textSurface);
			check_pointer(textTexture);
		}
		~output() {
			SDL_DestroyTexture(textTexture);
			SDL_FreeSurface(textSurface);
		}
	};
	static priority_cache<input, output> cache;
	output& out = cache.get(input(text,font,size,color,renderer));
	return sprite(out.textSurface, out.textTexture);
}

sprite cached_Image(const std::string& image, SDL_Renderer* const renderer, int nx, int ny) {
	typedef std::tuple< std::string, SDL_Renderer* > input;
	struct output {
		SDL_Surface* imageSurface;
		SDL_Texture* imageTexture;
		output() : imageSurface(NULL), imageTexture(NULL) {};
		output(const input& in) {
			// printf("Loading image: %s\n", std::get<0>(in).c_str());
			imageSurface = IMG_Load( std::get<0>(in).c_str() );
			check_pointer(imageSurface);
			//printf("Loaded: %s: %dx%d\n", std::get<0>(in).c_str(), (int) imageSurface->w, (int) imageSurface->h );
			imageTexture = SDL_CreateTextureFromSurface(std::get<1>(in), imageSurface);
			check_pointer(imageTexture);
		}
		~output() {
			SDL_DestroyTexture(imageTexture);
			SDL_FreeSurface(imageSurface);
		}
	};
	static priority_cache<input, output> cache;
	output& out = cache.get(input(image,renderer));
	printf("loaded: %p %p %d %d\n",out.imageSurface, out.imageTexture, nx, ny);
	return sprite(out.imageSurface, out.imageTexture, nx, ny);
}


window::sdl_global::sdl_global() {
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER );
	TTF_Init();
	IMG_Init( IMG_INIT_PNG );
	for (int i = 0; i < 256; i++) {
		kbd_tab[i] = false;
	}
}

window::sdl_global::~sdl_global() {
	SDL_Quit();
}


void window::init(int sx, int sy) {
    sdl_window = SDL_CreateWindow("Graphical Window", 10, 10, sx, sy, false);
	check_pointer(sdl_window);
	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
	check_pointer(sdl_renderer);
	initialised = true;
	global.windows.push_back(this);
	global.active = this;
	_setcolor(fg);	
	sdl_display = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, sx, sy);
	check_pointer(sdl_display);
	SDL_SetRenderTarget(sdl_renderer, sdl_display);
	_clear();
	_update();
}

void window::_clear() {
	_setcolor(bg);
	SDL_RenderClear(sdl_renderer);
	_setcolor(fg);
}

void window::_update() {
	SDL_SetRenderTarget(sdl_renderer, NULL);
	_clear();
	SDL_RenderCopy(sdl_renderer, sdl_display, NULL, NULL);
	SDL_RenderPresent( sdl_renderer );
	SDL_SetRenderTarget(sdl_renderer, sdl_display);
}

int window::sdl_global::animate(double fps) {
	for( window* w : global.windows) {
		w->_update();
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
	_clear();
}

sprite window::load_image(const std::string& image, int nx, int ny) {
	sprite sp = cached_Image(image, sdl_renderer, nx, ny);
	return sp;
}

void window::stamp(double x, double y, const sprite& sp, int ix, int iy) {
	SDL_Rect src_rect = getSpriteRect(sp,ix,iy);
	SDL_Rect dst_rect = src_rect;
	dst_rect.x = x - src_rect.w*adjx;
	dst_rect.y = y - src_rect.h*adjy;
	SDL_RenderCopy(sdl_renderer, sp.texture, &src_rect, &dst_rect);
}

void window::text(double x, double y, const std::string& text) {
	SDL_Color color = {fg.r, fg.g, fg.b, fg.a};
	sprite sp = cached_RenderedText(font_name, font_size, color, sdl_renderer, text);
	return stamp(x,y,sp);
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
		double m = 60.0/d;
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
	double mi = 0.1;
	double t2 = 0;
	_resetto();
	if (is_slow) {
		double m = 60.0/d;
		for (double t = 0; t < 1; t += m) {
			double t1 = twopi * flow(t);
			while (t2 < t1) {
				_lineto(x0+cos(t2)*r,y0+sin(t2)*r);
				t2 += mi;
			}
			_lineto(x0+cos(t1)*r,y0+sin(t1)*r);
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

void window::_setcolor(const color_t& col)
{
	SDL_SetRenderDrawColor(sdl_renderer, col.r, col.g, col.b, col.a);
}


void window::setcolor(int r, int g, int b, int a)
{
	fg.r = r;
	fg.g = g;
	fg.b = b;
	fg.a = a;
	_setcolor(fg);
}

void window::setcolor(const color_t& col)
{
	fg = col;
	_setcolor(fg);
}


void window::point(double x, double y) {
	SDL_RenderDrawPoint(sdl_renderer, x,y);
}

unsigned char _floatcolor(double d) {
	d = d*255;
	int c = d;
	if (rand() <= (d - c) * RAND_MAX) c += 1;
	if (c > 255) c = 255;
	if (c < 0) c = 0;
	return c;
}

color_t floatcolor(double r, double g, double b, double a=1)
{
	color_t col;
	col.r = _floatcolor(r);
	col.g = _floatcolor(g);
	col.b = _floatcolor(b);
	col.a = _floatcolor(a);
	return col;
}

color_t rainbow(double c)
{
	if (c < 0.00) { return floatcolor( 1, 0, 0 ); }
	if (c < 0.25) { return floatcolor( 1, sin(twopi * c), 0 ); }
	if (c < 0.50) { return floatcolor(sin(twopi * c), 1, 0); }
	if (c < 0.75) { return floatcolor(0, 1, -sin(twopi * c)); }
	if (c < 1.00) { return floatcolor(0, -sin(twopi * c), 1); }
	return floatcolor(0, 0, 1);
}

color_t grayscale(double c)
{
	if (c < 0) { return floatcolor(0, 0, 0); }
	if (c < 1) { return floatcolor(c, c, c); }
	return floatcolor(1, 1, 1);
}

