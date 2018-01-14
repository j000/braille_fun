#include "screen.h"

#include <stdlib.h>
#include <curses.h> /* drawing */

struct screen_s {
	unsigned short x;
	unsigned short y;
	unsigned char *dots;
};

unsigned short screen_get_x(screen_t screen) {
	return screen->x;
}

unsigned short screen_get_y(screen_t screen) {
	return screen->y;
}

/* **** */

screen_t screen_initialise(void) {
	int max_y = 0, max_x = 0;

	initscr();             /* inicjalizacja ekranu */
	cbreak();              /* linie na wejściu nie są buforowane */
	nonl();                /* ncurses zajmuje się enterami */
	noecho();              /* naciśnięte klawisze nie są wyświetlane */
	curs_set(FALSE);       /* kursor nie jest wyświetlany */

	getmaxyx(stdscr, max_y, max_x);

	screen_t screen = calloc(1, sizeof(*screen));

	screen->x = max_x;
	screen->y = max_y;
	screen->dots = calloc(max_x * max_y, sizeof(screen->dots[0]));

	return screen;
}

void screen_reset(void) {
	endwin();              /* przywróć terminal */
}

void screen_clear(screen_t screen) {
	for (size_t i = 0; i < screen_get_x(screen) * screen_get_y(screen); ++i)
		screen->dots[i] = 0;
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

	if (x >= screen_get_x(screen) * 2 || y >= screen_get_y(screen) * 4)
		return;

	unsigned char *a = &(screen->dots[screen_get_y(screen) * (x / 2) + (y / 4)]);

	x %= 2;
	y %= 4;

	if (y == 3)
		*a |= (1 << (6 + x));
	else
		*a |= (1 << (x * 3 + y));
}

unsigned char screen_get_dot(screen_t screen, unsigned short x, unsigned short y) {
	if (x >= screen_get_x(screen) || y >= screen_get_y(screen))
		return 0;
	return screen->dots[screen_get_y(screen) * x + y];
}
