/*
 * main_Asm1_IOT302.c
 *
 *  Created on: Nov 13, 2023
 *      Author: Mr.hDung
 *
 *  Requirement: Write a program to simulate the Lumi SmartHome touch switch device
 */

/****************************************************************************************/
/*                                      INCLUDEs                                		*/
/****************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "system_stm32f4xx.h"
#include "timer.h"
#include "eventman.h"
#include "led.h"
#include "buzzer.h"
#include "melody.h"
#include "lightsensor.h"
#include "temhumsensor.h"
#include "eventbutton.h"
#include "button.h"
#include "Ucglib.h"


/****************************************************************************************/
/*                                       DEFINEs                        			    */
/****************************************************************************************/
#define CYCLE_LED_CHANGE			20			// Cycle for changing LED brightness
#define PERIOD_SCAN_MULTISENSOR		1000		// Cycle of the sensor scanning


/****************************************************************************************/
/*                                  STRUCTs AND ENUMs                           		*/
/****************************************************************************************/
typedef enum
{
	EVENT_EMPTY,
	EVENT_APP_INIT,
	EVENT_APP_FLUSHMEM_READY
} event_api_t, *event_api_p;

typedef enum
{
	STATE_APP_STARTUP,
	STATE_APP_IDLE,
	STATE_APP_RESET
} state_app_t;


/****************************************************************************************/
/*                                  GLOBAL VARIABLEs                 					*/
/****************************************************************************************/
static ucg_t 		g_ucg;
state_app_t 		g_eCurrentState;		// Current state variable of the program
static uint8_t 		g_ledRed = 0;
static uint8_t 		g_ledGreen = 0;
static uint8_t 		g_ledWhite = 0;
static uint8_t 		g_ledBlue = 0;
static uint8_t 		g_ledChange = 0;
static uint8_t 		g_idTimerStartIncrease = NO_TIMER;
static uint8_t 		g_idTimerStartDecrease = NO_TIMER;
static uint8_t 		g_idTimerDisplayLCD = NO_TIMER;
static uint8_t 		g_idTimerSensorUpdate = NO_TIMER;
static uint16_t 	g_temperature, g_humidity, g_light;
static char 		g_strTemp[30] = "";
static char 		g_strHumi[30] = "";
static char 		g_strLight[30] = "";


/****************************************************************************************/
/*                                 FUNCTIONs PROTOTYPE                                  */
/****************************************************************************************/
void AppInitCommon ();
static void SetStateApp (state_app_t state);
static state_app_t GetStateApp ();
static void AppStateManager (uint8_t event);
void LoadConfiguration ();
void DeviceStateMachine (uint8_t event);
int Clamp(int value, int min , int max);
void Increase_LedLevel ();
void Decrease_LedLevel ();
void MultiSensorScan ();
void Task_multiSensorScan ();


