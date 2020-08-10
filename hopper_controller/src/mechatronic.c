//
//
//
//
//
//
//----------------------------------------------------------------------------------------------

#include <msp430.h>
#include "mechatronic.h"
#include "timer.h"


// Name: DoServoOperation
// Function: Move the valve to the desired position under servo control
// Arguments: 1 = Open full, 0 = Close full
// Returns: Result of operation
//------------------------------------------------------------------------
unsigned char DoServoOperation(unsigned char oper){

    if (oper == OPEN_FULLY){
       // If already open, do nothing
        if (P2IN == OPEN){
            return 0;
        }
        P1OUT |=HOPPER_MOTOR_FWD;       // Go from open --> closed position
        CCR1 = 55;                      // 55% PWM (1/2 motor torque)
        TIME_MEASURE = 0;               // Clear free-running timer counter
        while(P2IN != 0x80){
            if (TIME_MEASURE > 4000){
                P1OUT |= ERROR_LED;
                break;
            }
        }
        // Limit reached, power motor off
        CCR1 = 0;
        P1OUT &= ~HOPPER_MOTOR_FWD;

    }

    if (oper == CLOSE_FULLY){
       // If already closed, do nothing
        if (P2IN == CLOSED){
            return 0;
        }
        P1OUT |=HOPPER_MOTOR_REV;       // Go from open --> closed position
        CCR1 = 55;                      // 55% PWM (1/2 motor torque)
        TIME_MEASURE = 0;               // Clear free-running timer counter
        while(P2IN != 0x40){
            if (TIME_MEASURE > 4000){
                P1OUT |= ERROR_LED;
                break;
            }
        }
        // Limit reached, power motor off
        CCR1 = 0;
        P1OUT &= ~HOPPER_MOTOR_REV;


    }


    return 255;
}




