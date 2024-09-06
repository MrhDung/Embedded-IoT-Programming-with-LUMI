/*
 * main_Asm2_IOT302.c
 *
 *  Created on: Nov 27, 2023
 *      Author: Mr.hDung
 *
 *  Requirement: Write a program to simulate the Lumi SmartHome touch switch device
 *  			 and interface with the PC Simulator software
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
#include "melody.h"
#include "lightsensor.h"
#include "temhumsensor.h"
#include "eventbutton.h"
#include "button.h"
#include "Ucglib.h"
#include "uartcmd.h"
#include "serial.h"


/****************************************************************************************/
/*                                       DEFINEs                        			    */
/****************************************************************************************/
#define CYCLE_LED_CHANGE			20
#define PERIOD_SCAN_MULTISENSOR		1000


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
state_app_t 		g_eCurrentState;
static uint8_t 		g_ledRed = 0;
static uint8_t 		g_ledGreen = 0;
static uint8_t 		g_ledWhite = 0;
static uint8_t 		g_ledBlue = 0;
static uint8_t 		g_ledChange = 0;
static uint8_t 		g_idTimerStartIncrease = NO_TIMER;
static uint8_t 		g_idTimerStartDecrease = NO_TIMER;
static uint8_t 		g_idTimerDisplayLCD = NO_TIMER;
static uint8_t 		g_idTimerSensorUpdate = NO_TIMER;

// Counter variable for the number of times button B3 is pressed
// when interacting with the simulation software
static uint8_t 		g_B3Count = 0;

static uint16_t 	g_temperature, g_humidity, g_light;
static char 		g_strTemp[30] = "";
static char 		g_strHumi[30] = "";
static char 		g_strLight[30] = "";
char 				g_text[] = "IOT Programming by Lumi Smarthome";


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
void LedCmdSetState (uint8_t led_id, uint8_t led_color, uint8_t led_num_blink,
					 uint8_t led_interval, uint8_t led_last_state);
void BuzzerCmdSetState (uint8_t buzzer_state);
void LcdCmdSetState (char *text);
void ButtonCmdSetState (uint8_t button_id, uint8_t button_state);


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
	SetStateApp(STATE_APP_STARTUP);
	EventSchedulerAdd(EVENT_APP_INIT);

	while (1)
	{
		// Handle events according to the pre-set schedule
		processTimerScheduler();

		// Handle events when a button press is detected
		processEventScheduler();

		// Process received messages according to the format from the simulation software
		processSerialReceiver();
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
	SystemCoreClockUpdate();
	TimerInit();
	EventSchedulerInit(AppStateManager);
	EventButton_Init();
	BuzzerControl_Init();
	LedControl_Init();
	LightSensor_Init(ADC_READ_MODE_DMA);
	TemHumSensor_Init();

	// Configure the UART peripheral in full-duplex mode and initialize the buffer
	// to receive data from the Simulator software
	EventSerial_Init();

	// Process the control message data received from the simulation software
	EventSerial_SetEventLedCallback(LedCmdSetState);
	EventSerial_SetEventButtonCallback(ButtonCmdSetState);
	EventSerial_SetEventBuzzerCallback(BuzzerCmdSetState);
	EventSerial_SetEventLcdCallback(LcdCmdSetState);

	// Initialize the GLCD
	Ucglib4WireSWSPI_begin(&g_ucg, UCG_FONT_MODE_SOLID);
	ucg_ClearScreen(&g_ucg);
	ucg_SetFont(&g_ucg, ucg_font_ncenR10_hf);
	ucg_SetColor(&g_ucg, 0, 255, 255, 255);
	ucg_SetColor(&g_ucg, 1, 0, 0, 0);
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
				Serial_SendPacket(CMD_OPT_NOT_USE, CMD_ID_LCD, CMD_TYPE_SET, 	\
								 (uint8_t*)g_text, sizeof(g_text));
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
 * @brief:		- The function to control the LED, buzzer, and display information
 * 				after pressing the buttons on the KIT
 * 				- Simultaneously, send a status update message with the corresponding LED color
 * 				to the simulation software
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
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_RED, 50);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_RED, 50);
			}
			else
			{
				g_ledRed = 0;
				LedControl_SetAllColor(LED_COLOR_RED, 0);
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_RED, 0);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_RED, 0);
			}

			BuzzerControl_SetMelody(pbeep);
			BuzzerControl_SendPacketRespond(1);
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
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_GREEN, 50);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_GREEN, 50);
			}
			else
			{
				g_ledGreen = 0;
				LedControl_SetAllColor(LED_COLOR_GREEN, 0);
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_GREEN, 0);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_GREEN, 0);
			}

			BuzzerControl_SetMelody(pbeep);
			BuzzerControl_SendPacketRespond(1);
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
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_WHITE, 50);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_WHITE, 50);
			}
			else
			{
				g_ledWhite = 0;
				LedControl_SetAllColor(LED_COLOR_WHITE, 0);
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_WHITE, 0);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_WHITE, 0);
			}

			BuzzerControl_SetMelody(pbeep);
			BuzzerControl_SendPacketRespond(1);
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
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_BLUE, 50);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_BLUE, 50);
			}
			else
			{
				g_ledBlue = 0;
				LedControl_SetAllColor(LED_COLOR_BLUE, 0);
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_BLUE, 0);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_BLUE, 0);
			}

			BuzzerControl_SetMelody(pbeep);
			BuzzerControl_SendPacketRespond(1);
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
 * @brief:		- The function to get temperature, humidity, and light intensity values
 * 				from the sensor and display them on the LCD
 * 				- Simultaneously, send these values to the simulation software
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

	// Send information to the simulation software
	TempSensor_SendPacketRespond(g_temperature);
	HumiSensor_SendPacketRespond(g_humidity);
	LightSensor_SendPacketRespond(g_light);
}

