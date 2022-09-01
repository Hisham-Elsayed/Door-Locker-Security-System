/*******************************************************************************
 *  [FILE NAME]: mc1.c
 *
 *  [Author]: Hisham Elsayed
 *
 *  [DATE CREATED]: Oct 25, 2021
 *
 *  [DESCRIPTION]: Application file for MC1
 *******************************************************************************/
#define F_CPU 8000000UL
#include "mc1.h"
#include "lcd.h"
#include "keypad.h"
#include "timer.h"
#include "avr/delay.h"
#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/

uint8 g_inputPassword[PASS_SIZE];
uint8 g_password_match_status = 0;
uint16 g_seconds = 0;
uint8 g_wrongPasswordCounter=0;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void getPassword(uint8 * arrayName)
{
	uint8 i=0,key=0;
	LCD_moveCursor(1, 0);

	while(i != PASS_SIZE)
	{
		key = KEYPAD_getPressedKey();
		if (key >= 0 && key <= 9) {
			LCD_displayCharacter('*');
			*(arrayName + i) = key;
			i++;
		}
		_delay_ms(KEYPAD_INPUT_DELAY);
	}
	key=0;

	/* stay till user pushes Enter */
	while(KEYPAD_getPressedKey() != 13);
}

void appMainOptions(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "+: Open Door");
	LCD_displayStringRowColumn(1, 0, "-: Change Pass");
}

void initializePassword(void)
{
	while(g_password_match_status == PASSWORD_MISMATCHED)
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "New Pass:");
		LCD_moveCursor(1, 0);
		getPassword(g_inputPassword); /* get the password from user */
		UART_sendByte(READY_TO_SEND);
		while (UART_recieveByte() != READY_TO_RECEIVE); /* wait for a response */
		sendPasswordViaUART(g_inputPassword);

		/* get confirm password from user */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Re-enter Pass");
		LCD_moveCursor(1, 0);
		getPassword(g_inputPassword);
		UART_sendByte(READY_TO_SEND);
		while (UART_recieveByte() != READY_TO_RECEIVE);
		sendPasswordViaUART(g_inputPassword);

		/* wait for a response from Control ECU about passwords matching */
		while (UART_recieveByte() != READY_TO_SEND);
		g_password_match_status = UART_recieveByte();

		if (g_password_match_status == PASSWORD_MISMATCHED){
			LCD_clearScreen();
			LCD_displayString("Incorrect Pass");
			_delay_ms(DISPLAY_MESSAGE_DELAY);
		}
	}
	g_password_match_status = PASSWORD_MISMATCHED;
}

void sendPasswordViaUART(uint8 * passwordArray)
{
	uint8 cnt;
	for (cnt=0;cnt<PASS_SIZE;cnt++){
		UART_sendByte(passwordArray[cnt]);
		_delay_ms(100);
	}
}

void timerCallBack(void){
	g_seconds++;
}

void DoorOpeningTask(void)
{
	g_seconds=0;
	LCD_clearScreen();
	while (g_seconds < DOOR_UNLOCKING_PERIOD);
	LCD_displayStringRowColumn(0, 0, "Opening Door...");


	/* let the door be open for 3 seconds */
	g_seconds = 0;
	LCD_clearScreen();
	while (g_seconds < DOOR_LEFT_OPEN_PERIOD);
	LCD_displayStringRowColumn(0, 0, "Door is now open");

	/* hold the system for 15 seconds & display to user that door is locking */
	g_seconds = 0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Locking Door...");
	while (g_seconds < DOOR_UNLOCKING_PERIOD);
}


int main(void)
{
	/* Enable I-Bit */
	SREG |=(1<<SREG_I);
	UART_configType UART_Config = {DISABLED, ONE_BIT, BIT_8};
	UART_init(UART_BAUD_RATE,&UART_Config);

	/* Timer freq = 8MHz/1024, one clock-cycle time = 128 uSecond
		so to force the timer to produce an interrupt every 1 second:
		we set the compare value to be 1/(128u) = 7813 */
	Timer_Config TIMER_Config = {Timer1, CTC,0, 7813, Prescale_1024, timerCallBack};
	Timer_init(&TIMER_Config);

	/* Initialize LCD */
	LCD_init();

	uint8 receivedByte=0,key=0;

	initializePassword(); /* initialize first-time password */
	appMainOptions(); /* Display application options */

	while(1)
	{
		key=KEYPAD_getPressedKey();
		if (key == '+') {
			LCD_clearScreen();
			LCD_displayString("Enter Pass");
			getPassword(g_inputPassword);
			UART_sendByte(READY_TO_SEND); /* inform Control ECU to start sending */
			sendPasswordViaUART(g_inputPassword);
			/* inform Control ECU the option that user chose */
			UART_sendByte('+');
			/* Control ECU responses [either the password is correct or wrong] */
			receivedByte = UART_recieveByte();
			if (receivedByte == UNLOCKING_DOOR) {
				DoorOpeningTask(); /* start displaying door status on LCD */

			} else if (receivedByte == WRONG_PASSWORD) {
				LCD_clearScreen();
				LCD_displayString("Incorrect Pass");
				_delay_ms(DISPLAY_MESSAGE_DELAY);
				g_wrongPasswordCounter++;
				if(g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS )
				{
					LCD_clearScreen();
					while (g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS)
					{
						LCD_displayStringRowColumn(0, 5, "WARNING!!");
						LCD_displayStringRowColumn(1, 0, "Calling Security");
					}
				}
			}
			appMainOptions(); /* system back to idle & display main options */


		} else if (key == '-') {
			LCD_clearScreen();
			LCD_displayString("Enter Your Pass");
			getPassword(g_inputPassword);
			UART_sendByte(READY_TO_SEND); /* inform Control ECU to start sending */
			sendPasswordViaUART(g_inputPassword);

			/* inform Control ECU the option that user chose */
			UART_sendByte(CHANGE_PASSWORD_OPTION);

			receivedByte = UART_recieveByte();
			if (receivedByte == CHANGING_PASSWORD) {
				initializePassword();
				LCD_clearScreen();
			} else if (receivedByte == WRONG_PASSWORD) {
				LCD_clearScreen();
				LCD_displayString("Incorrect Pass");
				_delay_ms(DISPLAY_MESSAGE_DELAY);
				g_wrongPasswordCounter++;
				if(g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS )
				{
					LCD_clearScreen();
					while (g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS)
					{
						LCD_displayStringRowColumn(0, 5, "WARNING!!");
						LCD_displayStringRowColumn(1, 0, "Calling Security");
					}
				}
			}
			appMainOptions();
		}
	}
}





