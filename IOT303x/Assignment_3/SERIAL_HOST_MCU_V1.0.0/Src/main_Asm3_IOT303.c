/*
 * main_Asm3_IOT303.c
 *
 *  Created on: May 19, 2024
 *      Author: Mr.hDung
 *
 *  Request: Write a program for communication between the STM32F4 microcontroller
 *  		 and the PC_Simulator_KIT software
 */

/****************************************************************************************/
/*                                      INCLUDEs                                		*/
/****************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "system_stm32f4xx.h"
#include "timer.h"
#include "led.h"
#include "buzzer.h"
#include "melody.h"
#include "lightsensor.h"
#include "temhumsensor.h"
#include "eventbutton.h"
#include "button.h"
#include "ucg.h"
#include "Ucglib.h"
#include "buff.h"
#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_usart.h"
#include "misc.h"

/****************************************************************************************/
/*                                       DEFINEs                        			    */
/****************************************************************************************/
// The buffer size value (FIFO)
#define SIZE_QUEUE_DATA_RX 				256

#define USART2_GPIO_PORT				GPIOA
#define USART2_TX_GPIO_PIN				GPIO_Pin_2
#define USART2_RX_GPIO_PIN				GPIO_Pin_3
#define USART2_GPIO_RCC					RCC_AHB1Periph_GPIOA
#define USART2_RCC						RCC_APB1Periph_USART2
#define USART_BAUDRATE					57600

// The size of the array storing data retrieved from the queue
#define RX_BUFFER_SIZE 					16

#define FRAME_SOF 						0xB1
#define FRAME_ACK 						0x06
#define FRAME_NACK 						0x15
#define CXOR_INIT_VAL 					0xFF

// Message retrieving information and status of the device from the host computer
#define CMD_TYPE_GET               		0x00

// Message responding with the device's information and status to the host computer
#define CMD_TYPE_RES               	 	0x01

// Message for controlling the device from the host computer
#define CMD_TYPE_SET               	 	0x02

#define CMD_OPT							0x00
#define CMD_SEQUENCE					0x00
#define CMD_ID_LED 						0x01
#define CMD_ID_BUZZER					0x04
#define CMD_ID_BUTTON 					0x82
#define CMD_ID_TEMP_SENSOR 				0x84
#define CMD_ID_HUMI_SENSOR 				0x85
#define CMD_ID_LIGHT_SENSOR 			0x86
#define CMD_ID_LCD						0x87

#define CMD_ID							g_strRxBuffer[2]
#define CMD_TYPE						g_strRxBuffer[3]
#define CMD_DATA1						g_strRxBuffer[4]
#define CMD_DATA2						g_strRxBuffer[5]
#define CMD_DATA3						g_strRxBuffer[6]
#define CMD_DATA4						g_strRxBuffer[7]
#define CMD_DATA5						g_strRxBuffer[8]

#define CMD_DATA_LED_ID					CMD_DATA1
#define CMD_DATA_LED_COLOR				CMD_DATA2
#define CMD_DATA_LED_NUMBLINK			CMD_DATA3
#define CMD_DATA_LED_INTERVAL			CMD_DATA4
#define CMD_DATA_LED_LASTSTATE			CMD_DATA5
#define CMD_DATA_BUZZER_STATE			CMD_DATA1
#define CMD_DATA_BUTTON_EVENT			CMD_DATA1
#define CMD_DATA_BUTTON_STATE			CMD_DATA2
#define CMD_DATA_LCD					CMD_DATA1

// LED brightness adjustment cycle
#define CYCLE_LED_CHANGE				20

// Sensor scanning cycle
#define PERIOD_SCAN_MULTISENSOR			1000

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

typedef enum {
    RX_STATE_START_BYTE,
    RX_STATE_DATA_BYTES,
    RX_STATE_CXOR_BYTE
} RX_STATE;

typedef enum {
    USART_STATE_IDLE,
    USART_STATE_DATA_RECEIVED,
    USART_STATE_ACK_RECEIVED,
    USART_STATE_NACK_RECEIVED,
    USART_STATE_ERROR,
    USART_STATE_RX_TIMEOUT,
} USART_STATE;

