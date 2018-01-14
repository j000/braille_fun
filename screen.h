#ifndef SCREEN_H
#define SCREEN_H

typedef struct screen_s *screen_t;

unsigned short screen_get_x(screen_t screen);

unsigned short screen_get_y(screen_t screen);

/* **** */

screen_t screen_initialise(void);

void screen_reset(void);

void screen_clear(screen_t screen);

/* **** */

void screen_add_dot(screen_t screen, unsigned int x, unsigned int y);

unsigned char screen_get_dot(screen_t screen, unsigned short x, unsigned short y);

#endif /* SCREEN_H */
