#ifndef SCREEN_H
#define SCREEN_H

typedef struct screen {
	unsigned short x;
	unsigned short y;
	unsigned char dots[];
} *screen_t;

#endif /* SCREEN_H */