/****************************************************************************************/
/*                                  GLOBAL VARIABLEs                 					*/
/****************************************************************************************/
ucg_t 			g_ucg;

// Variable representing the current state of the program
state_app_t 	g_eCurrentState;

uint8_t 		g_ledRed = 0;
uint8_t 		g_ledGreen = 0;
uint8_t 		g_ledWhite = 0;
uint8_t 		g_ledBlue = 0;
uint8_t 		g_ledChange = 0;
uint8_t 		g_idTimerStartIncrease = NO_TIMER;
uint8_t 		g_idTimerStartDecrease = NO_TIMER;
uint8_t 		g_idTimerDisplayLCD = NO_TIMER;
uint8_t 		g_idTimerSensorUpdate = NO_TIMER;
uint8_t 		g_B3Count = 0;
uint16_t 		g_temperature, g_humidity, g_light;
uint8_t 		g_RxBufState;

// Variable storing the position of an element in the array holding data retrieved from the queue
uint8_t 		g_IndexRxBuf;

// Array storing data passed to FIFO (input data)
uint8_t 		g_strRxBufData[SIZE_QUEUE_DATA_RX];

// Pointer to reference the FIFO object
buffqueue_t 	g_serialQueueRx;

// Array storing data retrieved from the queue
uint8_t 		g_strRxBuffer[RX_BUFFER_SIZE] = {0};

uint8_t 		g_byCheckXorRxBuf;
uint8_t 		g_Data_Receive = 0;
char 			g_strTemp[30] = "";
char 			g_strHumi[30] = "";
char 			g_strLight[30] = "";

/****************************************************************************************/
/*                                 FUNCTIONs PROTOTYPE                                  */
/****************************************************************************************/
void 			AppInitManager (void);
void 			AppStateManager (uint8_t event);
void 			SetStateApp (state_app_t state);
state_app_t 	GetStateApp (void);
void 			SerialCustom_Init (void);
void 			USART2_Init (void);
void 			USART2Modify_IRQHandler (void);
void			LoadConfiguration (void);
void 			DeviceStateMachine (uint8_t event);
uint8_t 		Clamp (uint8_t value, uint8_t min , uint8_t max);
void 			Increase_LedLevel (void);
void 			Decrease_LedLevel (void);
void 			MultiSensorScan (void);
void 			Task_MultiSensorScan (void);
void 			Serial_SendPacketCustom (uint8_t byOption,uint8_t byCmdId, uint8_t byCmdType,
										 uint8_t *pPayload, uint8_t byLengthPayload);
void 			processSerialReceiverCustom (void);
uint8_t 		PollRxBuff (void);
void 			ButtonCmdSetState (uint8_t button_event, uint8_t button_state);
void 			LedCmdSetState (uint8_t led_id, uint8_t led_color, uint8_t led_num_blink,
		 	 	 	 	 	 	uint8_t led_interval, uint8_t led_last_state);
void 			BuzzerCmdSetState (uint8_t buzzer_state);
void 			LcdCmdSetState (char *text);

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
	AppInitManager();

	// Initializing the main program to the previous state of the device before power-off
	SetStateApp(STATE_APP_STARTUP);

	// Adding initialization events when the system starts operating to the Queue
	EventSchedulerAdd(EVENT_APP_INIT);

	while (1)
	{
		processTimerScheduler();
		processEventScheduler();

		// Processing received messages in the format from the simulation software
		processSerialReceiverCustom();
	}
}

/*
 * @func:  		AppInitManager
 *
 * @brief:		The function to initialize the resources needed for usage
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void AppInitManager (void)
{
	SystemCoreClockUpdate();
	TimerInit();

	// Initializing the buffer to store the event list of the program
	EventSchedulerInit(AppStateManager);

	EventButton_Init();
	BuzzerControl_Init();
	LedControl_Init();
	TemHumSensor_Init();

	// Configure the ADC peripheral to operate in DMA mode to read data from the light sensor
	LightSensor_Init(ADC_READ_MODE_DMA);

	SerialCustom_Init();

	Ucglib4WireSWSPI_begin(&g_ucg, UCG_FONT_MODE_SOLID);
	ucg_ClearScreen(&g_ucg);
	ucg_SetFont(&g_ucg, ucg_font_ncenR10_hf);
	ucg_SetColor(&g_ucg, 0, 255, 255, 255);
	ucg_SetColor(&g_ucg, 1, 0, 0, 0);
	ucg_SetRotate180(&g_ucg);
}

/*
 * @func:  		AppStateManager
 *
 * @brief		Event handling function of the main program
 *
 * @param:		event - Event when pressing the button on the KIT board
 *
 * @retval:		None
 *
 * @note:		None
 */
