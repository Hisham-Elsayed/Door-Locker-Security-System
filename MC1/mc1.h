 /******************************************************************************
 *
 * Module: MC1
 *
 * File Name: mc1.h
 *
 * Description: Header file for the Application
 *
 * Author: Hisham Elsayed
 *
 *******************************************************************************/

#ifndef MC1_H_
#define MC1_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASS_SIZE		                      5
#define KEYPAD_INPUT_DELAY		            500
#define DOOR_UNLOCKING_PERIOD	             15
#define DOOR_LEFT_OPEN_PERIOD	              3
#define DISPLAY_MESSAGE_DELAY	           3000
/* following definitions used to communicate with Control ECU */
#define PASSWORD_MATCHED		            1
#define PASSWORD_MISMATCHED		            0
#define READY_TO_SEND			           0x15
#define READY_TO_RECEIVE		           0x16
#define CHANGE_PASSWORD_OPTION	           0x18
#define UNLOCKING_DOOR			           0x25
#define WRONG_PASSWORD			           0x30
#define CHANGING_PASSWORD		           0X31

#define NUMBER_OF_WRONG_PASSWORD_ATTEMPTS 	(3)

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description: A function to get the password from user and store it in a given array
 * */
void getPassword(uint8 * arrayName);


/*
 * Description: A function to display Main options
 * */
void appMainOptions(void);

/*
 * Description: a function to initialize the password
 * */
void initializePassword(void);

/*
 * Description: A function to send the password via UART by looping on sendByte function
 * */
void sendPasswordViaUART(uint8 * passwordArray);

/*
 * Description: the call-back function called by the timer every 1 second
 * */
void timerCallBack(void);

/*
 * Description: A function that displays on LCD that door is opening or closing for a certain period of time
 * */
void DoorOpeningTask(void);

#endif /* MC1_H_ */
