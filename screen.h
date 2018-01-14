#ifndef SCREEN_H
#define SCREEN_H

typedef struct screen_s *screen_t;

unsigned short get_x(screen_t screen);

unsigned short get_y(screen_t screen);

/* **** */

screen_t initialise_screen(void);

void reset_screen(void);

void clear_screen(screen_t screen);

/* **** */

void add_dot(screen_t screen, unsigned int x, unsigned int y);

unsigned char get_dot(screen_t screen, unsigned short x, unsigned short y);

#endif /* SCREEN_H */