void AppStateManager (uint8_t event)
{
	switch (GetStateApp())
	{
		case STATE_APP_STARTUP:
		{
			if (event == EVENT_APP_INIT)
			{
				LoadConfiguration();
				SetStateApp(STATE_APP_IDLE);
			}
		} break;

		case STATE_APP_IDLE:
		{
			DeviceStateMachine(event);
		} break;

		case STATE_APP_RESET:
			break;

		default:
			break;
	}
}

/*
 * @func:  		SetStateApp
 *
 * @brief:		The function to set the state of the program
 *
 * @param:		state - The state of the program
 *
 * @retval:		None
 *
 * @note:		None
 */
void SetStateApp (state_app_t state)
{
	g_eCurrentState = state;
}

/*
 * @func:  		GetStateApp
 *
 * @brief:		The function to retrieve the current state of the program
 *
 * @param:		None
 *
 * @retval:		g_eCurrentState - The current state of the program
 *
 * @note:		None
 */
state_app_t GetStateApp (void)
{
	return g_eCurrentState;
}

/*
 * @func:  		SerialCustom_Init
 *
 * @brief:		The function to initialize the FIFO buffer and USART2
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void SerialCustom_Init (void)
{
	bufInit(g_strRxBufData, &g_serialQueueRx, sizeof(g_strRxBufData[0]), SIZE_QUEUE_DATA_RX);
	USART2_Init();
}

/*
 * @func:  		USART2_Init
 *
 * @brief:		The function to configure USART2 communication
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART2_Init (void)
{
	// Declare variables of data type struct GPIO, USART, NVIC-----------------------------
	GPIO_InitTypeDef	GPIO_InitStruct;
	USART_InitTypeDef	USART_InitStruct;
	NVIC_InitTypeDef	NVIC_InitStruct;

	/* Initialize GPIO with alternate function in Open-drain mode--------------------------*/
	// Clock supply for GPIO---------------------------------------------------------------
	RCC_AHB1PeriphClockCmd(USART2_GPIO_RCC, ENABLE);

	// Configure alternate function for GPIO pin-------------------------------------------
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStruct.GPIO_Pin = USART2_TX_GPIO_PIN | USART2_RX_GPIO_PIN;
	GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStruct);

	// Mapping Pin AF----------------------------------------------------------------------
	GPIO_PinAFConfig(USART2_GPIO_PORT, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(USART2_GPIO_PORT, GPIO_PinSource3, GPIO_AF_USART2);

	/* Configure USART2--------------------------------------------------------------------*/
	// Clock supply for USART2-------------------------------------------------------------
	RCC_APB1PeriphClockCmd(USART2_RCC, ENABLE);

	// Configure USART2--------------------------------------------------------------------
	USART_InitStruct.USART_BaudRate = USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART2, &USART_InitStruct);

	USART_Cmd(USART2, ENABLE);

	/* Configure interrupts for USART------------------------------------------------------*/
	// Clock supply for NVIC---------------------------------------------------------------
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	// NVIC Configuration------------------------------------------------------------------
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// Configure NVIC----------------------------------------------------------------------
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);
}