/****************************************************************************************/
/*                                      FUNCTIONs                                       */
/****************************************************************************************/
/*
 * @func:  		main
 *
 * @brief:		Main program
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
int main ()
{
	AppInitCommon();

	// Initialize the main program with the device's previous state before power off
	SetStateApp(STATE_APP_STARTUP);

	// Register initialization events (Add EVENT_APP_INIT initialization events to the Queue)
	EventSchedulerAdd(EVENT_APP_INIT);

	while (1)
	{
		// Handle events according to the pre-set schedule
		processTimerScheduler();

		// Handle events added to the buffer (button...)
		processEventScheduler();
	}
}

/*
 * @func:  		AppInitCommon
 *
 * @brief:		The function to initialize the resources needed
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void AppInitCommon ()
{
	// System clock configuration is 84 MHz
	SystemCoreClockUpdate();

	// Initialize timer system tick to handle time-based events
	TimerInit();

	// Register execution events (Initialize the buffer to store the program's event list)
	EventSchedulerInit(AppStateManager);

	// Configure GPIO pins for the push buttons on the board
	EventButton_Init();

	// Configure GPIO pin for the buzzer on the board
	BuzzerControl_Init();

	// Configure GPIO pins for the RGB LEDs on the board
	LedControl_Init();

	// Configure the ADC peripheral to operate in DMA mode to read data from the light sensor
	// on the board
	LightSensor_Init(ADC_READ_MODE_DMA);

	// Configure the I2C peripheral to communicate with the temperature and humidity sensor
	TemHumSensor_Init();

	// Configure the SPI pins of the STM32 for the LCD screen
	Ucglib4WireSWSPI_begin(&g_ucg, UCG_FONT_MODE_SOLID);

	// Clear the LCD screen
	ucg_ClearScreen(&g_ucg);

	// Set the ncenR10_hr font for display
	ucg_SetFont(&g_ucg, ucg_font_ncenR10_hf);

	// Set the color white for the displayed text
	ucg_SetColor(&g_ucg, 0, 255, 255, 255);

	// Set the color black for the display background
	ucg_SetColor(&g_ucg, 1, 0, 0, 0);

	// Rotate the screen 180 degrees
	ucg_SetRotate180(&g_ucg);
}

/*
 * @func:  		SetStateApp
 *
 * @brief:		The function to set the main program state
 *
 * @param:		state
 *
 * @retval:		None
 *
 * @note:		None
 */
static void SetStateApp (state_app_t state)
{
	// Set the program state
	g_eCurrentState = state;
}

/*
 * @func:  		GetStateApp
 *
 * @brief:		The function to get the main program state
 *
 * @param:		None
 *
 * @retval:		g_eCurrentState - Current state of the program
 *
 * @note:		None
 */
static state_app_t GetStateApp ()
{
	// Return the program state
	return g_eCurrentState;
}

/*
 * @func:  		AppStateManager
 *
 * @brief:		The function to handle the main program events
 *
 * @param:		event
 *
 * @retval:		None
 *
 * @note:		None
 */
static void AppStateManager (uint8_t event)
{
	switch (GetStateApp())
	{
		case STATE_APP_STARTUP:
			if (event == EVENT_APP_INIT)
			{
				LoadConfiguration();
				SetStateApp(STATE_APP_IDLE);
			}
			break;

		case STATE_APP_IDLE:
			DeviceStateMachine(event);
			break;

		case STATE_APP_RESET:
			break;

		default:
			break;
	}
}

/*
 * @func:  		LoadConfiguration
 *
 * @brief:		The function to display information when powering on the device
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LoadConfiguration ()
{
	ucg_DrawString(&g_ucg, 0, 40, 0, "IOT Programming");
	ucg_DrawString(&g_ucg, 55, 65, 0, "by");
	ucg_DrawString(&g_ucg, 5, 90, 0, "Lumi Smarthome");
}

/*
 * @func:  		DeviceStateMachine
 *
 * @brief:		The function to control the LED, buzzer, and display information
 * 				after pressing the buttons on the KIT
 *
 * @param:		event
 *
 * @retval:		None
 *
 * @note:		None
 */
