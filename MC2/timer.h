/*******************************************************************************
 *  [FILE NAME]: timer.h
 *
 *  [Author]: Hisham Elsayed
 *
 *  [DATE CREATED]: Oct 24, 2021
 *
 *  [DESCRIPTION]: Header file for Full Timer0 and Timer2 Driver
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum{
	Timer0, Timer1, Timer2
}Timer_type;

typedef enum
{
	Normal, CTC
}Timer_mode;

typedef enum
{
	No_clock,No_prescaling,Prescale_8,Prescale_64,Prescale_256,Prescale_1024
}Timer_clock;

typedef struct
{
		Timer_type type;
		Timer_mode mode;
		uint16 initialvalue;
		uint16 comparevalue;
		Timer_clock prescale;
		void (*callBackPtr)(void);
}Timer_Config;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description: A function that initialize a timer with dynamic configurations.
 *
 * Restrictions: - for Timer1 CTC mode, it's configured to control channel A.
 * 				 - support normal port operations, disable any timer-related pins.
 * */
void Timer_init(const Timer_Config* TimerConfig);


/*
 * Description: A function to disable a specific timer
 *  the function disables: the clock, overflow & CTC interrupts
 *  the function clears: initial value, compare value & the call-back function
 * */
void Timer_deinit(Timer_type type);

#endif /* TIMER_H_ */
