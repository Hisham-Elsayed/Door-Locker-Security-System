/******************************************************************************
 *
 * Module: MC2
 *
 * File Name: mc2.h
 *
 * Description: Header file for the Application
 *
 * Author: Hisham Elsayed
 *
 *******************************************************************************/

#ifndef MC2_H_
#define MC2_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASS_SIZE						    (5)
#define DOOR_UNLOCKING_PERIOD				(15)
#define DOOR_LEFT_OPEN_PERIOD				(3)
#define NUMBER_OF_WRONG_PASSWORD_ATTEMPTS 	(3)
#define ALARM_ON_DELAY						(60)
/* following definitions used to communicate with HMI ECU */
#define PASSWORD_MATCHED		(1)
#define PASSWORD_MISMATCHED		(0)
#define READY_TO_SEND			(0x15)
#define READY_TO_RECEIVE		(0x16)
#define CHANGE_PASSWORD_OPTION	(0x18)
#define UNLOCKING_DOOR			(0x25)
#define WRONG_PASSWORD			(0x30)
#define CHANGING_PASSWORD		(0X31)

#define TWI_CONTROL_ECU_ADDRESS				(0x1)
#define EEPROM_STORE_ADDREESS				(0x00)

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description: a function to compare the stored pass with the received one
 * */
uint8 compare_passwords(uint8 a_password1[PASS_SIZE],uint8 a_password2[PASS_SIZE]);

/*
 * Description: a function to initialize the password in first-run OR to change the password
 * */
void initializePassword(void);

/*
 * Decription: A function that rotates on the DC motor for 15 seconds clockwise, stops it for 3 seconds, then
 * 		rotates it anti-clockwise for 15 seconds.
 * */
void DoorOpeningTask(void);

/*
 * Decription: the call-back function called by the timer every 1 second
 * */
void timerCallBack(void);

/*
 * Description: A function to receive the password via UART by looping on receiveByte function
 * */
void receivePasswordViaUART(uint8 * passwordArray);

/*
 * Description: A function to retreive the stored password from EEPROM
 * */
void updateStoredPassword(void);

/*
 * Description: A function to store the password in EEPROM
 * */
void storePassword(void);


#endif /* MC2_H_ */
