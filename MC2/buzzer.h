/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the buzzer driver
 *
 * Author: Hisham Elsayed
 *
 *******************************************************************************/

#include "gpio.h"
#include "avr/io.h"
#include "avr/delay.h"

/*******************************************************************************
 *                         Definitions                                         *
 *******************************************************************************/
#define BUZZER_PORT_ID              PORTA_ID
#define BUZZER_PIN_ID               PIN0_ID

/*******************************************************************************
 *                         Function Prototypes                                 *
 *******************************************************************************/

/***************************************************************************************************
 * [Function Name]:         Buzzer_init
 *
 * [Description]:           The Function is used to initialize the buzzer
 *
 * [Arguments]:             VOID
 *
 * [Returns]:               VOID
 ***************************************************************************************************/
void Buzzer_init(void);

/***************************************************************************************************
 * [Function Name]:         Buzzer_Start
 *
 * [Description]:           The Function is used to Activate buzzer
 *
 * [Arguments]:             VOID
 *
 * [Returns]:               VOID
 ***************************************************************************************************/
void Buzzer_Start(void);

/***************************************************************************************************
 * [Function Name]:         Buzzer_Deinit
 *
 * [Description]:           The Function is used to Deactivate buzzer
 *
 * [Arguments]:             VOID
 *
 * [Returns]:               VOID
 ***************************************************************************************************/
void Buzzer_Deinit();
