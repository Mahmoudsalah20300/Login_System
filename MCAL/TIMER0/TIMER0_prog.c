/*****************************************************************
 * FILE DESCRIPTION:
----------------------------------------------------------------
 *  file    : TIMER0_prog.c
 *  details : Source file for TIMER0 Driver.
 *****************************************************************/

/******************************************
  INCLUDES
 *******************************************/
#include <avr/interrupt.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "Mcu_HW.h"

#include "TIMER0_priv.h"
#include "TIMER0_types.h"
#include "TIMER0_cfg.h"
#include "TIMER0_int.h"


/******************************************
  Local Data
 *******************************************/
static u32 Number_Of_OverFlow=0;
static u32 Number_Of_Remaining_Time=0;
static u32 Number_Of_CompareMatch = 0;

/******************************************
  Global Data
 *******************************************/


/******************************************
  Local Functions  body
 *******************************************/
/* Global pointer to function to set call back */
void (*OverFlow0Ptr)(void) = NULL;
void (*CompareMatch0Ptr)(void) = NULL;
/******************************************
  Global Functions body
 *******************************************/
/******************************************
  syntax : Error_State TIMER0_enInit(const TIMER0_ConfigType * Copy_xConfig)

  Description : this function is to initialize TIMER/COUNTER0
  Synch/Asynch     : Synchronous
  Reentrance       : Reentrant
  parameters (in)  : pointer to struct of initialization configurations
  parameters (out) : none
  Return Value     : Error_State
 ******************************************/
Error_State MTIMER0_enInit(const TIMER0_ConfigType * Copy_xConfig)
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;
	if(Copy_xConfig != NULL)
	{
		/*resetting TCCR0 register*/
		TCCR0 = 0;
		/********************************Setting mode of timer0********************************/
		/* Checking for correct input of timer mode */
		if(((Copy_xConfig->Mode)>=TIMER0_NORMAL_MODE) && ((Copy_xConfig->Mode)<=TIMER0_FAST_PWM))
		{
			/* Switching over timer modes */
			switch(Copy_xConfig->Mode)
			{
			/* In case of normal mode(over flow mode) */
			case TIMER0_NORMAL_MODE: CLR_BIT(TCCR0,TCCR0_WGM00); CLR_BIT(TCCR0,TCCR0_WGM01); break;
			/* In case of phase correct PWM */
			case TIMER0_PHASE_CORRECT_PWM: SET_BIT(TCCR0,TCCR0_WGM00); CLR_BIT(TCCR0,TCCR0_WGM01); break;
			/* In case of compare match mode */
			case TIMER0_CTC_MODE: CLR_BIT(TCCR0,TCCR0_WGM00); SET_BIT(TCCR0,TCCR0_WGM01); break;
			/* In case of fast PWM mode */
			case TIMER0_FAST_PWM: SET_BIT(TCCR0,TCCR0_WGM00); SET_BIT(TCCR0,TCCR0_WGM01); break;
			default: /* Do nothing */ break;
			}
		}
		/* In case of wrong input of timer mode */
		else
		{
			Local_enErrorState = OUT_OF_RANGE_VALUE;
		}

		/********************************Setting action of OC0 pin*****************************************/
		/* Checking for correct input of OC0 action */
		if(((Copy_xConfig->OC0_Action)>= OC0_DISCONNECTED)&& ((Copy_xConfig->OC0_Action)<=OC0_SET))
		{
			TCCR0 &= OC_ACTION_MASK;  //clearing OC0 bits to receive new values
			TCCR0 |= ((Copy_xConfig->OC0_Action)<<4);  //setting action of OC0 pin
		}
		/* In case of wrong input of OC0 action */
		else
		{
			Local_enErrorState = OUT_OF_RANGE_VALUE;
		}
	}
	else
	{
		Local_enErrorState = NULL_POINTER;
	}


	return Local_enErrorState;
}

/******************************************
  syntax : Error_State MTIMER0_enStart(const TIMER0_PrescalerType Copy_enPrescaler)

  Description : this function is to start TIMER/COUNTER0

  Synch/Asynch     : Synchronous
  Reentrance       : Reentrant
  parameters (in)  : the prescaler value.
  parameters (out) : none
  Return Value     : Error_State
 ******************************************/
