// g++ main.cpp `pkg-config --cflags --libs sdl2`
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

SDL_Window* window;
SDL_Renderer* renderer;

const double pi = 4*atan(1.0);
const double twopi = 2*pi;

class char_tab {
protected:
	bool buf[256];
	bool& el(char ch) {
		return buf[(unsigned char)ch];
	}
public:
	void down(char ch) {
		el(ch) = true;
	}
	void up(char ch) {
		el(ch) = false;
	}
	int is_pressed(char ch) {
		return el(ch);
	}
	char_tab() {
		for (int i = 0; i < 256; i++) {
			buf[i] = false;
		}
	}
};
static char_tab kbd_tab;


void graphics(int sx, int sy) {
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
    while( SDL_PollEvent(&event) )
    {
        if (event.type == SDL_QUIT) {
            SDL_Quit();
            exit(-1);
        }
        if (event.type == SDL_KEYDOWN) {
            int key = event.key.keysym.sym;
            kbd_tab.down(key);
        }
        if (event.type == SDL_KEYUP) {
            int key = event.key.keysym.sym;
            kbd_tab.up(key);
        }
    }
    return 1;
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

void setrgbrand(double rd, double gd, double bd)
{
	int r=rd, g=gd, b=bd;
	if (rand() <= (rd-r)*RAND_MAX) r += 1;
	if (rand() <= (gd-g)*RAND_MAX) g += 1;
	if (rand() <= (bd-b)*RAND_MAX) b += 1;
	setrgbcolor(r,g,b);
}


void setcolor(double c)
{
	c *= 4;
	if (c < 0) c = 0;
	if (c < 1) { setrgbrand(255,sin(c*pi/2)*255,0); return; }
	c -= 1;
	if (c < 1) { setrgbrand(cos(c*pi/2)*255,255,0); return; }
	c -= 1;
	if (c < 1) { setrgbrand(0,255,sin(c*pi/2)*255); return; }
	c -= 1;
	if (c > 1) c=1;
	{ setrgbrand(0,cos(c*pi/2)*255,255); return; }
}


int is_pressed(char ch) {
	return kbd_tab.is_pressed(ch);
}


int pressed_space() { return is_pressed(32); }

void point(double x, double y) {
    SDL_RenderDrawPoint(renderer, x,y);
}

int main() {
	graphics(800, 600);
	const int nx = 800;
	const int ny = 600;
	int castle[nx][ny];
	for (int i = 0; i < nx; i++) {
		for (int j = 0; j < ny; j++) {
			if (j > 500) {
                castle[i][j] = 2;
            } else if (i > 400 && i < 440 && j - 300 > 0) {
				castle[i][j] = 3;
            } else if (i > 460 && i < 600 && j-300 + (i - 530) > 0 && j - 300 - (i - 530) > 0) {
				castle[i][j] = 6;
			} else {
				castle[i][j] = 0;
			}
		}
	}
	int col = 0;
	double aa_max = -0.1;
	double aa_min = -0.8;
	double av_min = 10;
	double av_max = 30;
	double ax = 50, ay = 500, aa = aa_min, av = av_min;
	int phase = 0;
	double t = 0;
	double bx = 0;
	double by = 0;
	double bvx = 0;
	double bvy = 0;
	const int fps = 15;
    const double anim_dt = 1.0/fps;
    double anim_t = 0.0;
    const double move_dt = 0.01;
	double move_t = 0.0;
    const double sand_dt = 0.02;
	double sand_t = 0.0;
	while (1) {
		animate(fps);
        anim_t += anim_dt;
		t = t + 3.0 / fps;
		if (phase == 0) {
			bx = -10;
			by = -10;
			aa = (1 + cos(t)) / 2 * (aa_max - aa_min) + aa_min;
		}
		else if (phase == 1) {
			av = (1 - cos(t)) / 2 * (av_max - av_min) + av_min;
		}
		else if (phase == 2) {

		}
		double adx = cos(aa);
		double ady = sin(aa);

		if (pressed_space()) {
			if (phase == 0) {
				phase = 1;
				t = 0;
			}
		}
		else {
			if (phase == 1) {
				phase = 2;
				bx = ax;
				by = ay;
				bvx = adx * av*10;
				bvy = ady * av*10;
			}
		}

        for (; move_t < anim_t; move_t += move_dt) {
            if (phase==2) {
                bx += bvx * move_dt;
                by += bvy * move_dt;
                bvy += 50 * move_dt;
                if (bx >= 0 && by >= 0 && bx < nx && by < ny) {
                    int i = bx;
                    int j = by;
                    if (castle[i][j]) {
                        int r = 25;
                        int r2 = 20;
                        for (int ii = -r; ii < r; ii++) if (i + ii >= 0 && i + ii < nx) {
                            for (int ji = -r; ji < r; ji++) if (j + ji >= 0 && j + ji < ny) {
								double d2 = ii * ii + ji * ji;
                                if (d2 < r * r) {
                                    // if (ii * ii + ji * ji < r2 * r2) {
                                    //     castle[i + ii][j + ji] = 0;
                                    // } else {
                                    //     if (castle[i + ii][j + ji] == 1) castle[i + ii][j + ji] = 0;
                                    //     if (castle[i + ii][j + ji] > 1) {
									// 		if (rand() < RAND_MAX / 2) 
									// 			castle[i + ii][j + ji] = 1;
									// 		else
									// 			castle[i + ii][j + ji] = 0;
									// 	}
                                    // }
									if (castle[i + ii][j + ji] == 1) castle[i + ii][j + ji] = 0;
									if (castle[i + ii][j + ji] > 1) {
										if (rand() < 0.5*d2/(r*r) * RAND_MAX) 
									 			castle[i + ii][j + ji] = 1;
									 		else
									 			castle[i + ii][j + ji] = 0;
									}
                                }
                            }
                        }
                        phase = 0;
                    }
                }
                if (bx < 0 || bx > nx + 100 || by < 0 || by > ny) {
                    phase = 0;
                }
            }
        }

		for (; sand_t < anim_t; sand_t += sand_dt) {
			for (int i = 0; i < nx; i++) {
				for (int j = ny - 2; j >= 0; j--) {
					if (castle[i][j] == 1) {
						if (castle[i][j + 1] == 0) {
							castle[i][j] = 0;
							castle[i][j + 1] = 1;
						}
						else {
							if (rand() < RAND_MAX / 2) {
								if (i - 1 >= 0 && castle[i - 1][j + 1] == 0) {
									castle[i][j] = 0;
									castle[i - 1][j + 1] = 1;
								}
							}
							else {
								if (i + 1 < nx && castle[i + 1][j + 1] == 0) {
									castle[i][j] = 0;
									castle[i + 1][j + 1] = 1;
								}
							}
						}
					}
					else if (castle[i][j] > 1) {
						if (castle[i][j + 1] == 0) {
							int r = rand();
							if (i - 1 >= 0 && castle[i - 1][j] <= 1) r /= 3;
							if (i + 1 < nx && castle[i + 1][j] <= 1) r /= 3;
							if (r < (RAND_MAX / 20)) {
								castle[i][j] = 0;
								if (r < RAND_MAX / 50) castle[i][j + 1] = 1;
							}
						}
					}
				}
			}
		}

		clear();
		circle(bx, by, 5);
		line(0, ny, nx + 100, ny);
		for (int i = 0; i < nx; i++) {
			for (int j = 0; j < ny; j++) {
                
				if (castle[i][j]) {
                    setcolor(castle[i][j]*0.2);
                    point(i, j);
                }
			}
		}
		const double g1 = 5, g2 = 30;
		line(ax - ady * g1, ay + adx * g1, ax - ady * g1 + adx * g2, ay + adx * g1 + ady * g2);
		line(ax + ady * g1, ay - adx * g1, ax + ady * g1 + adx * g2, ay - adx * g1 + ady * g2);
		line(ax + ady * g1, ay - adx * g1, ax - ady * g1, ay + adx * g1);
		if (phase < 2) {
			line(ax, ay, ax + adx * av * 4, ay + ady * av * 4);
		}
	}
	return 0;
}










