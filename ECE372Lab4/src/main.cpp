#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "switch.h"
#include "timer.h"

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



int main() {
  //initialize everything (timer, pwm, ssd, motor, etc)
initTimer0();
initTimer1();
initADC();
initPWMTimer3();
initSwitchINT();
init7Seg();

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
      changeDutyCycle(adcVal); //changes the duty cycle from ADC for the PWM for motor
      
      if(switchEvent) {
        switchEvent = 0;

        //STOP MOTOR (turn off voltage source to it? turn signal to 0
        changeDutyCycle(0);
   //     disableSwitchINT(); //NEED TO MAKE THIS CODE -> should be fixed
        
         //DONE BY NOAH 4/5/26
        EIMSK &= ~(1 << INT0);   //BY NOAH: THIS LINE WILL DIABLE INTERUPT FLAGS FROM BEING MADE
        
        //countdown(); need to make a code for the countdown?
        i = 9
        countdowndone = 0;
        displayDigit(i);
        state = STATE_COUNTDOWN;
      }
    }
      
    else if (state == STATE_COUNTDOWN) {
      changeDutyCycle(0);
      
      //BUTTON IS PRESSED, motor must remain off, cannot be turned on until countdown is done
      //FIXMEFIXMEFIXME
      //OCR0A = 62500 for 1 second delay timer 1, 16Mhz/256
      //
      while(countdowndone == 0){
        displayDigit(i);
        i--;
        if(i == 0) {
          countdowndone = 1;
        }
      }
      

      
      if(countdowndone){ //not sure how to do this yet
  
        clearDisplay(); //clears the display

     //   enableSwitchINT();   
        //DONE BY NOAH 4/5/26
           EIFR |= (1 << INTF0);   // clear any pending interrupt flag (VERY important)
            EIMSK |= (1 << INT0);   // ENABLES INTERUPT FLAG 


        
        state = STATE_RUNNING;
      }
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