Error_State MTIMER0_enStart(const TIMER0_PrescalerType Copy_enPrescaler)
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;

	/********************************Setting prescaler*********************************/
	/* Checking for correct input for prescaler */
	if((Copy_enPrescaler>=TIMER0_NO_PRESCALER) && (Copy_enPrescaler<=TIMER0_1024_PRESCALER))
	{
		TCCR0 &= PRESCALER_MASK;   //clearing clock select bits to receive new values
		TCCR0 |= Copy_enPrescaler; //setting prescaler to run the timer
	}
	else
	{
		Local_enErrorState = OUT_OF_RANGE_VALUE;
	}

	return Local_enErrorState;
}

/******************************************
  syntax : void TIMER0_voidStop(void)

  Description : Stop the Timer0

  Synch/Asynch     : Synchronous
  Reentrance       : Reentrant
  parameters (in)  : void
  parameters (out) : none
  Return Value     : void
 ******************************************/
void MTIMER0_enStop(void)
{
	TCNT0 = 0;
	OCR0  = 0;
	TCCR0 = 0;
}

/******************************************
  syntax : Error_State TIMER0_enGetCounts(u8 * Copy_u8CounterValue)

  Description : return the number of counts since timer has started

  Synch/Asynch     : Synchronous
  Reentrance       : Reentrant
  parameters (in)  : u8 * Copy_u8CounterValue
  parameters (out) : none
  Return Value     : Error_State
 ******************************************/
Error_State MTIMER0_enGetCounts(u8 * Copy_u8CounterValue)
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;
	if(Copy_u8CounterValue != NULL)
	{
		*Copy_u8CounterValue = (Number_Of_OverFlow*255 +TCNT0);
	}
	else
	{
		Local_enErrorState = NULL_POINTER;
	}

	return Local_enErrorState;
}

/******************************************
  syntax : Error_State MTIMER0_enSetDelayTimeMilliSec(const TIMER0_ConfigType * Copy_xConfig , u16 Copy_u16MilliSec)

  Description : delay milliSec function

  Synch/Asynch     : Synchronous
  Reentrance       : Reentrant
  parameters (in)  : the pointer to config structure and number of millisec
  parameters (out) : none
  Return Value     : Error_State
 ******************************************/
Error_State MTIMER0_enSetDelayTimeMilliSec(const TIMER0_ConfigType * Copy_xConfig , u16 Copy_u16MilliSec)
{
	u8 Local_u8Value;
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;

	u16 Local_u16PrescalerArr[5] = {1,8,64,256,1024};

	f32 Local_f32TickTime = (f32) Local_u16PrescalerArr[(Copy_xConfig->Prescaler-1)] / FCPU_CLOCK;

	u32 Local_u32DesiredTicks = ((Copy_u16MilliSec)/Local_f32TickTime);

	if(Copy_xConfig->Mode == TIMER0_NORMAL_MODE)
	{
		Number_Of_OverFlow = (Local_u32DesiredTicks / MAX_NUMBER_OF_OVERFLOW);

		Number_Of_Remaining_Time = (Local_u32DesiredTicks % MAX_NUMBER_OF_OVERFLOW);

		if(Number_Of_Remaining_Time != 0)
		{
			TCNT0 = MAX_NUMBER_OF_OVERFLOW - Number_Of_Remaining_Time;
			Number_Of_OverFlow++;
		}
	}
	else if(Copy_xConfig->Mode == TIMER0_CTC_MODE)
	{
		Local_u8Value = (MAX_NUMBER_OF_OVERFLOW -1);

		while(Local_u32DesiredTicks % Local_u8Value)
		{
			Local_u8Value--;
		}

		Number_Of_CompareMatch = (Local_u32DesiredTicks / Local_u8Value);
		OCR0 = Local_u8Value;
	}
	else
	{
		Local_enErrorState = OUT_OF_RANGE_VALUE;
	}

	return Local_enErrorState;
}


/******************************************
  syntax : Error_State MTIMER0_enEnableInt(const TIMER0_InterruptSource Copy_enInterrupt)

  Description : function to enable the timer0 interrupt.

  Synch/Asynch     : Synchronous
  Reentrance       : Reentrant
  parameters (in)  : the interrupt source
  parameters (out) : none
  Return Value     : Error_State
 ******************************************/
