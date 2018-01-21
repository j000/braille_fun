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
	/* parametry */
	bool use_braille = true;
	unsigned int fps = 25;
	unsigned int n = 1600;

	/* **** */
	if (argc > 1) {
		char *end = NULL;
		unsigned int argument = strtoul(argv[1], &end, 0);

		if (*end != '\0') {
			printf("To nie jest liczba: %s\n", argv[1]);
			return EXIT_FAILURE;
		}
		if (argument != 0 && argument < 10001)
			n = argument;
	}

	/* **** */
	const int delay = floor(1000. / fps);

	setlocale(LC_ALL, ""); /* włącz UTF-8 */

	screen_t screen = screen_initialise(use_braille);

	boids_model boids = boids_new_model(screen, n);

#if 0
	screen_show_test(screen);
	getch();               /* poczekaj na klawisz */
#endif

	timeout(delay);        /* getch nie blokuje */

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

	boids_destroy(&boids);
	screen_reset();

	return EXIT_SUCCESS;
}
