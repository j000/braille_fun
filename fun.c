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

void dot(unsigned long int i) {
	i += 0x2800ul;
	wadd_wch(
		stdscr,
		&(cchar_t) {A_NORMAL, { i, 0 }, 0 }
	);
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

	screen_t screen = initialise_screen();

	wprintw(stdscr, "max: %d %d\n", get_x(screen), get_y(screen));

	initialise_boids(n, a, get_x(screen) * 2, get_y(screen) * 4);
	wprintw(stdscr, "dot: %d %d\n", get_x(screen) * 2, get_y(screen) * 4);

	dot(1);
	dot(2);
	dot(4);
	dot(64);
	dot(128);
	dot(32);
	dot(16);
	dot(8);

	addch('\n');

	for (int i = 0; i < 8; ++i)
		addch('0' + i);

	for (int i = 0; i <= 0xFF; ++i) {
		if (i % 8 == 0)
			addch('\n');
		dot(i);
	}

	refresh();
	getch();              /* poczekaj na klawisz */

	char c = 0;

	wtimeout(stdscr, 30); /* getch nie blokuje */
	while (ERR == (c = wgetch(stdscr))) {
		clear_screen(screen);
		for (size_t i = 0; i < n; ++i) {
			int tmp_x = floor(a[i].pos.x);
			int tmp_y = floor(a[i].pos.y);

			add_dot(screen, tmp_x, tmp_y);
			if (a[i].pos.x >= get_x(screen) * 2 || a[i].pos.x < 0)
				a[i].vel.x *= -1;
			if (a[i].pos.y >= get_y(screen) * 4 || a[i].pos.y < 0)
				a[i].vel.y *= -1;
			a[i].pos.x += a[i].vel.x;
			a[i].pos.y += a[i].vel.y;
		}
		clear();
		for (size_t ix = 0; ix < get_x(screen); ++ix)
			for (size_t iy = 0; iy < get_y(screen); ++iy) {
				unsigned char tmp = get_dot(screen, ix, iy);
				if (tmp) {
					wmove(stdscr, iy, ix);
					dot(tmp);
				}
			}
		refresh();
	}

	reset_screen();

	return EXIT_SUCCESS;
}
