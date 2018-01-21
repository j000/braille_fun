#define _XOPEN_SOURCE 700
#include "boids.h"

#include <math.h>

#include "random.h"

/* **** */

typedef struct {
	double x;
	double y;
} vector;

typedef struct {
	vector pos;
	vector vel;
} boid;

struct boids_model_s {
	size_t n;
	boid *array;
};

/* **** */

static void initialise_boids(const size_t n, boid a[n], int x, int y) {
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

boids_model boids_new_model(screen_t screen, unsigned int ilosc) {
	boids_model boids = calloc(1, sizeof(boids));

	boids->n = ilosc;
	boids->array = calloc(boids->n, sizeof(*(boids->array)));
	initialise_boids(
		boids->n,
		boids->array,
		screen_get_x(screen) + 1,
		screen_get_y(screen) + 1
	);
	return boids;
}

void boids_destroy(boids_model *boids) {
	free((*boids)->array);
	free((*boids));
	*boids = NULL;
}

void boids_update(screen_t screen, boids_model boids) {
	for (size_t i = 0; i < boids->n; ++i) {
		boid *b = &boids->array[i];
		int tmp_x = floor(b->pos.x);
		int tmp_y = floor(b->pos.y);

		screen_add_dot(screen, tmp_x, tmp_y);

		/* ruch */
		b->pos.x += b->vel.x;
		b->pos.y += b->vel.y;
		/* odbicia */
		if ((b->pos.x >= screen_get_x(screen) && b->vel.x > 0) ||
			(b->pos.x < 0 && b->vel.x < 0)) {
			b->vel.x *= -1.;
			b->pos.x += 2 * b->vel.x;
		}
		if ((b->pos.y >= screen_get_y(screen) && b->vel.y > 0) ||
			(b->pos.y < 0 && b->vel.y < 0)) {
			b->vel.y *= -1.;
			b->pos.y += 2 * b->vel.y;
		}
	}
}