/*
 * @func:  		USART2Modify_IRQHandler
 *
 * @brief:		The function to handle USART2 interrupt
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART2Modify_IRQHandler (void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		g_Data_Receive = (uint8_t)USART_ReceiveData(USART2);
		bufEnDat(&g_serialQueueRx, &g_Data_Receive);
	}

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

/*
 * @func:  		LoadConfiguration
 *
 * @brief		The function to display information on the LCD and PC_Simulator_KIT
 * 				when the device is powered on
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LoadConfiguration (void)
{
	ucg_DrawString(&g_ucg, 0, 40, 0, "IOT Programming");
	ucg_DrawString(&g_ucg, 55, 65, 0, "by");
	ucg_DrawString(&g_ucg, 5, 90, 0, "Lumi Smarthome");

	char text[] = "IOT Programming by Lumi Smarthome";
	Serial_SendPacketCustom(CMD_OPT, CMD_ID_LCD, CMD_TYPE_GET, (uint8_t*)text, sizeof(text));
}

/*
 * @func:  		DeviceStateMachine
 *
 * @brief:		- The function to control LED, Buzzer, and display information on the LCD after
 * 				pressing buttons on the KIT board
 * 				- Simultaneously, send updates of the corresponding LED color status and buzzer state
 * 				to the PC_Simulator_KIT
 *
 * @param:		event - Event when pressing a button on the KIT board
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

			// Display system status information on the LCD screen-------------------------
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

			// Blink the green LED 5 times-------------------------------------------------
			LedControl_BlinkStart(LED_ALL_ID, BLINK_GREEN, 10, 1000, LED_COLOR_BLACK);

			// Temperature, humidity, light intensity parameters---------------------------
			if (g_idTimerDisplayLCD != NO_TIMER)
			{
				TimerStop(g_idTimerDisplayLCD);
				g_idTimerDisplayLCD = NO_TIMER;
			}

			g_idTimerDisplayLCD = TimerStart("MultiSensorScan", 7000, 1,
											(void*) MultiSensorScan, NULL);
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
												(void*) Increase_LedLevel,
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
												(void*) Decrease_LedLevel,
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
 * @brief:		The function to generate a range of values
 *
 * @param[1]:	value - Current value
 * @param[2]:	min - Min value
 * @param[3]:	max - Max value
 *
 * @retval:		value
 *
 * @note:		None
 */
uint8_t Clamp (uint8_t value, uint8_t min , uint8_t max)
{
	if(value < min) value = min;
	if(value > max) value = max;
	return value;
}

/*
 * @func:  		Increase_LedLevel
 *
 * @brief:		The function to increase the brightness of the LED
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void Increase_LedLevel (void)
{
	g_ledChange = Clamp((uint8_t)(g_ledChange + 1), 0 , 100);
	LedControl_SetAllColor(LED_COLOR_GREEN, g_ledChange);
}

/*
 * @func:  		Decrease_LedLevel
 *
 * @brief:		The function to decrease the brightness of the LED
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void Decrease_LedLevel (void)
{
	g_ledChange = Clamp((uint8_t)(g_ledChange - 1), 0, 100);
	LedControl_SetAllColor(LED_COLOR_GREEN, g_ledChange);
}

/*
 * @func:  		MultiSensorScan
 *
 * @brief:		The function to update temperature, humidity, and light intensity values
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void MultiSensorScan (void)
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
									(void*)Task_MultiSensorScan,
									NULL);
}

/*
 * @func:  		Task_MultiSensorScan
 *
 * @brief:		- The function to retrieve temperature, humidity, and light intensity values from
 * 				the sensors and display them on the LCD
 * 				- Simultaneously, send these values to the PC_Simulator_KIT
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void Task_MultiSensorScan (void)
{
	// Retrieve temperature, humidity, and light intensity values from the sensors---------
	g_temperature = (TemHumSensor_GetTemp() / 100);
	g_humidity =  (TemHumSensor_GetHumi() / 100);
	g_light = LightSensor_MeasureUseDMAMode();

	// Store temperature, humidity, and light intensity values-----------------------------
	memset(g_strTemp, 0, sizeof(g_strTemp));
	memset(g_strHumi, 0, sizeof(g_strHumi));
	memset(g_strLight, 0, sizeof(g_strLight));

	sprintf(g_strTemp, "Temp = %d oC     ", g_temperature);
	sprintf(g_strHumi, "Humi = %d %%     ", g_humidity);
	sprintf(g_strLight, "Light = %d Lux     ", g_light);

	// Display on LCD----------------------------------------------------------------------
	ucg_DrawString(&g_ucg, 0, 40, 0, g_strTemp);
	ucg_DrawString(&g_ucg, 0, 65, 0, g_strHumi);
	ucg_DrawString(&g_ucg, 0, 90, 0, g_strLight);

	// Send data to simulation software----------------------------------------------------
	TempSensor_SendPacketRespond(g_temperature);
	HumiSensor_SendPacketRespond(g_humidity);
	LightSensor_SendPacketRespond(g_light);
}

/*
 * @func:  		Serial_SendPacketCustom
 *
 * @brief:		The function send text to PC_Simulator_KIT
 *
 * @param[1]:	byOption - Byte Option of the frame
 * @param[2]:	byCmdId - Byte CmdID of the frame
 * @param[3]:	byCmdType - Byte CmdType of the frame
 * @param[4]:	pPayload - Byte Data of the frame
 * @param[5]:	byLengthPayload - Data size
 *
 * @retval:		None
 *
 * @note:		None
 */
