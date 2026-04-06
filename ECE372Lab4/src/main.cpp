#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "switch.h"
#include "timer.h"
#include "adc.h" 
#include "pwm.h" 
#include "sevensegment.h"

typedef enum {
  STATE_RUNNING,
  STATE_COUNTDOWN
}  State_t;

//motor on or off
typedef enum {
  SWITCH_WAIT_PRESS, 
   SWITCH_DEBOUNCE_PRESS,
   SWITCH_WAIT_RELEASE,
   SWITCH_DEBOUNCE_RELEASE
} SwitchState_t;
// controls time delay for button being pressed for timer0.
State_t state = STATE_RUNNING;

volatile SwitchState_t switchState = SWITCH_WAIT_PRESS;
volatile unsigned char switchEvent = 0;
volatile unsigned char second = 0; //keeps track of timer 1 for global interrupt.



int main() {
  //initialize everything timers, adc, pwm, switch, 7seg
initTimer0();
initTimer1();
initADC();
initPWM3();
initSwitchPD0();
init7Seg();

DDRH |= (1 << DDH3);   // digital pin 6 output
DDRH |= (1 << DDH4);   // digital pin 7 output

sei();
unsigned int adcVal = readADC();
unsigned char i = 9;
int countdowndone = 0;

//GET PINS RIGHT//
  while(1) {
if(switchState == SWITCH_DEBOUNCE_PRESS) {
      if (!(PIND & (1 << PD0))) { // Check if the switch is still pressed
        switchState = SWITCH_WAIT_RELEASE; // Move to the next state
      } else {
        switchState = SWITCH_WAIT_PRESS; // If the switch is not pressed, go back to waiting for press
      }
    }

    else if (switchState == SWITCH_DEBOUNCE_RELEASE) {
      if (PIND & (1 << PD0)) { // Check if the switch is still released
        switchEvent=1;
        switchState = SWITCH_WAIT_PRESS;
      }
      else {
        switchState = SWITCH_WAIT_RELEASE;
      }
    }


    
    
    if (state == STATE_RUNNING) {
      //motor is on until button is pressed.

      adcVal = readADC(); //gets signal for PWM
      // dead zone around 2.5V
if (adcVal >= 500 && adcVal <= 524) {
    // stop motor
    PORTH &= ~(1 << PH3);   // pin 6 LOW
    PORTH &= ~(1 << PH4);   // pin 7 LOW
    changeDutyCycle(0);
}
else if (adcVal < 500) {
    // clockwise
    PORTH |= (1 << PH3);    // pin 6 HIGH
    PORTH &= ~(1 << PH4);   // pin 7 LOW

    // lower voltage = faster
    changeDutyCycle((500 - adcVal) * 2);
}
else {
    // counterclockwise
    PORTH &= ~(1 << PH3);   // pin 6 LOW
    PORTH |= (1 << PH4);    // pin 7 HIGH

    // higher voltage = faster
    changeDutyCycle((adcVal - 524) * 2);
}
      
      if(switchEvent) {
        switchEvent = 0;
        //STOP MOTOR (turn off voltage source to it? turn signal to 0
        changeDutyCycle(0);
        
         //DONE BY NOAH 4/5/26
        EIMSK &= ~(1 << INT0);   //BY NOAH: THIS LINE WILL DIABLE INTERUPT FLAGS FROM BEING MADE
        
        // countdown logic by NICK
        // will count down from 9 to 0 with 1 second delay in between, then turn on motor again and reset state to running
        i = 9;
        countdowndone = 0;
        second =0;
        displayDigit(i);
        startTimer1();  //NEW CHANG EBY NOAH FOR TIMER
        state = STATE_COUNTDOWN;
      }
    }
      
    else if (state == STATE_COUNTDOWN) {
      changeDutyCycle(0);
      
      //BUTTON IS PRESSED, motor must remain off, cannot be turned on until countdown is done
      //OCR0A = 62500 for 1 second delay timer 1, 16Mhz/256
     if (second) {
       second = 0;

       // keep counting down until 0 is hit and then turn the motor back on
       if (i>0) {
         i--;
         displayDigit(i);
       }
       else {
         countdowndone = 1;
       }
     }
      

      
      if(countdowndone){ 
        
      stopTimer1(); //NEW
        
        clearDisplay(); //clears the display

     //   enableSwitchINT();   
        //DONE BY NOAH 4/5/26
           EIFR |= (1 << INTF0);   // clear any pending interrupt flag (VERY important)
            EIMSK |= (1 << INT0);   // ENABLES INTERUPT FLAG 


        
        state = STATE_RUNNING;
      }
  }
  }
  return 0;
}
  





ISR(INT0_vect) {
    switch(switchState) {
      
      case(SWITCH_WAIT_PRESS):
        switchState = SWITCH_DEBOUNCE_PRESS;
        break;

      case(SWITCH_WAIT_RELEASE):
        switchState = SWITCH_DEBOUNCE_RELEASE;
        break;
          
    }
}

