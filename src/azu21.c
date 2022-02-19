#include "charger.h"

int main()
{
	InitCharger();

	while (1)
	  {
			OnIdle();
	  }
	return 0;
}
