#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <locale.h>        /* setlocale */
#include <curses.h>        /* drawing */

#include "screen.h"
#include "boids.h"

int main(int argc, char **argv) {
	setlocale(LC_ALL, ""); /* włącz UTF-8 */

	screen_t screen = screen_initialise(true);

	boids_model boids = boids_new_model(screen);

	screen_show_test(screen);
	getch();               /* poczekaj na klawisz */

	timeout(30);           /* getch nie blokuje */

	bool keep_looping = true;

	while (keep_looping) {
		switch (getch()) {
		case KEY_RESIZE:
			screen_resize(screen);
			break;
		case ERR:
			break;
		default:
			keep_looping = false;
		}

		screen_clear(screen);
		boids_update(screen, boids);
		screen_draw(screen);
	}

	screen_reset();

	return EXIT_SUCCESS;
}
