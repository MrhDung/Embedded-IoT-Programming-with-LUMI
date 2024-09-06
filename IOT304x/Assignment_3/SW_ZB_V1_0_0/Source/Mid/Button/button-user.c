/*
 * button-user.c
 *
 *  Created on: Jul 21, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Mid/Button/button-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl				scanButtonEventControl;

static pPressButtonCallback 	g_pressCallback;
static pHoldButtonCallback 		g_holdCallback;

static s_Button buttonArr[BUTTON_NUMBER] = BUTTON_INIT;

uint8_t g_index = 0;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		Button_Init
 *
 * @brief:		The function initializes the button
 *
 * @params[1]:	pressCallback
 * @params[2]:	holdCallback
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Button_Init (pPressButtonCallback pressCallback, pHoldButtonCallback holdCallback)
{
	g_pressCallback = pressCallback;
	g_holdCallback  = holdCallback;
}

/*
 * @func:		GetButtonIndex
 *
 * @brief:		The function converts the button pin to index
 *
 * @params:		pin - Pin button
 *
 * @retVal:		BUTTON_PIN_TO_INDEX
 *
 * @note:		None
 */
uint8_t GetButtonIndex (uint8_t pin)
{
	for (uint8_t i = 0; i < BUTTON_NUMBER; i++)
	{
		if(buttonArr[i].pin == pin) return i;
	}

	return BUTTON_PIN_TO_INDEX;
}

/** @brief Hal Button Isr
 *
 * This callback is called by the framework whenever a button is pressed on the
 * device. This callback is called within ISR context.
 *
 * @param button The button which has changed state, either BUTTON0 or BUTTON1
 * as defined in the appropriate BOARD_HEADER.  Ver.: always
 * @param state The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED
 * if the button has been released.  Ver.: always
 */
void emberAfHalButtonIsrCallback (uint8_t button, uint8_t state)
{
	// Convert pin number to index
	g_index = GetButtonIndex(button);

	if(g_index == BUTTON_PIN_TO_INDEX) return;

	if (state == BUTTON_PRESSED)
	{
		buttonArr[g_index].timePress = Timer_GetMilSecTick();
		buttonArr[g_index].countButton++;
		emberEventControlSetActive(scanButtonEventControl);
	}
	else if (state == BUTTON_RELEASED)
	{
		// Debounce key presses
		uint32_t currentTime = Timer_GetMilSecTick();

		if(CalculatorTime(buttonArr[g_index].timePress, currentTime) <= 50)
		{
			buttonArr[g_index].countButton--;
		}
		else
		{
			buttonArr[g_index].timeRelease = Timer_GetMilSecTick();
		}
	}
}

/*
 * @func:		scanButtonEventHandler
 *
 * @brief:		The function executes events from button
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void scanButtonEventHandler (void)
{
	emberEventControlSetInactive(scanButtonEventControl);

	for (uint8_t i = 0; i < BUTTON_NUMBER; i++)
	{
		uint32_t TimeCurrent = Timer_GetMilSecTick();

		if (buttonArr[i].countButton != 0)
		{
			if (GPIO_PinInGet(buttonArr[i].port, buttonArr[i].pin) == BUTTON_PRESS_SCHEMATIC)
			{
				if (CalculatorTime(buttonArr[i].timePress, TimeCurrent) == 1000)
				{
					g_holdCallback(buttonArr[i].pin, hold_1s);
					buttonArr[i].holdFlag = 1;
				}
				else if (CalculatorTime(buttonArr[i].timePress, TimeCurrent) == 2000)
				{
					g_holdCallback(buttonArr[i].pin, hold_2s);
					buttonArr[i].holdFlag = 1;
				}
				else if (CalculatorTime(buttonArr[i].timePress, TimeCurrent) == 3000)
				{
					g_holdCallback(buttonArr[i].pin, hold_3s);
					buttonArr[i].holdFlag = 1;
				}
			}
			else
			{
				if (buttonArr[i].holdFlag == 0)
				{
					if (CalculatorTime(buttonArr[i].timePress, TimeCurrent) >= 400)
					{
						switch (buttonArr[i].countButton)
						{
							case press_1:
								g_pressCallback(buttonArr[i].pin, press_1);
								break;

							case press_2:
								g_pressCallback(buttonArr[i].pin, press_2);
								break;

							case press_3:
								g_pressCallback(buttonArr[i].pin, press_3);
								break;

							case press_4:
								g_pressCallback(buttonArr[i].pin, press_4);
								break;

							case press_5:
								g_pressCallback(buttonArr[i].pin, press_5);
								break;

							default:
								break;
						}

						buttonArr[i].countButton = 0;
//						emberAfCorePrintln("SW%d is released after press button", (g_index +1 ));
					}
				}
				else
				{
					buttonArr[i].holdFlag = 0;
					buttonArr[i].countButton = 0;
//					emberAfCorePrintln("SW%d is released after hold button", (g_index +1 ));
				}
			}
		}

		// Only perform the loop while the button is pressed, and not after releasing the button
		if(buttonArr[i].countButton != 0) emberEventControlSetDelayMS(scanButtonEventControl, 100);
	}
}

/* END FILE */
