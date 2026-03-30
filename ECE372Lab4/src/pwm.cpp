#include "pwm.h"

void initPWMTimer3() {
    DDRE |= (1 << PE3); // initialize OC3A as output

    // set Fast PWM 10-bit mode, non-inverting
    TCCR3A |= (1 << COM3A1)|(1 << WGM31)|(1 << WGM30);
    TCCR3B |= (1 << WGM32) | (1 << CS31) | (1 << CS30);//prescaler set to 64
    TCCR3A &= ~(1 << COM3A0);

    // Initialize duty cycle to 0
    OCR3A = 0;
}

void changeDutyCycle(uint16_t adcValue) {
    OCR3A = adcValue;
}