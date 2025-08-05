//
// Simple wrapper for SDL library for teaching C/C++
//
// Author: Lukasz Laniewski-Wollk
//         MEiL, Politechnika Warszawska
//         Warsaw University of Technology
//

#include <vector>
#include <string>

class SDL_Window;
class SDL_Renderer;
class SDL_Surface;
class SDL_Texture;

struct color_t {
	unsigned char r,g,b,a;
	inline color_t(): r(0),g(0),b(0),a(255) {};
	inline color_t(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_=255): r(r_),g(g_),b(b_),a(a_) {};
};

class sprite {
public:
	SDL_Surface* surface;
	SDL_Texture* texture;
	int nx, ny;
	int w, h;
	sprite(SDL_Surface* surface_, SDL_Texture* texture_, int nx_=1, int ny_=1);
};

class window {
private:
	struct sdl_global {
		bool kbd_tab[256];
		window* active;
		std::vector<window*> windows;
	public:
		sdl_global();
		~sdl_global();
		inline bool& is_pressed(char ch) {
			return kbd_tab[(unsigned char)ch];
		}
		int animate(double fps);
		void wait();
	};

	SDL_Window* sdl_window;
	SDL_Renderer* sdl_renderer;
	SDL_Texture* sdl_display;
	
	bool is_slow;
	bool to_set;
	double to_x, to_y;
	bool initialised;
	std::string font_name;
	int font_size;
	double adjx, adjy;
	color_t fg, bg;
	void _resetto();
	void _lineto(double x, double y);
	void _line(double x1, double y1, double x2, double y2);
	void _setcolor(const color_t& c);
	void _update();
	void _clear();
public:
	window();
	~window();
	static sdl_global global;
	void init(int sx, int sy);
	void clear();
	void line(double x1, double y1, double x2, double y2);
	void circle(double x0, double y0, int r);
	void setcolor(int r, int g, int b, int a=255);
	void setcolor(const color_t& c);
	void point(double x, double y);
	void slow();
	void stamp(double x, double y, const sprite& sp, int ix=0, int iy=0);
	void text(double x, double y, const std::string& text);
	inline void set_font(const std::string& font_name_) { font_name = font_name_; };
	inline void set_font_size(int font_size_) { font_size = font_size_; };
	void set_adj(double adjx_, double adjy_) { adjx = adjx_; adjy = adjy_; };
	sprite load_image(const std::string& image, int nx=1, int ny=1);
};

color_t rainbow(double c);
color_t grayscale(double c);

const color_t GREEN = {0,255,0};
const color_t WHITE = {255,255,255};
const color_t BLACK = {0,0,0};
const char SPACEBAR = 32;

inline window& active_window() { return *window::global.active; }
inline int is_pressed(char ch) { return window::global.is_pressed(ch); }
inline void graphics(int sx, int sy) {
	window* win = new window;
	win->init(sx,sy);
}
inline void clear() { return active_window().clear(); }
inline void line(double x1, double y1, double x2, double y2) { return active_window().line(x1, y1, x2, y2); }
inline void circle(double x, double y, int r) { return active_window().circle(x, y, r); }
inline void point(double x, double y) { return active_window().point(x, y); }
inline void setcolor(int r, int g, int b, int a=255) {return active_window().setcolor(r, g, b, a); }
inline void setcolor(color_t c) { return active_window().setcolor(c); }
inline void slow() { return active_window().slow(); }
inline int animate(double fps) { return window::global.animate(fps); }
inline void wait() { return window::global.wait(); }
inline void text(double x, double y, const std::string& text) { return active_window().text(x, y, text); };
inline sprite load_image(const std::string& image, int nx=1, int ny=1) { return active_window().load_image(image, nx, ny); };
inline void stamp(double x, double y, const sprite& sp, int ix=0, int iy=0) { return active_window().stamp(x, y, sp, ix, iy); }