void DeviceStateMachine (uint8_t event)
{
	switch (event)
	{
		case EVENT_OF_BUTTON_3_PRESS_5_TIMES:
		{
			if (g_idTimerSensorUpdate != NO_TIMER)
			{
				TimerStop(g_idTimerSensorUpdate);
				g_idTimerSensorUpdate = NO_TIMER;
			}

			// Display information
			memset(g_strTemp, 0, sizeof(g_strTemp));
			memset(g_strHumi, 0, sizeof(g_strHumi));
			memset(g_strLight, 0, sizeof(g_strLight));
			ucg_ClearScreen(&g_ucg);
			ucg_SetFont(&g_ucg, ucg_font_ncenR08_hf);
			ucg_DrawString(&g_ucg, 0, 12, 0, "Device: Board");
			ucg_DrawString(&g_ucg, 0, 24, 0, "STM32 Nucleo.");
			ucg_DrawString(&g_ucg, 0, 36, 0, "Code: STM32F401RE_");
			ucg_DrawString(&g_ucg, 0, 48, 0, "NUCLEO.");
			ucg_DrawString(&g_ucg, 0, 60, 0, "Manufacturer:");
			ucg_DrawString(&g_ucg, 0, 72, 0, "STMicroelectronics.");
			ucg_DrawString(&g_ucg, 0, 84, 0, "Kit expansion:");
			ucg_DrawString(&g_ucg, 0, 97, 0, "Lumi Smarthome.");
			ucg_DrawString(&g_ucg, 0, 110, 0, "Project:");
			ucg_DrawString(&g_ucg, 0, 123, 0, "Simulator touch switch.");

			// Blink the green LED 5 times
			LedControl_BlinkStart(LED_ALL_ID, BLINK_GREEN, 10, 1000, LED_COLOR_BLACK);

			// Temperature, humidity, and light intensity values
			if (g_idTimerDisplayLCD != NO_TIMER)
			{
				TimerStop(g_idTimerDisplayLCD);
				g_idTimerDisplayLCD = NO_TIMER;
			}

			g_idTimerDisplayLCD = TimerStart("MultiSensorScan", 7000, 1, MultiSensorScan, NULL);
		} break;

		case EVENT_OF_BUTTON_1_PRESS_LOGIC:
		{
			if(g_ledRed == 0)
			{
				g_ledRed = 1;
				g_ledGreen = 0;
				g_ledWhite = 0;
				g_ledBlue = 0;
				LedControl_SetAllColor(LED_COLOR_RED, 50);
			}
			else
			{
				g_ledRed = 0;
				LedControl_SetAllColor(LED_COLOR_RED, 0);
			}

			BuzzerControl_SetMelody(pbeep);
		} break;

		case EVENT_OF_BUTTON_2_PRESS_LOGIC:
		{
			if(g_ledGreen == 0)
			{
				g_ledRed = 0;
				g_ledGreen = 1;
				g_ledWhite = 0;
				g_ledBlue = 0;
				LedControl_SetAllColor(LED_COLOR_GREEN, 50);
			}
			else
			{
				g_ledGreen = 0;
				LedControl_SetAllColor(LED_COLOR_GREEN, 0);
			}

			BuzzerControl_SetMelody(pbeep);
		} break;

		case EVENT_OF_BUTTON_4_PRESS_LOGIC:
		{
			if(g_ledWhite == 0)
			{
				g_ledRed = 0;
				g_ledGreen = 0;
				g_ledWhite = 1;
				g_ledBlue = 0;
				LedControl_SetAllColor(LED_COLOR_WHITE, 50);
			}
			else
			{
				g_ledWhite = 0;
				LedControl_SetAllColor(LED_COLOR_WHITE, 0);
			}

			BuzzerControl_SetMelody(pbeep);
		} break;

		case EVENT_OF_BUTTON_5_PRESS_LOGIC:
		{
			if(g_ledBlue == 0)
			{
				g_ledRed = 0;
				g_ledGreen = 0;
				g_ledWhite = 0;
				g_ledBlue = 1;
				LedControl_SetAllColor(LED_COLOR_BLUE, 50);
			}
			else
			{
				g_ledBlue = 0;
				LedControl_SetAllColor(LED_COLOR_BLUE, 0);
			}

			BuzzerControl_SetMelody(pbeep);
		} break;

		case EVENT_OF_BUTTON_1_HOLD_1S:
		{
			if(g_idTimerStartIncrease != NO_TIMER)
			{
				TimerStop(g_idTimerStartIncrease);
				g_idTimerStartIncrease = NO_TIMER;
			}

			g_idTimerStartIncrease = TimerStart("Increase_Led",
												CYCLE_LED_CHANGE,
												100,
												Increase_LedLevel,
												NULL);
		} break;

		case EVENT_OF_BUTTON_5_HOLD_1S:
		{
			if(g_idTimerStartDecrease != NO_TIMER)
			{
				TimerStop(g_idTimerStartDecrease);
				g_idTimerStartDecrease = NO_TIMER;
			}

			g_idTimerStartDecrease = TimerStart("Decrease_Led",
												CYCLE_LED_CHANGE,
												100,
												Decrease_LedLevel,
												NULL);
		} break;

		case EVENT_OF_BUTTON_1_RELEASED_1S:
		{
			TimerStop(g_idTimerStartIncrease);
		} break;

		case EVENT_OF_BUTTON_5_RELEASED_1S:
		{
			TimerStop(g_idTimerStartDecrease);
		} break;

		default:
			break;
	}
}

