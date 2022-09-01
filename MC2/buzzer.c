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
#include "buzzer.h"

/*******************************************************************************
 *                         Function Definitions                                *
 *******************************************************************************/

void Buzzer_init(void)
{
	/* setup buzzer pin direction as output pin */
	GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID, PIN_OUTPUT);
}

void Buzzer_Start(void)
{
	/* Activate Buzzer */
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH);
}

void Buzzer_Deinit()
{
	/* Disable Buzzer */
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}
