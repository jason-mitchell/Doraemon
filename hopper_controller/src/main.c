
// Included headers
//-----------------

#include <msp430.h> 
#include "serial.h"
#include "timer.h"
#include "mechatronic.h"




// Definitions
//------------
#define HEARTBEAT_LED BIT0
//#define ERROR_LED BIT1
//#define HOPPER_MOTOR_FWD BIT3
//#define HOPPER_MOTOR_REV BIT4
//#define VIBRATOR_MOTOR BIT5


// Variables
//-------------
unsigned char rxbyte;
unsigned char rxbuf[10];
unsigned char rxbufptr = 0;
unsigned int calc_crc;

// Main Function
//---------------
int main(void){

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// Configure and calibrate system clock
	//--------------------------------------
	DCOCTL = 0;                                  // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                       // Set range
	DCOCTL = CALDCO_1MHZ;                        // Set DCO step + modulation */
	BCSCTL2 = 0x00;



	// Configure ports
	//------------------
	P1DIR = 0x7F;                               // P1.0 thru P1.6 OUTPUTS
	P1OUT = 0x00;
	P2SEL &= 0x3F;                             // P2.6 and P2.7 -> GPIO mode
	P2DIR = 0x00;
	P2REN = 0xC0;
	P1REN = 0x80;
	P1OUT |= TxD;                               // Transmit line always high

    P1IES = 0x80;                               // This register MUST be set up
    P1IE |= RxD;


    // Configure Timer 1 to provide PWM
    //----------------------------------
    P1SEL |= BIT2;                              // P1.2 to TA0.1
    TACTL = TACLR;                              // Clear Timer
    CCR0 = 120;                                  // PWM Period
    CCTL1 = OUTMOD_7;                           // CCR1 reset/set
    CCR1 = 0;                                  // CCR1 PWM duty cycle = 0
    TACTL = TASSEL_2 + MC_1 + ID_3;             // SMCLK, up mode, timer running, divide input clock by 8

	// Configure WDT as systick timer
	//--------------------------------

	WDTCTL = WDT_MDLY_0_5 ;                     // Enable WDT with period of 500uS
	IE1 |= WDTIE;                               // Enable WDT interrupt
	__enable_interrupt();                       // Enable System interrupts
	InitTimerSystem();

	// Set up start-up conditions
	P1OUT |= ERROR_LED;                         // ERROR LED is on
	P1OUT |= HEARTBEAT_LED;
	Delay(1500);
	TxString("\r\nHopper Mechatronic controller v1.0\r\nBy Sonikku\r\nAugust 2020");
	P1OUT &= ~ERROR_LED;
	P1OUT &= ~HEARTBEAT_LED;


	//P1OUT |=HOPPER_MOTOR_REV;
	Delay(500);
	// Quick-test limit switch

	// Do not escape main function...
	for(;;){


	 Delay(500);
	 TxByte(P2IN);
	 P1OUT ^= HEARTBEAT_LED;
	 if (rxbufptr > 5){
	     rxbufptr = 0;
	     TxByte(rxbuf[0]);
	     TxByte(rxbuf[1]);
	     TxByte(rxbuf[2]);
	     calc_crc = CalcCRC(rxbuf, 3);
	     TxByte(*(((char *)&calc_crc)+1));
	     TxByte(*(((char *)&calc_crc)+0));
	     if (calc_crc == 0xD135){
	         P1OUT &= ~ERROR_LED;
	     }
	 }


	DoServoOperation(OPEN_FULLY);
	//P1OUT |= VIBRATOR_MOTOR;
	Delay(1500);
	P1OUT &= ~VIBRATOR_MOTOR;
	DoServoOperation(CLOSE_FULLY);
	Delay(1500);
	}

}

// Port 1 Interrupt
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    //P1IFG = 0;                    // Clear interrupt
    rxbuf[rxbufptr] = RxByte();
    rxbufptr++;
    if (rxbufptr > 10){
        rxbufptr = 0;               // Wrap around to zero creating circular buffer
    }

}

// Watchdog Timer interrupt service routine
// Used here as a system timer
// Period is 65.6 uS
//---------------------------------------------------
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void){

    CallInISR();


}
