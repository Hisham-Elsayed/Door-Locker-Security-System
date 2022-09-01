/*******************************************************************************
 *  [FILE NAME]: mc2.c
 *
 *  [Author]: Hisham Elsayed
 *
 *  [DATE CREATED]: Nov 1, 2021
 *
 *  [DESCRIPTION]: Application file for MC2
 *******************************************************************************/
#define F_CPU 8000000UL
#include "uart.h"
#include "std_types.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include "twi.h"
#include "dc_motor.h"
#include "external_eeprom.h"
#include "buzzer.h"
#include "timer.h"
#include "mc2.h"

/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
uint8 g_receivedPassword[PASS_SIZE];
uint8 g_storedPassword[PASS_SIZE];
uint8 g_wrongPasswordCounter=0;
uint16 g_seconds = 0;

/*******************************************************************************
 *                          Function Definitions                               *
 *******************************************************************************/
uint8 compare_passwords(uint8 a_password1[PASS_SIZE],uint8 a_password2[PASS_SIZE]) {
	updateStoredPassword();
	uint8 i;
	for (i = 0; i < PASS_SIZE; i++) {
		if (a_password1[i] != a_password2[i]) {
			return PASSWORD_MISMATCHED;
		}
	}
	return PASSWORD_MATCHED;
}

void DoorOpeningTask(void){
	/* run the DC motor clockwise for 15 seconds */
	g_seconds = 0;
	DcMotor_Rotate(Clockwise);
	while (g_seconds < DOOR_UNLOCKING_PERIOD);

	/* let the door be open for 3 seconds */
	g_seconds = 0;
	DcMotor_Rotate(Stop);
	while (g_seconds < DOOR_LEFT_OPEN_PERIOD);

	/* hold the system for 15 seconds & display to user that door is locking */
	g_seconds = 0;
	DcMotor_Rotate(Anti_Clockwise);
	while (g_seconds < DOOR_UNLOCKING_PERIOD);

	DcMotor_Rotate(Stop);
}

void timerCallBack(void){
	g_seconds++;
}

void initializePassword(void){
	/* do not return from this function till Control ECU sends a PASSWORD_MATCH signal */
	uint8 confirmationPassword[PASS_SIZE];
	uint8 check=0;
	while(!check){
		while (UART_recieveByte() != READY_TO_SEND); /* wait till HMI gets ready */
		UART_sendByte(READY_TO_RECEIVE); /* inform HMI that Control ECU ready to receive the password */
		receivePasswordViaUART(g_receivedPassword);

		while (UART_recieveByte() != READY_TO_SEND);
		UART_sendByte(READY_TO_RECEIVE); /* inform HMI to send the confirmation password */
		receivePasswordViaUART(confirmationPassword);

		if (compare_passwords(g_receivedPassword, confirmationPassword) == PASSWORD_MATCHED){
			UART_sendByte(READY_TO_SEND);
			UART_sendByte(PASSWORD_MATCHED);
			storePassword();
			check=1;
		}else{
			UART_sendByte(READY_TO_SEND);
			UART_sendByte(PASSWORD_MISMATCHED);
		}
	}
}

void receivePasswordViaUART(uint8 * passwordArray){
	uint8 cnt;
	for (cnt=0;cnt<PASS_SIZE;cnt++){
		*(passwordArray+cnt) = UART_recieveByte();
		_delay_ms(100);
	}
}

void updateStoredPassword(void){
	uint8 i;
	for (i=0;i<PASS_SIZE;i++){
		EEPROM_readByte(EEPROM_STORE_ADDREESS+i, g_storedPassword+i);
	}
}

void storePassword(void){
	uint8 i;
	for (i = 0; i < PASS_SIZE; i++) {
		EEPROM_writeByte(EEPROM_STORE_ADDREESS + i, g_receivedPassword[i]);
		_delay_ms(100);
	}
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

	/* initialize I2C */
	TWI_Configurations TWI_Config = {0x02, TWI_CONTROL_ECU_ADDRESS};
	TWI_init(&TWI_Config);

	DcMotor_Init();
	Buzzer_init();

	initializePassword();

	uint8 receivedByte=0;

	while (1)
	{
		if (UART_recieveByte() == READY_TO_SEND){
			receivePasswordViaUART(g_receivedPassword);
			receivedByte = UART_recieveByte();

			if ( receivedByte == '+'){
				if (compare_passwords(g_storedPassword, g_receivedPassword) == PASSWORD_MATCHED){
					UART_sendByte(UNLOCKING_DOOR); /* inform HMI ECU to display that door is unlocking */
					DoorOpeningTask(); /* start opening door process/task */
				}else{
					UART_sendByte(WRONG_PASSWORD);
					/* count number of wrong attempts, and turn on a buzzer of it exceeds the limit */
					g_wrongPasswordCounter++;
					if (g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS)
					{
						Buzzer_Start();
						g_seconds=0;
						while(g_seconds < ALARM_ON_DELAY); /* turn on alarm for a certain period */
						Buzzer_Deinit();
						g_wrongPasswordCounter=0; /* reset the counter */
					}
				}


			} else if (receivedByte == CHANGE_PASSWORD_OPTION) {
				if (compare_passwords(g_storedPassword, g_receivedPassword) == PASSWORD_MATCHED) {
					UART_sendByte(CHANGING_PASSWORD); /* inform HMI to process changing password */
					initializePassword();
				}else{
					UART_sendByte(WRONG_PASSWORD);
					if (g_wrongPasswordCounter == NUMBER_OF_WRONG_PASSWORD_ATTEMPTS)
					{
						Buzzer_Start();
						g_seconds=0;
						while(g_seconds < ALARM_ON_DELAY); /* turn on alarm for a certain period */
						Buzzer_Deinit();
						g_wrongPasswordCounter=0; /* reset the counter */
					}
				}
			}
		}
	}
}



