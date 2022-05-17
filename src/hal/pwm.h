#ifndef PWM_H
#define PWM_H

#include <stdbool.h>

void init_pwm();

void set_pwm_value(unsigned short value);

void enable_pwm(bool ena);

#endif // PWM_H

