#include "random.h"

#include <stdlib.h> /* rand, srand, RAND_MAX */

/**
 * Zwróć wartość [0,1]
 * TODO: może [0,1)?
 */
double get_random(void) {
	/* TODO: znajdź lepsze rozwiązanie */
	return ((double)rand()) / RAND_MAX;
}
