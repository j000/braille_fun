#include "random.h"

#include <stdlib.h> /* rand, srand, RAND_MAX */

/**
 * Zwróć wartość [0,1)
 */
static double generate_random(void) {
	/* TODO: znajdź lepsze rozwiązanie */
	return ((double)rand()) / (1. + RAND_MAX);
}

static double init_random(void) {
	/* domyślna wartość */
	unsigned int seed = 0xFEEDu;

	/* zainicjuj generator */
	srand(seed);

	/* zmień wskaźnik */
	get_random = generate_random;
	/* wygeneruj liczbę */
	return generate_random();
}

double (*get_random)(void) = init_random;
