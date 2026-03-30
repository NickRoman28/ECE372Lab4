#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

void initPWMTimer3();
void changeDutyCycle(uint16_t adcValue);

#endif