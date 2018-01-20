#ifndef BOIDS_H
#define BOIDS_H

#include "screen.h"
#include <stdlib.h>

typedef struct boids_model_s *boids_model;

boids_model boids_new_model(screen_t screen);

void boids_destroy(boids_model *boids);

void boids_update(screen_t screen, boids_model boids);

#endif /* BOIDS_H */