/*
 * @func:  		LedCmdSetState
 *
 * @brief:		The function to control the LED when a message is received
 * 				from the simulation software
 *
 * @param[1]:	led_id - Corresponding LED on KIT
 * @param[2]:	led_color - Corresponding color of the Led
 * @param[3]:	led_num_blink - Number of Led flashes
 * @param[4]:	led_interval - Led on/off cycle
 * @param[5]:	led_last_state - Last state of the Led
 *
 * @retval:		None
 *
 * @note:		None
 */
void LedCmdSetState (uint8_t led_id, uint8_t led_color, uint8_t led_num_blink,
		 	 	 	 uint8_t led_interval, uint8_t led_last_state)
{
	if (led_id == LED_KIT_ID0)
	{
		if (led_last_state == LED_COLOR_RED)
		{
			LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_RED, 50);
		}
		else if (led_last_state == LED_COLOR_GREEN)
		{
			LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_GREEN, 50);
		}
		else if (led_last_state == LED_COLOR_BLUE)
		{
			LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_BLUE, 50);
		}
		else if (led_last_state == LED_COLOR_BLACK)
		{
			LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_RED, 0);
			LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_GREEN, 0);
			LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_BLUE, 0);
		}
	}
	else if (led_id == LED_KIT_ID1)
	{
		if (led_last_state == LED_COLOR_RED)
		{
			LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_RED, 50);
		}
		else if (led_last_state == LED_COLOR_GREEN)
		{
			LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_GREEN, 50);
		}
		else if (led_last_state == LED_COLOR_BLUE)
		{
			LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_BLUE, 50);
		}
		else if (led_last_state == LED_COLOR_BLACK)
		{
			LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_RED, 0);
			LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_GREEN, 0);
			LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_BLUE, 0);
		}
	}
}

/*
 * @func:  		BuzzerCmdSetState
 *
 * @brief:		The function to control the buzzer when a message is received
 * 				from the simulation software
 *
 * @param:		buzzer_state
 *
 * @retval:		None
 *
 * @note:		None
 */
void BuzzerCmdSetState (uint8_t buzzer_state)
{
	if (buzzer_state == 100)
	{
		BuzzerControl_SetMelody(pbeep);
	}
//	else
//	{
//		BuzzerControl_SetMelody(0);
//	}
}

/*
 * @func:  		LcdCmdSetState
 *
 * @brief:		The function to display a text segment on the LCD screen when a message
 * 				is received from the simulation software
 *
 * @param:		text - Received message
 *
 * @retval:		None
 *
 * @note:		None
 */
void LcdCmdSetState (char *text)
{
	if (g_idTimerSensorUpdate != NO_TIMER)
	{
		TimerStop(g_idTimerSensorUpdate);
		g_idTimerSensorUpdate = NO_TIMER;
	}

	memset(g_strTemp, 0, sizeof(g_strTemp));
	memset(g_strHumi, 0, sizeof(g_strHumi));
	memset(g_strLight, 0, sizeof(g_strLight));

	ucg_ClearScreen(&g_ucg);
	ucg_DrawString(&g_ucg, 0, 40, 0, text);
}

/*
 * @func:  		ButtonCmdSetState
 *
 * @brief:		The function to control the push buttons when a message is received
 * 				from the simulation software
 *
 * @param[1]:	button_id
 * @param[2]:	button_state
 *
 * @retval:		None
 *
 * @note:		None
 */
void ButtonCmdSetState (uint8_t button_id, uint8_t button_state)
{
	switch (button_id)
	{
		case EVENT_OF_BUTTON_3_PRESS_LOGIC:
		{
			g_B3Count++;

			if (g_B3Count == 5)
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

				g_idTimerDisplayLCD = TimerStart("MultiSensorScan", 7000, 1, 	\
												 MultiSensorScan, NULL);
				g_B3Count = 0;
			}
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
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_RED, 50);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_RED, 50);
			}
			else
			{
				g_ledRed = 0;
				LedControl_SetAllColor(LED_COLOR_RED, 0);
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_RED, 0);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_RED, 0);
			}

			BuzzerControl_SetMelody(pbeep);
			BuzzerControl_SendPacketRespond(1);
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
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_GREEN, 50);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_GREEN, 50);
			}
			else
			{
				g_ledGreen = 0;
				LedControl_SetAllColor(LED_COLOR_GREEN, 0);
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_GREEN, 0);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_GREEN, 0);
			}

			BuzzerControl_SetMelody(pbeep);
			BuzzerControl_SendPacketRespond(1);
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
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_WHITE, 50);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_WHITE, 50);
			}
			else
			{
				g_ledWhite = 0;
				LedControl_SetAllColor(LED_COLOR_WHITE, 0);
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_WHITE, 0);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_WHITE, 0);
			}

			BuzzerControl_SetMelody(pbeep);
			BuzzerControl_SendPacketRespond(1);
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
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_BLUE, 50);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_BLUE, 50);
			}
			else
			{
				g_ledBlue = 0;
				LedControl_SetAllColor(LED_COLOR_BLUE, 0);
				LedControl_SendPacketRespond(LED_KIT_ID0, LED_COLOR_BLUE, 0);
				LedControl_SendPacketRespond(LED_KIT_ID1, LED_COLOR_BLUE, 0);
			}

			BuzzerControl_SetMelody(pbeep);
			BuzzerControl_SendPacketRespond(1);
		} break;

		default:
			break;
	}
}

/* END FILE */

