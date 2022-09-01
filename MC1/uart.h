 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Hisham Elsayed
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"


/*******************************************************************************
 *                               Definitions                                   *
 *******************************************************************************/
#define UART_BAUD_RATE     9600


/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	DISABLED,RESERVED,EVEN,ODD
}UART_parityMode;

typedef enum
{
	BIT_5,BIT_6,BIT_7,BIT_8
}UART_characterSize;

typedef enum
{
	ONE_BIT,TWO_BIT
}UART_stopBit;

typedef struct
{
	UART_parityMode parity;
	UART_characterSize size;
	UART_stopBit stop;
}UART_configType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(uint32 baud_rate,UART_configType *Config_ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
