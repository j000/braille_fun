#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h> /* assert */
#include <time.h>   /* nanosleep */
#include <locale.h> /* setlocale */
#include <curses.h> /* drawing */

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

/**
 * Zwróć wartość [0,1]
 * TODO: może [0,1)?
 */
double get_random(void) {
	/* TODO: znajdź lepsze rozwiązanie */
	return ((double)rand()) / RAND_MAX;
}

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

struct screen {
	unsigned short x;
	unsigned short y;
	unsigned char dots[];
};

void add_dot(struct screen *screen, unsigned int x, unsigned int y) {
	/*
	 * Braille:
	 * 1 4       1   8
	 * 2 5  =>   2  16
	 * 3 6       4  32
	 * 7 8      64 128
	 */

	if (x >= screen->x * 2 || y >= screen->y * 4)
		return;

	unsigned char *a = &(screen->dots[screen->y * (x / 2) + (y / 4)]);

	x %= 2;
	y %= 4;

	if (y == 3)
		*a |= (1 << (6 + x));
	else
		*a |= (1 << (x * 3 + y));
}

unsigned char get_dot(
	struct screen *screen,
	unsigned short x,
	unsigned short y
) {
	if (x >= screen->x || y >= screen->y)
		return 0;
	return screen->dots[screen->y * x + y];
}

void clear_screen(struct screen *screen) {
	for (size_t i = 0; i < screen->x * screen->y; ++i)
		screen->dots[i] = 0;
}

void initialise_screen(void) {
	setlocale(LC_ALL, ""); /* włącz UTF-8 */

	initscr();             /* inicjalizacja ekranu */
	cbreak();              /* linie na wejściu nie są buforowane */
	nonl();                /* ncurses zajmuje się enterami */
	noecho();              /* naciśnięte klawisze nie są wyświetlane */
	curs_set(FALSE);       /* kursor nie jest wyświetlany */
}

void reset_screen(void) {
	endwin();              /* przywróć terminal */
}

/* **** */

int main(int argc, char **argv) {
	int max_y = 0, max_x = 0;

	const size_t n = 400;
	boid *a = calloc(n, sizeof(*a));

	initialise_screen();

	getmaxyx(stdscr, max_y, max_x);

	struct screen *screen =
		malloc(sizeof(*screen) + max_x * max_y * sizeof(screen->dots[0]));

	screen->x = max_x;
	screen->y = max_y;
	clear_screen(screen);

	wprintw(stdscr, "max: %d %d\n", max_x, max_y);

	max_x *= 2;
	max_y *= 4;
	initialise_boids(n, a, max_x, max_y);
	wprintw(stdscr, "dot: %d %d\n", max_x - 1, max_y - 1);

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
	getch();               /* poczekaj na klawisz */

	unsigned char simple[] = {
		1,
		2,
		4,
		64,
		128,
		32,
		16,
		8
	};
	unsigned char circle[][2] = {
		{ 8, 0 },
		{ 2, 0 },
		{ 4, 0 },
		{ 128, 0 },
		{ 0, 64 },
		{ 0, 32 },
		{ 0, 16 },
		{ 0, 1 }
	};

	char c = 0;

	wtimeout(stdscr, 30); /* getch nie blokuje */
	while (ERR == (c = wgetch(stdscr))) {
		clear_screen(screen);
		for (size_t i = 0; i < n; ++i) {
			int tmp_x = floor(a[i].pos.x);
			int tmp_y = floor(a[i].pos.y);

			add_dot(screen, tmp_x, tmp_y);
			if (a[i].pos.x >= max_x || a[i].pos.x < 0)
				a[i].vel.x *= -1;
			if (a[i].pos.y >= max_y || a[i].pos.y < 0)
				a[i].vel.y *= -1;
			a[i].pos.x += a[i].vel.x;
			a[i].pos.y += a[i].vel.y;
		}
		clear();
		for (size_t ix = 0; ix < screen->x; ++ix)
			for (size_t iy = 0; iy < screen->y; ++iy) {
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
