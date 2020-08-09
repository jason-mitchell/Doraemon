/*
 * serial.h
 *
 *  Created on: 09 Aug 2020
 *      Author: jasonmitchell
 */

#ifndef SERIAL_H_
#define SERIAL_H_


#define TxD BIT6
#define RxD BIT7


// Function Prototypes
//---------------------
unsigned int CalcCRC(unsigned char *data, unsigned char len);
unsigned int crc16(unsigned int crcValue, unsigned char newByte);
void SerialTxTime(unsigned char delay);
void TxString(char *string);
void TxByte(unsigned char data);
unsigned char RxByte(void);
unsigned char samplebit(void);


#endif /* SERIAL_H_ */
