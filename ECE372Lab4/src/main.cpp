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

volaitile SwitchState_t switchState = SWITCH_WAIT_PRESS;
volaitile unsigned char switchEvent = 0;



int main() {
  //initialize everything (timer, pwm, ssd, motor, etc)
initTimer0();
initTimer1();
initADC();
initPWM();
initSwitchINT();
init7Seg();

sei();
unsigned int adcVal = readADC();

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
  //put motor code here 
      //adcVal = readADC()
      //PWM based off adcVal
      if(switchEvent) {
        switchEvent = 0;

        //STOP MOTOR (turn off voltage source to it? turn signal to 0?
        //STOPMOTOR();
        disableSwitchINT(); //NEED TO MAKE THIS CODE
        //countdown();
        state = STATE_COUNTDOWN;
      }
    }
    else if (state == STATE_COUNTDOWN) {
      //motor must remain off, cannot be turned on until countdown is done

      if(countdown == 1); //not sure how to do this yet
      countdown = 0;

      enableSwitchINT();
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
