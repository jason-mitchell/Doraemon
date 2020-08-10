/*
 * mechatronic.h
 *
 *  Created on: 10 Aug 2020
 *      Author: jasonmitchell
 */

#ifndef MECHATRONIC_H_
#define MECHATRONIC_H_

// Limit switch defs
//-------------------

#define CLOSED 0x40
#define OPEN 0x80
#define INDETERMINATE 0xC0

#define OPEN_FULLY 0x01
#define CLOSE_FULLY 0x00

#define HOPPER_MOTOR_FWD BIT3
#define HOPPER_MOTOR_REV BIT4
#define VIBRATOR_MOTOR BIT5

#define ERROR_LED BIT1


// Function Prototypes
//--------------------
unsigned char DoServoOperation(unsigned char oper);




#endif /* MECHATRONIC_H_ */
