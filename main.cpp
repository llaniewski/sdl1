// g++ main.cpp `pkg-config --cflags --libs sdl2`
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "graphics_sdl.h"

int main() {
	const int nx = 800;
	const int ny = 600;
	graphics(nx,ny);
	char castle[nx][ny];
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

		if (is_pressed(SPACEBAR)) {
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
                    setcolor(rainbow(castle[i][j]*0.2));
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










