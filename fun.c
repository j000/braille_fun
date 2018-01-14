#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h> /* assert */
#include <time.h>   /* nanosleep */
#include <locale.h> /* setlocale */
#include <curses.h> /* drawing */

#include "screen.h"
#include "random.h"

void sleep_ms(int milliseconds) {
	struct timespec ts = {
		.tv_sec = milliseconds / 1000,
		.tv_nsec = (milliseconds % 1000) * 1000000
	};

	nanosleep(&ts, NULL);
}

typedef struct {
	double x;
	double y;
} vector;

typedef struct {
	vector pos;
	vector vel;
} boid;

void initialise_boids(const size_t n, boid a[n], int x, int y) {
	for (size_t i = 0; i < n; ++i) {
		a[i].pos = (vector){
			.x = get_random() * x,
			.y = get_random() * y
		};
		a[i].vel = (vector){
			.x = -1. + get_random() * 2,
			.y = -1. + get_random() * 2
		};
	}
}

/* **** */

int main(int argc, char **argv) {
	const size_t n = 400;
	boid *a = calloc(n, sizeof(*a));

	setlocale(LC_ALL, ""); /* włącz UTF-8 */

	screen_t screen = screen_initialise();

	initialise_boids(n, a, screen_get_x(screen), screen_get_y(screen));

	screen_show_test(screen);

	getch();              /* poczekaj na klawisz */

	char c = 0;

	timeout(30); /* getch nie blokuje */
	while (ERR == (c = getch())) {
		screen_clear(screen);
		for (size_t i = 0; i < n; ++i) {
			int tmp_x = floor(a[i].pos.x);
			int tmp_y = floor(a[i].pos.y);

			screen_add_dot(screen, tmp_x, tmp_y);
			if (a[i].pos.x >= screen_get_x(screen) || a[i].pos.x < 0)
				a[i].vel.x *= -1;
			if (a[i].pos.y >= screen_get_y(screen) || a[i].pos.y < 0)
				a[i].vel.y *= -1;
			a[i].pos.x += a[i].vel.x;
			a[i].pos.y += a[i].vel.y;
		}
		clear();
		for (size_t ix = 0; ix < screen_get_cx(screen); ++ix)
			for (size_t iy = 0; iy < screen_get_cy(screen); ++iy) {
				unsigned char tmp = screen_get_dot(screen, ix, iy);
				if (tmp) {
					move(iy, ix);
					screen_print_dot(tmp);
				}
			}
		refresh();
	}

	screen_reset();

	return EXIT_SUCCESS;
}
