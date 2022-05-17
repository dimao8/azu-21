#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void init_systick();

void msleep(uint32_t ms);
void sleep(uint32_t s);

uint32_t get_ticks();

#endif // SYSTICK_H