Error_State MTIMER0_enEnableInt(const TIMER0_InterruptSource Copy_enInterrupt)
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;

	/*******************************Setting interrupt****************************************/

	if(Copy_enInterrupt == TIMER0_OVER_FLOW || Copy_enInterrupt == TIMER0_COMPARE_MATCH)
	{
		switch(Copy_enInterrupt)
		{
		case TIMER0_OVER_FLOW: SET_BIT(TIMSK,TIMSK_TOIE0); break;
		case TIMER0_COMPARE_MATCH: SET_BIT(TIMSK,TIMSK_OCIE0); break;
		default: /* Do nothing */ break;
		}
	}
	else
	{
		Local_enErrorState = OUT_OF_RANGE_VALUE;
	}

	return Local_enErrorState;
}

/******************************************
  syntax : Error_State MTIMER0_enDisableInt(const TIMER0_InterruptSource Copy_enInterrupt)

  Description : function to disable the timer0 interrupt.

  Synch/Asynch     : Synchronous
  Reentrance       : Reentrant
  parameters (in)  : the interrupt source
  parameters (out) : none
  Return Value     : Error_State
 ******************************************/
Error_State MTIMER0_enDisableInt(const TIMER0_InterruptSource Copy_enInterrupt)
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;

	/*******************************Setting interrupt****************************************/

	if(Copy_enInterrupt == TIMER0_OVER_FLOW || Copy_enInterrupt == TIMER0_COMPARE_MATCH)
	{
		switch(Copy_enInterrupt)
		{
		case TIMER0_OVER_FLOW: CLR_BIT(TIMSK,TIMSK_TOIE0); break;
		case TIMER0_COMPARE_MATCH: CLR_BIT(TIMSK,TIMSK_OCIE0); break;
		default: /* Do nothing */ break;
		}
	}
	else
	{
		Local_enErrorState = OUT_OF_RANGE_VALUE;
	}

	return Local_enErrorState;
}


/******************************************
  syntax : Error_State MTIMER0_enSetCallBack(TIMER0_InterruptSource Copy_enInterrupt , *Copy_pvCallBackFunc)(void))

  Description : function to Set the Callback Function.

  Synch/Asynch     : Synchronous
  Reentrance       : Reentrant
  parameters (in)  : pointer to the callback function
  parameters (out) : none
  Return Value     : Error_State
 ******************************************/
Error_State MTIMER_enSetCallBack(TIMER0_InterruptSource Copy_enInterrupt , void (*Copy_pvCallBackFunc)(void))
{
	/*local variable for error checking*/
	Error_State Local_enErrorState = NO_ERROR;

	if(Copy_pvCallBackFunc != NULL)
	{
		switch(Copy_enInterrupt)
		{
		case TIMER0_OVER_FLOW: OverFlow0Ptr = Copy_pvCallBackFunc; break;
		case TIMER0_COMPARE_MATCH: CompareMatch0Ptr = Copy_pvCallBackFunc; break;
		default: /* Do nothing */ break;
		}
	}
	else
	{
		Local_enErrorState = NULL_POINTER;
	}

	return Local_enErrorState;
}

void MTIMER0_voidSetFastPWM(u8 Copy_u8DutyCucle , TIMER0_PWMType Copy_enMode)
{
	u8 Local_u8Value;
	switch(Copy_enMode)
	{
	case TIMER0_NON_INVERTING:
		Local_u8Value = (((Copy_u8DutyCucle*MAX_NUMBER_OF_OVERFLOW)/100)-1);
		OCR0 = Local_u8Value;
		break;
	case TIMER0_INVERTING:
		Local_u8Value = ((((MAX_NUMBER_OF_OVERFLOW-1)/MAX_NUMBER_OF_OVERFLOW)-((Copy_u8DutyCucle/100)))*MAX_NUMBER_OF_OVERFLOW);
		OCR0 = Local_u8Value;
		break;
	}
}

ISR(TIMER0_OVF_vect)
{
	static u32 Local_u32Counter=0;
	Local_u32Counter++;

	if(Local_u32Counter == Number_Of_OverFlow)
	{
		OverFlow0Ptr();
		Local_u32Counter=0;
		TCNT0 = MAX_NUMBER_OF_OVERFLOW - Number_Of_Remaining_Time;
	}
}

ISR(TIMER0_COMP_vect)
{
	static u32 Local_u32Counter=0;
	Local_u32Counter++;
	if(Local_u32Counter == 10000)
	{
		CompareMatch0Ptr();
		Local_u32Counter=0;
	}
}

