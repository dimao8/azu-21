#include "systick.h"

#include <stm32f0xx.h>

uint32_t elapsed;

/***********************  init_systick  ***********************/

void init_systick()
{
  uint32_t reload = FREQUENCY/1000;

  elapsed = 0;

  SysTick->LOAD = reload & 0xFFFFFF;
  SysTick->VAL = reload & 0xFFFFFF;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
    | SysTick_CTRL_TICKINT_Msk
    | SysTick_CTRL_ENABLE_Msk;

  NVIC_EnableIRQ(SysTick_IRQn);
}

/**************************  msleep  *************************/

void msleep(uint32_t ms)
{
  uint32_t start = elapsed;

  while (elapsed - start < ms) { asm("nop"); }
}

/**************************  sleep  **************************/

void sleep(uint32_t s)
{
  uint32_t start = elapsed;

  while (elapsed - start < s*1000) { asm("nop"); }
}

/*************************  get_ticks  ************************/

uint32_t get_ticks()
{
  return elapsed;
}

/*********************  SysTick_Handler  *********************/

void SysTick_Handler()
{
  elapsed++;
}
