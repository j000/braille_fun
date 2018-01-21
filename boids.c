#define _XOPEN_SOURCE 700
#include "boids.h"

#include <math.h>

#include "random.h"

/* **** */

/* defaults */
static const double range = 5.;
static const double max_speed = 1.5;
static const double max_acceleration = 0.2;
static const double separation = 3.;
static const double edge_distance = 2.;

static const double wander_angle_change = M_PI / 180. * 30;
static const double wander_distance = 0.3;

static const double wander_weight = 0.1;
static const double alignment_weight = 0.4;
static const double cohesion_weight = 0.3;
static const double separation_weight = 2.0;

/* **** */

typedef struct {
	double x;
	double y;
} vector;

typedef struct {
	vector pos;
	vector vel;
	vector acc;
	double wander_angle;
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

		while (j >= lo && j <= hi && compare(&a[j], &x) > 0) {
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

static void quicksort(
	boid a[],
	size_t lo,
	size_t hi,
	signed char limit,
	comp compare
) {
	if (lo >= hi)
		return;
	if (lo + 16 >= hi || limit <= 0) {
		insertion_sort(a, lo, hi, compare);
		return;
	}

	size_t p = partition(a, lo, hi, compare);

	quicksort(a, lo, p, limit - 1, compare);
	quicksort(a, p + 1, hi, limit - 1, compare);
}

static void sort(boid a[], size_t size, comp compare) {
	quicksort(a, 0, size - 1, 16, compare);
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
		a[i].acc = (vector){0 };
	}
}

boids_model boids_new_model(screen_t screen, unsigned int ilosc) {
	boids_model boids = calloc(1, sizeof(*boids));

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
	sort(boids->array, boids->n, compare_x);

	/* sprawdzamy tylko przedział (x - range; x + range)
	 * i przesuwamy go dla każdego boida */
	size_t lo = 0, hi = 0;

	for (size_t i = 0; i < boids->n; ++i) {
		boid *b = &boids->array[i];

		/* popraw przedział */
		while (lo < boids->n - 1 &&
			boids->array[lo].pos.x + range < b->pos.x)
			++lo;
		while (hi < boids->n &&
			boids->array[hi].pos.x - range <= b->pos.x)
			++hi;

		/* Wander */
		vector wander_v = { 0 };
		{
			wander_v.x += wander_distance * cos(b->wander_angle);
			wander_v.y += wander_distance * sin(b->wander_angle);
			b->wander_angle += (get_random() - 0.5) * wander_angle_change;
		}

		b->acc.x += wander_v.x * wander_weight;
		b->acc.y += wander_v.y * wander_weight;

		/* Alignment */
		vector alignment_v = { 0 };
		unsigned int alignment_c = 0;

		/* Cohesion */
		vector cohesion_v = { 0 };
		unsigned int cohesion_c = 0;

		/* Separation */
		vector separation_v = { 0 };

		for (size_t j = lo; j < hi; ++j) {
			if (i == j)
				continue;

			vector out = boids->array[j].pos;

			out.x -= b->pos.x;
			out.y -= b->pos.y;

			double dist_sqared = out.x * out.x + out.y * out.y;

			if (dist_sqared >= (range * range))
				continue;

			if (dist_sqared <= separation) {
				/* double tmp = (sqrt(dist_sqared) / boids->separation) - 1.; */
				double tmp = -1. / dist_sqared;

				separation_v.x += out.x * tmp;
				separation_v.y += out.y * tmp;
			} else {
				alignment_v.x += boids->array[j].vel.x;
				alignment_v.y += boids->array[j].vel.y;
				++alignment_c;

				cohesion_v.x += boids->array[j].pos.x;
				cohesion_v.y += boids->array[j].pos.y;
				++cohesion_c;
			}
		}

		if (alignment_c > 0) {
			alignment_v.x *= 1. / alignment_c;
			alignment_v.y *= 1. / alignment_c;
			alignment_v.x -= b->vel.x;
			alignment_v.y -= b->vel.y;

			b->acc.x += alignment_v.x * alignment_weight;
			b->acc.y += alignment_v.y * alignment_weight;
		}

		if (cohesion_c > 0) {
			cohesion_v.x *= 1. / cohesion_c;
			cohesion_v.y *= 1. / cohesion_c;
			cohesion_v.x -= b->pos.x;
			cohesion_v.y -= b->pos.y;

			b->acc.x += cohesion_v.x * cohesion_weight;
			b->acc.y += cohesion_v.y * cohesion_weight;
		}

		b->acc.x += separation_v.x * separation_weight;
		b->acc.y += separation_v.y * separation_weight;

		/* borders */
		if (b->pos.x + edge_distance >= screen_get_x(screen))
			b->acc.x -= (max_speed - b->vel.x);
		else if (b->pos.x - edge_distance <= 0)
			b->acc.x += max_speed - b->vel.x;
		if (b->pos.y + edge_distance >= screen_get_y(screen))
			b->acc.y -= max_speed - b->vel.y;
		else if (b->pos.y - edge_distance <= 0)
			b->acc.y += max_speed - b->vel.y;

		/* acceleration limit */
		{
			double l = b->acc.x * b->acc.x + b->acc.y * b->acc.y;

			if (l > 0 && l > max_acceleration * max_acceleration) {
				b->acc.x *= max_acceleration / sqrt(l);
				b->acc.y *= max_acceleration / sqrt(l);
			}
		}
	}

	for (size_t i = 0; i < boids->n; ++i) {
		boid *b = &boids->array[i];

		/* update */
		b->vel.x += b->acc.x;
		b->vel.y += b->acc.y;

		b->pos.x += b->vel.x;
		b->pos.y += b->vel.y;

		b->acc.x = 0;
		b->acc.y = 0;

		/* speed limit */
		{
			double l = b->vel.x * b->vel.x + b->vel.y * b->vel.y;

			if (l > 0 && l > max_speed * max_speed) {
				b->vel.x *= max_speed / sqrt(l);
				b->vel.y *= max_speed / sqrt(l);
			}
		}

		/* draw */
		int tmp_x = floor(b->pos.x);
		int tmp_y = floor(b->pos.y);

		screen_add_dot(screen, tmp_x, tmp_y);
	}
}
