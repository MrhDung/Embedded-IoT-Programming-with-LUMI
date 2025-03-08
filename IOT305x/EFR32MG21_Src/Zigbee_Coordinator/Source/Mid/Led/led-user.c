/*
 * led-user.c
 *
 *  Created on: Jul 26, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Mid/Led/led-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl	led1ToggleEventControl, led2ToggleEventControl;
EmberEventControl	*pLedEventControl[LED_RGB_COUNT];

LedStruct_t g_ledRgbArray[LED_RGB_COUNT][LED_RGB_ELEMENT] = {LED_RGB_1,LED_RGB_2};

LedAction_t g_ledAction[LED_RGB_COUNT];

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		led_Init
 *
 * @brief:		The function initialize Led
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void led_Init (void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	for (int i = 0; i < LED_RGB_COUNT; i++)
	{
		for (int j = 0; j < LED_RGB_ELEMENT; j++)
		{
			GPIO_PinModeSet(g_ledRgbArray[i][j].port,
							g_ledRgbArray[i][j].pin,
							gpioModePushPull,
							0);
		}
	}

	// OFF Led after Init
	led_turnOff(LED_1);
	led_turnOff(LED_2);

	// Call the corresponding LED event
	pLedEventControl[LED_1] = (EmberEventControl *) &led1ToggleEventControl;
	pLedEventControl[LED_2] = (EmberEventControl *) &led2ToggleEventControl;
}

/*
 * @func:		led_turnOff
 *
 * @brief:		The function OFF Led
 *
 * @params:		index - Corresponding LED
 *
 * @retVal:		None
 *
 * @note:		None
 */
void led_turnOff (LedNumber_e index)
{
	for (int j = 0; j < LED_RGB_ELEMENT; j++)
	{
		GPIO_PinOutSet(g_ledRgbArray[index][j].port, g_ledRgbArray[index][j].pin);
	}
}

/*
 * @func:		led_turnOn
 *
 * @brief:		The function ON Led
 *
 * @params[1]:	index - Corresponding LED
 * @params[2]:	color - Corresponding LED color
 *
 * @retVal:		None
 *
 * @note:		None
 */
void led_turnOn (LedNumber_e index, LedColor_e color)
{
	for (int j = 0; j < LED_RGB_ELEMENT; j++)
	{
		if ( ((color >> j) & 0x01) == 1)
		{
			GPIO_PinOutClear(g_ledRgbArray[index][j].port, g_ledRgbArray[index][j].pin);
		}
		else
		{
			GPIO_PinOutSet(g_ledRgbArray[index][j].port, g_ledRgbArray[index][j].pin);
		}
	}
}

/*
 * @func:		toggleLed
 *
 * @brief:		Set the LED flashing parameters and activate the event for the corresponding light
 *
 * @params[1]:	ledIndex - Corresponding LED
 * @params[2]:	color - Corresponding LED color
 * @params[3]:	toggleTime - Number of LED flashes
 * @params[4]:	onTimeMs - LED on time
 * @params[5]:	offTimeMs - LED off time
 *
 * @retVal:		None
 *
 * @note:		None
 */
void toggleLed (LedNumber_e ledIndex, LedColor_e color, uint8_t toggleTime,
				uint32_t onTimeMs, uint32_t offTimeMs)
{
	g_ledAction[ledIndex].ledMode = LED_TOGGLE;
	g_ledAction[ledIndex].color = color;
	g_ledAction[ledIndex].ledToggle.onTimeMs = onTimeMs;
	g_ledAction[ledIndex].ledToggle.offTimeMs = offTimeMs;
	g_ledAction[ledIndex].ledToggle.blinkTime = toggleTime*2;

	emberEventControlSetActive(*pLedEventControl[ledIndex]);
//	if(ledIndex == LED_1){
//		emberEventControlSetActive(led1ToggleEventControl);
//	}
//	else if(ledIndex == LED_2){
//		emberEventControlSetActive(led2ToggleEventControl);
//	}
}

/*
 * @func:		toggleLedHandler
 *
 * @brief:		The function control LED flashing
 *
 * @params:		ledIndex - Corresponding LED
 *
 * @retVal:		None
 *
 * @note:		None
 */
void toggleLedHandler (LedNumber_e ledIndex)
{
	emberEventControlSetInactive(*pLedEventControl[ledIndex]);

	if(g_ledAction[ledIndex].ledToggle.blinkTime != 0 && g_ledAction[ledIndex].ledMode == LED_TOGGLE)
	{
		if(g_ledAction[ledIndex].ledToggle.blinkTime % 2)
		{
			led_turnOn(ledIndex, g_ledAction[ledIndex].color);

			// Call the event again
			emberEventControlSetDelayMS(*pLedEventControl[ledIndex], g_ledAction[ledIndex].ledToggle.onTimeMs);
		}
		else
		{
			led_turnOff(ledIndex);

			// Call the event again
			emberEventControlSetDelayMS(*pLedEventControl[ledIndex], g_ledAction[ledIndex].ledToggle.offTimeMs);
		}

		g_ledAction[ledIndex].ledToggle.blinkTime--;
	}
	else
	{
		g_ledAction[ledIndex].ledMode = LED_FREE;
		led_turnOff(ledIndex);
	}
}

/*
 * @func:		led1ToggleEventHandler
 *
 * @brief:		The function executing LED_1 flash
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void led1ToggleEventHandler (void)
{
	toggleLedHandler(LED_1);
}

/*
 * @func:		led2ToggleEventHandler
 *
 * @brief:		The function executing LED_2 flash
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void led2ToggleEventHandler (void)
{
	toggleLedHandler(LED_2);
}

/* END FILE */
