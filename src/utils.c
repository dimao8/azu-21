#include "utils.h"

//************************  Wait  ************************

void Wait(unsigned int n)
{
  while (n--) asm("nop");
}