void Serial_SendPacketCustom (uint8_t byOption, uint8_t byCmdId, uint8_t byCmdType,
							  uint8_t *pPayload, uint8_t byLengthPayload)
{
	uint8_t	byLength = 5 + byLengthPayload;
	static uint8_t bySequence = 0;
	uint8_t byCXOR = CXOR_INIT_VAL;
	uint8_t size = 0;
	uint8_t Frame[byLength + 2];

	bySequence++;

	Frame[size++] = FRAME_SOF;
	Frame[size++] = byLength;
	Frame[size++] = byOption;
	Frame[size++] = byCmdId;
	Frame[size++] = byCmdType;

	for (uint8_t i = 0; i < byLengthPayload; i++)
	{
		Frame[size++] = pPayload[i];
	}

	Frame[size++] = bySequence;

	for (uint8_t j = 2; j < size; j++)
	{
		byCXOR ^= Frame[j];
	}
	Frame[size++] = byCXOR;

	for (uint8_t k = 0; k < size; k++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, Frame[k]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	}
}

/*
 * @func:  		processSerialReceiverCustom
 *
 * @brief:		The function to check and process data received from PC_Simulator_KIT
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void processSerialReceiverCustom (void)
{
	uint8_t	RxState = PollRxBuff();

	if (RxState != USART_STATE_IDLE)
	{
		switch (RxState)
		{
			case USART_STATE_DATA_RECEIVED:
			{
				if ((CMD_ID == CMD_ID_LED) && (CMD_TYPE == CMD_TYPE_SET))
				{
					LedCmdSetState(CMD_DATA_LED_ID, CMD_DATA_LED_COLOR, CMD_DATA_LED_NUMBLINK,	\
								   CMD_DATA_LED_INTERVAL, CMD_DATA_LED_LASTSTATE);
				}
				else if ((CMD_ID == CMD_ID_BUZZER) && (CMD_TYPE == CMD_TYPE_SET))
				{
					BuzzerCmdSetState(CMD_DATA_BUZZER_STATE);
				}
				else if ((CMD_ID == CMD_ID_BUTTON) && (CMD_TYPE == CMD_TYPE_SET))
				{
					ButtonCmdSetState(CMD_DATA_BUTTON_EVENT, CMD_DATA_BUTTON_STATE);
				}
				else if ((CMD_ID == CMD_ID_LCD) && (CMD_TYPE == CMD_TYPE_SET))
				{
					LcdCmdSetState((char*)(&CMD_DATA_LCD));
				}
			} break;

			case USART_STATE_ACK_RECEIVED:
				break;

			case USART_STATE_NACK_RECEIVED:
				break;

			case USART_STATE_ERROR:
			case USART_STATE_RX_TIMEOUT:
				break;

			default:
				break;
		}
	}
}

/*
 * @func:  		PollRxBuff
 *
 * @brief:		The function to process received messages based on format
 *
 * @param:		None
 *
 * @retval:		byUartState - USART Status
 *
 * @note:		None
 */
