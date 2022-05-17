#include "utils.h"

//************************  wait  ************************

void wait(unsigned int n)
{
  while (n--) asm("nop");
}
