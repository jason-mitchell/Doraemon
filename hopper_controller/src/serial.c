//----------------------------------------------------------------------------------
//      serial.c
//    Bit-banged 9600bps serial interface
//    9-Aug-2020
//
//    * Extremely accurate on transmit
//
//----------------------------------------------------------------------------------
#include <msp430.h>
#include "serial.h"

//#define HEARTBEAT_LED BIT0

#define CRC16_CCITT 0x1021



// Functions
//----------------

unsigned int CalcCRC(unsigned char *data, unsigned char len){
    unsigned int crc;
    unsigned char pos = 0;

    crc = 0xFFFF;           // Init vector for CRC16-CCITT

    while(pos < len){
        crc = crc16(crc, data[pos]);
        pos++;
    }
    return crc;

}


unsigned int crc16(unsigned int crcValue, unsigned char newByte){
    unsigned char i;

    for (i = 0; i < 8; i++) {

        if (((crcValue & 0x8000) >> 8) ^ (newByte & 0x80)){
            crcValue = (crcValue << 1)  ^ CRC16_CCITT;
        }else{
            crcValue = (crcValue << 1);
        }

        newByte <<= 1;
    }

    return crcValue;
}

// Name: RxByte
// Function: Get character bit-bang from pin
// Arguments: void
// Returns: received character
//--------------------------------------------------------------
unsigned char RxByte(void){

    unsigned char shift_reg = 0;
    unsigned char bits = 0;
    unsigned char bitpos = 0x01;
    __disable_interrupt();


    // Interrupt latency means we reach here in the middle of the start bit which is great...
    SerialTxTime(4);

    for(bits = 0; bits < 8; bits++){
        SerialTxTime(6);            // Wait for 1/2 the bit time..
        //P1OUT ^= HEARTBEAT_LED;
        if((P1IN & 0x80) == 0x80){
            // 1
            shift_reg |= bitpos;
            //P1OUT |= HEARTBEAT_LED;
        } else {
            // 0
            shift_reg &= ~bitpos;
            //P1OUT &= ~HEARTBEAT_LED;

        };
        bitpos = bitpos << 1;
        SerialTxTime(3);


    }

    // Wait out stop bit
    SerialTxTime(6);  // was 11

    P1IFG = 0x0;
    __enable_interrupt();
    //P1OUT &= ~HEARTBEAT_LED;
    return shift_reg;

}


// Name: SerialTxTime
// Function: Generates a software delay for timing serial bits at 9600
// i.e. 104uS
// Arguments: void
// Returns: void
//--------------------------------------------------------------
void SerialTxTime(unsigned char delay){

    unsigned int n = 0;
    for (n = 0; n < delay; n++){
        __delay_cycles(1);   // This macro is required to prevent the C compiler from optimizing out the loop
   }
}

// Name: TxString
// Function: Send ASCII strings or byte arrays
//           Used as standard output in lieu of stdout as this micro has very little ROM space
// Arguments: Pointer to null-terminated string
// Returns: void
//--------------------------------------------------------------------------------------------
void TxString(char *string){
    unsigned int position = 0;

    for(;;){
        if (string[position] !=0){
            TxByte(string[position]);
            position++;
        } else {
            break;
        }
    }



}

// Name: TxByte
// Function: Transmit byte out on bit-banged serial interface
// Arguments: Byte or character to transmit
// Returns: void
//--------------------------------------------------------------
void TxByte(unsigned char data){

    unsigned char bits = 0;
    // Generate start bit

    __disable_interrupt();
    P1OUT &= ~TxD;                               // Transmit line drops for start bit
    SerialTxTime(12);

    // Shift 8 bits out
    for (bits = 0; bits < 8; bits++){
        if (data & 0x01 == 0x01){
            P1OUT |= TxD;                       // Bit is high
            SerialTxTime(12);
        } else {
            P1OUT &= ~TxD;                      // Bit is low
            SerialTxTime(12);
        }
        data = data >> 1;


    }

    // Generate stop bit
    P1OUT |= TxD;
    __enable_interrupt();
    SerialTxTime(15);

}