uint8_t PollRxBuff (void)
{
	// Variable to store value after retrieved from Queue
	uint8_t byRxData;

	uint8_t byUartState = (uint8_t) USART_STATE_IDLE;

	while ((bufNumItems(&g_serialQueueRx) != 0) && (byUartState == USART_STATE_IDLE))
	{
		bufDeDat(&g_serialQueueRx, &byRxData);			// Retrieve data from the queue (FIFO)

		switch (g_RxBufState)
		{
			case RX_STATE_START_BYTE:
			{
				if (byRxData == FRAME_SOF)
				{
					g_IndexRxBuf = 0;
					g_byCheckXorRxBuf = CXOR_INIT_VAL;
					g_RxBufState = RX_STATE_DATA_BYTES;
				}
				else if (byRxData == FRAME_ACK)
				{
					byUartState = USART_STATE_ACK_RECEIVED;
				}
				else if (byRxData == FRAME_NACK)
				{
					byUartState = USART_STATE_NACK_RECEIVED;
				}
				else
				{
					byUartState = USART_STATE_ERROR;
				}
			} break;

			case RX_STATE_DATA_BYTES:
			{
				if (g_IndexRxBuf < RX_BUFFER_SIZE)
				{
					// Retrieve the bytes in the transmission frame
					g_strRxBuffer[g_IndexRxBuf] = byRxData;

					if (g_IndexRxBuf > 0)
					{
						g_byCheckXorRxBuf ^= byRxData;		// Calculator CXOR
					}

					g_IndexRxBuf++;

					if (g_IndexRxBuf == *g_strRxBuffer)
					{
						g_RxBufState = RX_STATE_CXOR_BYTE;
					}
				}
				else
				{
					g_RxBufState = RX_STATE_START_BYTE;
					byUartState = USART_STATE_ERROR;
				}
			} break;

			case RX_STATE_CXOR_BYTE:
			{
				if (byRxData == g_byCheckXorRxBuf)
				{
					byUartState = USART_STATE_DATA_RECEIVED;
				}
				else
				{
					byUartState = USART_STATE_ERROR;
				}

				g_RxBufState = RX_STATE_START_BYTE;
			} break;

			default:
				g_RxBufState = RX_STATE_START_BYTE;
				break;
		}
	}

	return byUartState;
}

/*
 * @func:  		ButtonCmdSetState
 *
 * @brief:		The function to control the button when receiving a message from PC_Simulator_KIT
 *
 * @param[1]:	button_event - Corresponding button press event on KIT
 * @param[2]:	button_state - Status of the button press
 *
 * @retval:		None
 *
 * @note:		None
 */
void ButtonCmdSetState (uint8_t button_event, uint8_t button_state)
{
	switch (button_event)
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

				// Display system status information on the LCD screen-------------------------
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

				// Blink the green LED 5 times-------------------------------------------------
				LedControl_BlinkStart(LED_ALL_ID, BLINK_GREEN, 10, 1000, LED_COLOR_BLACK);

				// Temperature, humidity, light intensity parameters---------------------------
				if (g_idTimerDisplayLCD != NO_TIMER)
				{
					TimerStop(g_idTimerDisplayLCD);
					g_idTimerDisplayLCD = NO_TIMER;
				}

				g_idTimerDisplayLCD = TimerStart("MultiSensorScan", 7000, 1, 	\
												(void*)MultiSensorScan, NULL);
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

/*
 * @func:  		LedCmdSetState
 *
 * @brief:		The function to control the turning on/off of the RGB LED on KIT
 * 				when receiving a message from PC_Simulator_KIT
 *
 * @param[1]:	led_id - Corresponding LED on KIT
 * @param[2]:	led_color - Corresponding color of the Led
 * @param[3]:	led_num_blink - Number of Led flashes
 * @param[4]:	led_interval - Led on/off cycle
 * @param[5]:	led_last_state - Last state of the Led
 *
 * @retval:	None
 *
 * @note:	None
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
 * @brief:		The function to control the turning on/off of the horn
 * 				when receiving a message from PC_Simulator_KIT
 *
 * @param:		buzzer_state - Buzzer state
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
}

/*
 * @func:  		LcdCmdSetState
 *
 * @brief:		The function to display a text segment on the LCD screen
 * 				when receiving a message from PC_Simulator_KIT
 *
 * @param:		text - Text segment to be displayed
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

	uint8_t i = 0;
	static char buffer[20];

	memset((uint8_t *)buffer, 0, sizeof(buffer));

	while (text[i] != 0x0D)
	{
		buffer[i] = text[i];
		i++;
	}

	ucg_ClearScreen(&g_ucg);
	ucg_DrawString(&g_ucg, 0, 40, 0, (char *)buffer);
}


/* END FILE */

