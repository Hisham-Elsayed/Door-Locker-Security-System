/*******************************************************************************
 *  [FILE NAME]: timer.c
 *
 *  [Author]: Hisham Elsayed
 *
 *  [DATE CREATED]: Oct 24, 2021
 *
 *  [DESCRIPTION]: Source file for Full Timer0 and Timer2 Driver
 *******************************************************************************/

#include "timer.h"
#include "gpio.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include "avr/io.h" /* To use the IO Ports Registers */
#include "avr/interrupt.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_Timer0CallBackPtr)(void) = NULL_PTR;
static volatile void (*g_Timer1CallBackPtr)(void) = NULL_PTR;
static volatile void (*g_Timer2CallBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                        Interrupt Service Routines                           *
 ******************************************************************************/
/* Timer0 OVF mode */
ISR(TIMER0_OVF)
{
	if (g_Timer0CallBackPtr != NULL_PTR) {
		(*g_Timer0CallBackPtr)();
	}
}

/* Timer0 CTC mode */
ISR(TIMER0_CTC)
{
	if (g_Timer0CallBackPtr != NULL_PTR) {
		(*g_Timer0CallBackPtr)();
	}
}

/* Timer1 OVF mode */
ISR(TIMER1_OVF)
{
	if (*g_Timer1CallBackPtr != NULL_PTR) {
		(*g_Timer1CallBackPtr)();
	}
}

/* Timer1 CTC mode */
ISR(TIMER1_CTC)
{
	if (*g_Timer1CallBackPtr != NULL_PTR)
	{
		(*g_Timer1CallBackPtr)();
	}
}

/* Timer2 OVF mode */
ISR(TIMER2_OVF)
{
	if (g_Timer2CallBackPtr != NULL_PTR) {
		(*g_Timer2CallBackPtr)();
	}
}
/* Timer2 CTC mode */
ISR(TIMER2_CTC)
{
	if (g_Timer2CallBackPtr != NULL_PTR) {
		(*g_Timer2CallBackPtr)();
	}
}

/*******************************************************************************
 *                         Function Definitions                                *
 *******************************************************************************/
void Timer_init(const Timer_Config* TimerConfig)
{
	if( (TimerConfig->type) == Timer0 )
	{
		/* starting with adjusting common bits for the timer, then mode-specific bits */
		TCCR0 |= (1<<FOC0);
		TCNT0 = TimerConfig -> initialvalue;
		TCCR0 = 0;
		TCCR0 = (TCCR0 & 0xF8) | ((TimerConfig -> prescale) & 0x07);

		if(TimerConfig -> mode == Normal )
		{
			/* Enable interrupts for overflow mode */
			SET_BIT(TIMSK,TOIE0);
			/* Enable overflow flag */
			SET_BIT(TIFR,TOV0);
		}
		else if (TimerConfig -> mode == CTC )
		{
			SET_BIT(TCCR0,WGM01);
			OCR0 = TimerConfig -> comparevalue;
			SET_BIT(TIMSK,OCIE0); /* enable interrupts for CTC mode */
			SET_BIT(TIFR,OCF0); /* enable CTC flag */
		}
		g_Timer0CallBackPtr = TimerConfig -> callBackPtr;
	}

	else if ( (TimerConfig -> type) == Timer1 )
	{
		TCCR1A =0;
		SET_BIT(TCCR1A,FOC1A); /* for non-PWM mode */
		SET_BIT(TCCR1A,FOC1B);
		TCCR1B = TimerConfig->prescale;
		TCNT1 = TimerConfig->initialvalue;

		if( (TimerConfig -> mode) == Normal )
		{
			/* Enable interrupts for overflow mode */
			SET_BIT(TIMSK,TOIE1);
			/* Enable overflow flag */
			SET_BIT(TIFR,TOV1);
		}
		else if ( (TimerConfig -> mode) == CTC )
		{
			SET_BIT(TCCR1B,WGM12);
			OCR1A = TimerConfig -> comparevalue;
			SET_BIT(TIMSK,OCIE1A); /* enable interrupts for CTC mode */
			SET_BIT(TIFR,OCF1A); /* enable CTC flag */
		}
		g_Timer1CallBackPtr = TimerConfig -> callBackPtr;
	}

	else if (TimerConfig -> type == Timer2 )
	{
		SET_BIT(TCCR2, FOC2); /* for non-PWM */
		TCNT2 = TimerConfig -> initialvalue;
		TCCR2 = 0;
		TCCR2 |= (TimerConfig -> prescale);

		if((TimerConfig -> mode) == Normal )
		{
			/* Enable interrupts for overflow mode */
			SET_BIT(TIMSK,TOIE2);
			/* Enable overflow flag */
			SET_BIT(TIFR,TOV2);
		}
		else if ( (TimerConfig -> mode) == CTC )
		{
			SET_BIT(TCCR1B,WGM21);
			OCR1A = TimerConfig -> comparevalue;
			SET_BIT(TIMSK,OCIE2); /* enable interrupts for CTC mode */
			SET_BIT(TIFR,OCF2); /* enable CTC flag */
		}
		g_Timer2CallBackPtr = TimerConfig -> callBackPtr;
	}
}

void Timer_deinit(Timer_type type)
{
	if ( type == Timer0 )
	{
		TCCR0 = 0;
		TCNT0= 0; /* clear initial value */
		OCR0 = 0; /* clear compare value for CTC mode*/
		CLEAR_BIT(TIMSK,OCIE0); /* disable interrupts for CTC mode */
		CLEAR_BIT(TIMSK,TOIE0); /* disable interrupts for overflow mode */
		g_Timer0CallBackPtr = NULL_PTR; /* clear the call-back function */
	}
	else if ( type == Timer1 )
	{
		TCCR1A =0;
		TCCR1B = 0; /* disable clock */
		TCNT1 = 0;
		CLEAR_BIT(TIMSK, TOIE1); /* disable interrupts for overflow mode */
		OCR1A = 0;  /* clear compare value for CTC mode */
		CLEAR_BIT(TIMSK, OCIE1A); /* disable interrupts for CTC mode */
		g_Timer1CallBackPtr = NULL_PTR;
	}
	else if ( type == Timer2 )
	{
		TCCR2 = 0;
		TCNT2 = 0; /* clear initial value */
		OCR2 = 0; /* clear compare value for CTC mode*/
		CLEAR_BIT(TIMSK, OCIE2); /* disable interrupts for CTC mode */
		CLEAR_BIT(TIMSK, TOIE2); /* disable interrupts for overflow mode */
		g_Timer2CallBackPtr = NULL_PTR;
	}
}



