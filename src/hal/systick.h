#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void InitSysTick();

void msleep(uint32_t ms);
void sleep(uint32_t s);

uint32_t GetTicks();

#endif // SYSTICK_H
