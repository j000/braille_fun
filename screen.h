#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

typedef struct screen_s *screen_t;

unsigned short screen_get_x(screen_t screen);

unsigned short screen_get_y(screen_t screen);

unsigned short screen_get_cx(screen_t screen);

unsigned short screen_get_cy(screen_t screen);

bool screen_get_braille(screen_t screen);

/* **** */

screen_t screen_initialise(bool use_braille);

void screen_reset(void);

void screen_clear(screen_t screen);

void screen_resize(screen_t screen);

void screen_draw(screen_t screen);

void screen_show_test(screen_t screen);

/* **** */

void screen_add_dot(screen_t screen, unsigned int x, unsigned int y);

unsigned char screen_get_dot(
	screen_t screen,
	unsigned short x,
	unsigned short y
);

void screen_print_dot(const unsigned long int i);

#endif /* SCREEN_H */
