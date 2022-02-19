#ifndef PWM_H
#define PWM_H

#include <stdbool.h>

void InitPWM();

void SetPWMValue(unsigned short value);

void EnablePWM(bool ena);

#endif // PWM_H

