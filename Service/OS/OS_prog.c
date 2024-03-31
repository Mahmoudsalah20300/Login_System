/*
 * OS_prog.c
 *
 *  Created on: Mar 5, 2023
 *      Author: m_hsa
 */


#include "STD_TYPES.h"

#include "TIMER0_int.h"
#include "GIE_int.h"

#include "OS_priv.h"
#include "OS_types.h"
#include "OS_cfg.h"
#include "OS_int.h"



Task_t SystemTasks[TASK_NUM] = {{NULL}};

void OS_voidStart(void)
{
	MTIMER_enSetCallBack(TIMER0_COMPARE_MATCH, &voidScheduler);
	MGIE_voidEnable();
	//MTIMER0_voidOSInit();
}

void OS_voidCreateTask(u8 copy_u8Priority , u16 copy_u16Periodicity , void (*copy_pvTaskFunc)(void))
{
	SystemTasks[copy_u8Priority].Periodicity = copy_u16Periodicity;

	SystemTasks[copy_u8Priority].TaskFunc = copy_pvTaskFunc;
}

static void voidScheduler(void)
{
	static u16 Local_u16TickCounter=0;
	u8 Local_u8TaskCounter;

	Local_u16TickCounter++;

	/* Loop on all tasks to check their periodicity */
	for(Local_u8TaskCounter=0;Local_u8TaskCounter<TASK_NUM;Local_u8TaskCounter++)
	{
		if((Local_u16TickCounter % SystemTasks[Local_u8TaskCounter].Periodicity) == 0)
		{
			/* Invoke the task function */
			if(SystemTasks[Local_u8TaskCounter].TaskFunc != NULL)
			{
				SystemTasks[Local_u8TaskCounter].TaskFunc();
			}
			else
			{
				/* Do nothing */
			}
		}
	}
}
