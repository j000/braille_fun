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

typedef int (*comp)(const void *, const void *);

static void swap(boid *a, boid *b) {
	boid t;

	t = *a;
	*a = *b;
	*b = t;
}

static int compare_x(const void *_a, const void *_b) {
	const boid *a = (const boid *)_a;
	const boid *b = (const boid *)_b;

	if (a->pos.x > b->pos.x)
		return 1;
	if (a->pos.x < b->pos.x)
		return -1;
	if (a->pos.y > b->pos.y)
		return 1;
	if (a->pos.y < b->pos.y)
		return -1;
	return 0;
}

static void median_of_3(boid *a, boid *b, boid *c, comp compare) {
	if (compare(a, b) > 0)
		swap(a, b);
	if (compare(a, c) < 0)
		swap(a, c);
	if (compare(b, c) > 0)
		swap(b, c);
}

static void insertion_sort(
	boid a[],
	const size_t lo,
	const size_t hi,
	comp compare
) {
	size_t i = lo + 1;

	while (i <= hi) {
		boid x = a[i];
		size_t j = i - 1;

		while (j >= lo && compare(&a[j], &x) > 0) {
			a[j + 1] = a[j];
			j -= 1;
		}
		a[j + 1] = x;
		i += 1;
	}
}

static size_t partition(boid a[], size_t lo, size_t hi, comp compare) {
	size_t center = lo + (hi - lo) / 2;

	median_of_3(&a[lo], &a[center], &a[hi], compare);

	boid pivot = a[center];
	size_t i = lo - 1;
	size_t j = hi + 1;

	while (1) {
		do
			i += 1;
		while (compare(&a[i], &pivot) < 0); /* a[i] < pivot */
		do
			j -= 1;
		while (compare(&a[j], &pivot) > 0); /* a[j] > pivot */

		if (i >= j)
			return j;

		swap(&a[i], &a[j]);
	}
}

static void quicksort(boid a[], size_t lo, size_t hi, comp compare) {
	if (lo >= hi)
		return;
	if (lo + 16 >= hi) {
		insertion_sort(a, lo, hi, compare);
		return;
	}

	size_t p = partition(a, lo, hi, compare);

	quicksort(a, lo, p, compare);
	quicksort(a, p + 1, hi, compare);
}

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
	quicksort(boids->array, 0, boids->n - 1, compare_x);

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