/*
 * @func:  		Clamp
 *
 * @brief:		The function to create the permissible operating range
 *
 * @param[1]:	value - Current value
 * @param[2]:	min - Min value
 * @param[3]:	max - Max value
 *
 * @retval:		value
 *
 * @note:		None
 */
int Clamp (int value, int min , int max)
{
	if(value < min) value = min;
	if(value > max) value = max;
	return value;
}

/*
 * @func:  		Increase_LedLevel
 *
 * @brief:		The function to increase the LED brightness
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void Increase_LedLevel ()
{
	g_ledChange = Clamp((int)(g_ledChange + 1), 0 , 100);
	LedControl_SetAllColor(LED_COLOR_GREEN, g_ledChange);
}

/*
 * @func:  		Decrease_LedLevel
 *
 * @brief:		The function to decrease the LED brightness
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void Decrease_LedLevel ()
{
	g_ledChange = Clamp((int)(g_ledChange - 1), 0, 100);
	LedControl_SetAllColor(LED_COLOR_GREEN, g_ledChange);
}

/*
 * @func:  		MultiSensorScan
 *
 * @brief:		The function to update temperature, humidity, and light intensity
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void MultiSensorScan ()
{
	ucg_ClearScreen(&g_ucg);
	ucg_SetFont(&g_ucg, ucg_font_ncenR10_hf);

	if (g_idTimerSensorUpdate != NO_TIMER)
	{
		TimerStop(g_idTimerSensorUpdate);
		g_idTimerSensorUpdate = NO_TIMER;
	}

	g_idTimerSensorUpdate = TimerStart("Task_multiSensorScan",
									PERIOD_SCAN_MULTISENSOR,
									TIMER_REPEAT_FOREVER,
									Task_multiSensorScan,
									NULL);
}

/*
 * @func:  		Task_multiSensorScan
 *
 * @brief:		The function to get temperature, humidity, and light intensity values
 * 				from the sensor and display them on the LCD
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void Task_multiSensorScan ()
{
		// Get temperature, humidity, and light intensity values from the sensor
		g_temperature = (TemHumSensor_GetTemp() / 100);
		g_humidity =  (TemHumSensor_GetHumi() / 100);
		g_light = LightSensor_MeasureUseDMAMode();

		// Save temperature, humidity, and light intensity values
		memset(g_strTemp, 0, sizeof(g_strTemp));
		memset(g_strHumi, 0, sizeof(g_strHumi));
		memset(g_strLight, 0, sizeof(g_strLight));

		sprintf(g_strTemp, "Temp = %d oC     ", g_temperature);
		sprintf(g_strHumi, "Humi = %d %%     ", g_humidity);
		sprintf(g_strLight, "Light = %d Lux     ", g_light);

		// Display on the LCD
		ucg_DrawString(&g_ucg, 0, 40, 0, g_strTemp);
		ucg_DrawString(&g_ucg, 0, 65, 0, g_strHumi);
		ucg_DrawString(&g_ucg, 0, 90, 0, g_strLight);
}

/* END FILE */

