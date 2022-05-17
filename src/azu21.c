#include "charger.h"

int main()
{
	init_charger();

	while (1)
	  {
			on_idle();
	  }
	return 0;
}
