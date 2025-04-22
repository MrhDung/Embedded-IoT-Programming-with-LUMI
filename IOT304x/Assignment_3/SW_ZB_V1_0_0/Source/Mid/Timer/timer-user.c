/*
 * timer-user.c
 *
 *  Created on: Jul 21, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Mid/Timer/timer-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl 	systemTimerEventControl;

static uint32_t	 	g_systemTick = 0;
static uint32_t 	g_systemCounter = 0;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		Timer_Init
 *
 * @brief:		The function initializes the timer
 *
 * @params:		msTick
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Timer_Init (uint32_t msTick)
{
	emberEventControlSetInactive(systemTimerEventControl);

	g_systemCounter = 0;
	g_systemTick = msTick;

	emberEventControlSetDelayMS(systemTimerEventControl, g_systemTick);
}

/*
 * @func:		systemTimerEventHandler
 *
 * @brief:		The function calculates the system time
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void systemTimerEventHandler (void)
{
	emberEventControlSetInactive(systemTimerEventControl);

	g_systemCounter += g_systemTick;

	emberEventControlSetDelayMS(systemTimerEventControl, g_systemTick);
}

/*
 * @func:		Timer_GetMilSecTick
 *
 * @brief:		The function retrieves the timestamp
 *
 * @params:		None
 *
 * @retVal:		g_systemCounter
 *
 * @note:		None
 */
uint32_t Timer_GetMilSecTick (void)
{
	return g_systemCounter;
}

/*
 * @func:		CalculatorTime
 *
 * @brief:		The function calculates the time when an event occurs
 *
 * @params[1]:	TimeInit - Event start time
 * @params[2]:	TimeCurrent
 *
 * @retVal:		TimeTotal - Time between events
 *
 * @note:		None
 */
uint32_t CalculatorTime (uint32_t TimeInit, uint32_t TimeCurrent)
{
	uint32_t TimeTotal;

	if (TimeCurrent >= TimeInit)
	{
		TimeTotal = TimeCurrent - TimeInit;
	}
	else
	{
		TimeTotal = 0xFFFFFFFFU + TimeCurrent - TimeInit;
	}

	return TimeTotal;
}

/* END FILE */
