#define _XOPEN_SOURCE 700
#include "screen.h"

#include <stdlib.h>
#include <string.h> /* memset */
#include <curses.h> /* drawing */

struct screen_s {
	unsigned short x, y;
	unsigned short cx, cy;
	bool use_braille;
	unsigned char *dots;
};

unsigned short screen_get_x(screen_t screen) {
	return screen->x;
}

unsigned short screen_get_y(screen_t screen) {
	return screen->y;
}

unsigned short screen_get_cx(screen_t screen) {
	return screen->cx;
}

unsigned short screen_get_cy(screen_t screen) {
	return screen->cy;
}

bool screen_get_braille(screen_t screen) {
	return screen->use_braille;
}

/* **** */

screen_t screen_initialise(bool use_braille) {
	initscr();             /* inicjalizacja ekranu */
	cbreak();              /* linie na wejściu nie są buforowane */
	nonl();                /* ncurses zajmuje się enterami */
	noecho();              /* naciśnięte klawisze nie są wyświetlane */
	curs_set(FALSE);       /* kursor nie jest wyświetlany */

	screen_t screen = calloc(1, sizeof(*screen));

	screen->cx = 1;
	screen->cy = 1;
	screen->dots = calloc(1, sizeof(screen->dots[0]));
	screen->use_braille = use_braille;

	screen_resize(screen);

	return screen;
}

void screen_reset(void) {
	endwin();              /* przywróć terminal */
}

void screen_clear(screen_t screen) {
	memset(screen->dots, 0, screen->cx * screen->cy * sizeof(screen->dots[0]));
}

void screen_resize(screen_t screen) {
	int max_x = 0, max_y = 0;

	getmaxyx(stdscr, max_y, max_x);

	if (screen->cx == max_x && screen->cy == max_y)
		return;

	screen->cx = max_x;
	screen->cy = max_y;
	if (screen->use_braille) {
		screen->x = 2 * max_x;
		screen->y = 4 * max_y;
	} else {
		screen->x = max_x;
		screen->y = max_y;
	}
	screen->dots =
		realloc(
			screen->dots,
			screen->cx * screen->cy * sizeof(screen->dots[0])
		);
}

void screen_draw(screen_t screen) {
	clear();
	for (size_t ix = 0; ix < screen_get_cx(screen); ++ix)
		for (size_t iy = 0; iy < screen_get_cy(screen); ++iy) {
			unsigned char tmp = screen_get_dot(screen, ix, iy);
			if (tmp) {
				move(iy, ix);
				if (screen_get_braille(screen))
					screen_print_dot(tmp);
				else
					addch('*');
			}
		}
	refresh();
}

void screen_show_test(screen_t screen) {
	printw(
		"max: %d %d\n",
		screen_get_cx(screen),
		screen_get_cy(screen)
	);

	printw(
		"dots: %d %d\n",
		screen_get_x(screen),
		screen_get_y(screen)
	);

	screen_print_dot(1);
	screen_print_dot(2);
	screen_print_dot(4);
	screen_print_dot(64);
	screen_print_dot(128);
	screen_print_dot(32);
	screen_print_dot(16);
	screen_print_dot(8);

	addch('\n');

	for (int i = 0; i < 8; ++i)
		addch('0' + i);

	for (int i = 0; i <= 0xFF; ++i) {
		if (i % 8 == 0)
			addch('\n');
		screen_print_dot(i);
	}

	refresh();
}

/* **** */

void screen_add_dot(screen_t screen, unsigned int x, unsigned int y) {
	/*
	 * Braille:
	 * 1 4       1   8
	 * 2 5  =>   2  16
	 * 3 6       4  32
	 * 7 8      64 128
	 */

	if (x >= screen_get_x(screen) || y >= screen_get_y(screen))
		return;

	if (screen_get_braille(screen)) {
		unsigned char *a =
			&(screen->dots[(x / 2) + (y / 4) * screen_get_cx(screen)]);

		x %= 2;
		y %= 4;

		if (y == 3) /* kropki 6 i 7 */
			*a |= (1 << (6 + x));
		else
			*a |= (1 << (x * 3 + y));
	} else {
		screen->dots[x + y * screen_get_cx(screen)] |= '1';
	}
}

unsigned char screen_get_dot(
	screen_t screen,
	unsigned short x,
	unsigned short y
) {
	if (x >= screen_get_cx(screen) || y >= screen_get_cy(screen))
		return 0;
	return screen->dots[x + y * screen_get_cx(screen)];
}

void screen_print_dot(const unsigned long int i) {
	add_wch(&((cchar_t) {A_NORMAL, { 0x2800ul + i, 0 }, 0 }));
}
