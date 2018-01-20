#define _XOPEN_SOURCE 700
#include "random.h"

#include <stdlib.h> /* rand, srand, RAND_MAX */
#include <stdio.h>
#if defined (__has_include) && __has_include("sys/random.h")
#   include <sys/random.h>
#else /* if defined (__has_include) && __has_include("sys/random.h") */
#   define NO_GETRANDOM
#endif /* if defined (__has_include) && __has_include("sys/random.h") */

/**
 * Zwróć wartość [0,1)
 */
static double generate_random(void) {
	/* TODO: znajdź lepsze rozwiązanie niż rand() */
	return ((double)rand()) / (1. + RAND_MAX);
}

/**
 * fallback:
 * 1) getrandom (jeśli dostępny podczas kompilacji)
 * 2) odczyt z /dev/urandom
 * 3) domyślna wartość
 */
static double init_random(void) {
	unsigned int seed = 0;

#ifndef NO_GETRANDOM
	if (getrandom(&seed, sizeof(seed), 0) == -1)
#endif /* ifndef NO_GETRANDOM */
	{
		/* otwórz /dev/urandom */
		FILE *urandom = fopen("/dev/urandom", "r");

		/* i przeczytaj kilka bajtów */
		if (urandom != NULL) {
			if (fread(&seed, sizeof(seed), 1, urandom) != 1)
				/* domyślna wartość */
				seed = 0xFEEDu;
			fclose(urandom);
		}
	}

	/* zainicjuj generator */
	srand(seed);

	/* zmień wskaźnik */
	get_random = generate_random;
	/* wygeneruj liczbę */
	return generate_random();
}

double (*get_random)(void) = init_random;
