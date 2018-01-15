#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h> /* assert */
#include <time.h>   /* nanosleep */
#include <locale.h> /* setlocale */
#include <curses.h> /* drawing */

#include "screen.h"
#include "boids.h"

void sleep_ms(int milliseconds) {
	struct timespec ts = {
		.tv_sec = milliseconds / 1000,
		.tv_nsec = (milliseconds % 1000) * 1000000
	};

	nanosleep(&ts, NULL);
}

/* **** */

int main(int argc, char **argv) {
	setlocale(LC_ALL, ""); /* włącz UTF-8 */

	screen_t screen = screen_initialise();

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
