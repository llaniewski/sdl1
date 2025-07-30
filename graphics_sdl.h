//
// Simple wrapper for SDL library for teaching C/C++
//
// Author: Lukasz Laniewski-Wollk
//         MEiL, Politechnika Warszawska
//         Warsaw University of Technology
//

class SDL_Window;
class SDL_Renderer;

class window {
private:
	SDL_Window* sdl_window;
	SDL_Renderer* sdl_renderer;
	
	static bool kbd_tab[256];
	bool is_slow;
	bool to_set;
	double to_x, to_y;
	bool initialised;
	void _resetto();
	void _lineto(double x, double y);
	void _line(double x1, double y1, double x2, double y2);
public:
	static inline bool& is_pressed(char ch) {
		return kbd_tab[(unsigned char)ch];
	}
	window();
	~window();
	void graphics(int sx, int sy);
	void clear();
	void line(double x1, double y1, double x2, double y2);
	void circle(double x0, double y0, int r);
	void setcolor(int r, int g, int b);
	void setcolor(unsigned int c);
	void point(double x, double y);
	void slow();
	static int animate(double fps);
	static void wait();
};



unsigned int rainbow(double c);
unsigned int setgray(double c);

const unsigned int GREEN = 0x00FF00;
const unsigned int WHITE = 0xFFFFFF;
const char SPACEBAR = 32;

extern window DefaultWindow;

inline int is_pressed(char ch) { return DefaultWindow.is_pressed(ch); }
inline void graphics(int sx, int sy) { return DefaultWindow.graphics(sx, sy); }
inline void clear() { return DefaultWindow.clear(); }
inline void line(double x1, double y1, double x2, double y2) { return DefaultWindow.line(x1, y1, x2, y2); }
inline void circle(double x, double y, int r) { return DefaultWindow.circle(x, y, r); }
inline void point(double x, double y) { return DefaultWindow.point(x, y); }
inline void setcolor(int r, int g, int b) {return DefaultWindow.setcolor(r, g, b); }
inline void setcolor(unsigned int c) { return DefaultWindow.setcolor(c); }
inline void slow() { return DefaultWindow.slow(); }
inline int animate(double fps) { return DefaultWindow.animate(fps); }
inline void wait() { return DefaultWindow.wait(); }
