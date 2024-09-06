/*
 * main_Asm1_IOT303.c
 *
 *  Created on: Apr 12, 2024
 *      Author: Mr.hDung
 *
 *  Request: Simulate a touch switch device for controlling the opening/closing
 *  		 of curtains in Lumi Smart Home
 */

/****************************************************************************************/
/*                                      INCLUDEs                                		*/
/****************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "timer.h"
#include "misc.h"
#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_exti.h"
#include "stm32f401re_syscfg.h"


/****************************************************************************************/
/*                                       DEFINEs                        			    */
/****************************************************************************************/
#define GPIO_PIN_SET				1
#define GPIO_PIN_RESET				0

#define GPIOA_PORT					GPIOA
#define GPIOB_PORT					GPIOB
#define GPIOC_PORT					GPIOC

#define LEDBOARD_GPIO_PIN			GPIO_Pin_5			// PA5

#define LEDRED_1_GPIO_PIN			GPIO_Pin_1			// PA1
#define LEDRED_2_GPIO_PIN			GPIO_Pin_13			// PB13

#define LEDGREEN_1_GPIO_PIN			GPIO_Pin_0			// PA0
#define LEDGREEN_2_GPIO_PIN			GPIO_Pin_11			// PA11

#define LEDBLUE_1_GPIO_PIN			GPIO_Pin_3			// PA3
#define LEDBLUE_2_GPIO_PIN			GPIO_Pin_10			// PA10

#define BUTTON_B1_IT_PIN			GPIO_Pin_5			// PB5
#define BUTTON_B2_IT_PIN			GPIO_Pin_3			// PB3
#define BUTTON_B3_IT_PIN			GPIO_Pin_4			// PA4
#define BUTTON_B4_IT_PIN			GPIO_Pin_0			// PB0
#define BUTTON_B5_IT_PIN			GPIO_Pin_4			// PB4

#define BUZZER_GPIO_PIN				GPIO_Pin_9			// PC9

#define CLOCK_GPIOA_RCC				RCC_AHB1Periph_GPIOA
#define CLOCK_GPIOB_RCC				RCC_AHB1Periph_GPIOB
#define CLOCK_GPIOC_RCC				RCC_AHB1Periph_GPIOC

#define SYSCFG_GPIO_RCC				RCC_APB2Periph_SYSCFG

#define PERIOD_LED					300
#define PERIOD_BUZZER				300


/****************************************************************************************/
/*                                  STRUCTs AND ENUMs                           		*/
/****************************************************************************************/
/* @brief  Led ID */
enum Led_Idx
{
	LED_BOARD_ID = 0,
	LED_KIT_ID0 = 1,
	LED_KIT_ID1 = 2,
	LED_ALL_ID = 3
};

/* @brief  Led color */
enum led_color
{
    LED_COLOR_RED = 0x00u,
    LED_COLOR_GREEN = 0x01u,
    LED_COLOR_BLUE = 0x02u,
};

/****************************************************************************************/
/*                                  GLOBAL VARIABLEs                 					*/
/****************************************************************************************/
static uint32_t g_B2TimePress = 0;
static uint32_t g_B2TimeRelease = 0;
static uint8_t	g_B2CountPress = 0;

static uint32_t g_B3TimePress = 0;
static uint32_t g_B3TimeRelease = 0;
static uint8_t	g_B3CountPress = 0;

static uint32_t g_B4TimePress = 0;
static uint32_t g_B4TimeRelease = 0;
static uint8_t	g_B4CountPress = 0;

static uint8_t 	g_TimeID_StatusPower = NO_TIMER;
static uint8_t 	g_TimeID_LedRGB = NO_TIMER;
static uint8_t 	g_TimeID_Buzzer = NO_TIMER;


/****************************************************************************************/
/*                                 FUNCTIONs PROTOTYPE                                  */
/****************************************************************************************/
void 			AppInitManager (void);
static void 	LedBuzzer_Init (void);
static void 	Button_Init (void);
void 			LedControl_SetState (uint8_t led_id, uint8_t led_color, uint8_t  led_state);
void 			BlinkLed_StatusPower (void);
void 			BlinkLed_RGB (void);
void 			BuzzerControl_SetBeep (uint8_t Buzzer_state);
void 			Buzzer_Play (void);
void 			LedBuzzer_SetStatus (void);
uint32_t 		CalculatorTime (uint32_t dwTimeInit, uint32_t dwTimeCurrent);
void 			ScanB3 (void);
void 			ScanB2 (void);
void 			ScanB4 (void);


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

	while (1)
	{
		processTimerScheduler();
		ScanB2();
		ScanB3();
		ScanB4();
	}

	return 0;
}

/*
 * @func:  		AppInitManager
 *
 * @brief:		The function initializes the resources needed
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void AppInitManager (void)
{
	// Set the system clock to 84 MHz----------------------------------------------
	SystemCoreClockUpdate();

	// Initialize Timer------------------------------------------------------------
	TimerInit();

	// Initialize LED and Buzzer---------------------------------------------------
	LedBuzzer_Init();

	// Initialize button-----------------------------------------------------------
	Button_Init();

	// State when starting---------------------------------------------------------
	if (g_TimeID_StatusPower != NO_TIMER)
	{
		TimerStop(g_TimeID_StatusPower);
		g_TimeID_StatusPower = NO_TIMER;
	}
	g_TimeID_StatusPower = TimerStart("BlinkLed_StatusPower", PERIOD_LED, 8,
								  	 (void*) BlinkLed_StatusPower, NULL);
}

/*
 * @func:  		LedBuzzer_Init
 *
 * @brief:		The function configures GPIO pins using LED, Buzzer
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void LedBuzzer_Init (void)
{
	// Initialize a struct GPIO variable-------------------------------------------
	GPIO_InitTypeDef GPIO_InitStruct;

	// Enable clock for GPIO-------------------------------------------------------
	RCC_AHB1PeriphClockCmd(CLOCK_GPIOA_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(CLOCK_GPIOB_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(CLOCK_GPIOC_RCC, ENABLE);

	// Configure GPIO--------------------------------------------------------------
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	// The function configures GPIO pins using LED---------------------------------
	GPIO_InitStruct.GPIO_Pin = 	LEDBOARD_GPIO_PIN |
								LEDRED_1_GPIO_PIN |
								LEDGREEN_1_GPIO_PIN |
								LEDGREEN_2_GPIO_PIN |
								LEDBLUE_1_GPIO_PIN |
								LEDBLUE_2_GPIO_PIN;
	GPIO_Init(GPIOA_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = LEDRED_2_GPIO_PIN;
	GPIO_Init(GPIOB_PORT, &GPIO_InitStruct);

	// The function configures GPIO pins using Buzzer------------------------------
	GPIO_InitStruct.GPIO_Pin = BUZZER_GPIO_PIN;
	GPIO_Init(GPIOC_PORT, &GPIO_InitStruct);
}

/*
 * @func:  		Button_Init
 *
 * @brief:		The function configures GPIO pins using Button
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void Button_Init (void)
{
	// Initialize a struct GPIO variable-------------------------------------------
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	// Enable the clock for GPIO---------------------------------------------------
	RCC_AHB1PeriphClockCmd(CLOCK_GPIOA_RCC, ENABLE);
	RCC_AHB1PeriphClockCmd(CLOCK_GPIOB_RCC, ENABLE);

	// Configure GPIO--------------------------------------------------------------
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	// The function configures GPIO pins using Button------------------------------
	GPIO_InitStruct.GPIO_Pin = 	BUTTON_B2_IT_PIN | BUTTON_B4_IT_PIN;
	GPIO_Init(GPIOB_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = BUTTON_B3_IT_PIN;
	GPIO_Init(GPIOA_PORT, &GPIO_InitStruct);

	// Enable the clock for SYSCFG and connect EXTI with GPIO Pin------------------
	RCC_APB2PeriphClockCmd(SYSCFG_GPIO_RCC, ENABLE);

	// Connect the interrupt line with Button--------------------------------------
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);

	// Configure for interrupt usage-----------------------------------------------
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;

	EXTI_InitStruct.EXTI_Line = EXTI_Line3 | EXTI_Line0;
	EXTI_Init(&EXTI_InitStruct);

	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_Init(&EXTI_InitStruct);

	// Configure the interrupt service routine-------------------------------------
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStruct);
}

/*
 * @func:  		LedControl_SetState
 *
 * @brief:		The function control LED
 *
 * @param[1]:	led_id
 * @param[2]:	led_color
 * @param[3]:	led_state
 *
 * @retval:		None
 *
 * @note:		None
 */
void LedControl_SetState (uint8_t led_id, uint8_t led_color, uint8_t  led_state)
{
	if (led_state == 0)
	{
		if (led_id == LED_BOARD_ID)
		{
			if (led_color == LED_COLOR_GREEN)
			{
				GPIO_ResetBits(GPIOA, LEDBOARD_GPIO_PIN);
			}
		}
		else if (led_id == LED_KIT_ID0)
		{
			switch (led_color)
			{
				case LED_COLOR_RED:
				{
					GPIO_ResetBits(GPIOA, LEDRED_1_GPIO_PIN);
				} break;

				case LED_COLOR_GREEN:
				{
					GPIO_ResetBits(GPIOA, LEDGREEN_1_GPIO_PIN);
				} break;

				case LED_COLOR_BLUE:
				{
					GPIO_ResetBits(GPIOA, LEDBLUE_1_GPIO_PIN);
				} break;

				default:
					break;
			}
		}
		else if (led_id == LED_KIT_ID1)
		{
			switch (led_color)
			{
				case LED_COLOR_RED:
				{
					GPIO_ResetBits(GPIOB, LEDRED_2_GPIO_PIN);
				} break;

				case LED_COLOR_GREEN:
				{
					GPIO_ResetBits(GPIOA, LEDGREEN_2_GPIO_PIN);
				} break;

				case LED_COLOR_BLUE:
				{
					GPIO_ResetBits(GPIOA, LEDBLUE_2_GPIO_PIN);
				} break;

				default:
					break;
			}
		}
	}
	else	/* led_state == 1 */
	{
		if (led_id == LED_BOARD_ID)
		{
			if (led_color == LED_COLOR_GREEN)
			{
				GPIO_SetBits(GPIOA, LEDBOARD_GPIO_PIN);
			}
		}
		else if (led_id == LED_KIT_ID0)
		{
			switch (led_color)
			{
				case LED_COLOR_RED:
				{
					GPIO_SetBits(GPIOA, LEDRED_1_GPIO_PIN);
				} break;

				case LED_COLOR_GREEN:
				{
					GPIO_SetBits(GPIOA, LEDGREEN_1_GPIO_PIN);
				} break;

				case LED_COLOR_BLUE:
				{
					GPIO_SetBits(GPIOA, LEDBLUE_1_GPIO_PIN);
				} break;

				default:
					break;
			}
		}
		else if (led_id == LED_KIT_ID1)
		{
			switch (led_color)
			{
				case LED_COLOR_RED:
				{
					GPIO_SetBits(GPIOB, LEDRED_2_GPIO_PIN);
				} break;

				case LED_COLOR_GREEN:
				{
					GPIO_SetBits(GPIOA, LEDGREEN_2_GPIO_PIN);
				} break;

				case LED_COLOR_BLUE:
				{
					GPIO_SetBits(GPIOA, LEDBLUE_2_GPIO_PIN);
				} break;

				default:
					break;
			}
		}
	}
}

/*
 * @func:  		BlinkLed_StatusPower
 *
 * @brief:		The function controls LED blinking at startup
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void BlinkLed_StatusPower (void)
{
	static uint8_t Blink_state = 0;
	Blink_state = !Blink_state;

	if (Blink_state == 0)
	{
		LedControl_SetState(LED_BOARD_ID, LED_COLOR_GREEN, 0);
	}
	else	/* Blink_state == 1 */
	{
		LedControl_SetState(LED_BOARD_ID, LED_COLOR_GREEN, 1);
	}
}

/*
 * @func:  		BlinkLed_RGB
 *
 * @brief:		The function controls RGB LED blinking
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void BlinkLed_RGB (void)
{
	static uint8_t RGB_state = 0;
	RGB_state = !RGB_state;

	if (RGB_state == 0)
	{
		LedControl_SetState(LED_KIT_ID0, LED_COLOR_GREEN, 0);
		LedControl_SetState(LED_KIT_ID1, LED_COLOR_GREEN, 0);
	}
	else	/* RGB_state == 1 */
	{
		LedControl_SetState(LED_KIT_ID0, LED_COLOR_GREEN, 1);
		LedControl_SetState(LED_KIT_ID1, LED_COLOR_GREEN, 1);
	}
}

/*
 * @func:  		BuzzerControl_SetBeep
 *
 * @brief:		The function On/Off Buzzer
 *
 * @param:		Buzzer_state
 *
 * @retval:		None
 *
 * @note:		None
 */
void BuzzerControl_SetBeep (uint8_t Buzzer_state)
{
	if (Buzzer_state == 0)
	{
		GPIO_ResetBits(GPIOC_PORT, BUZZER_GPIO_PIN);
	}
	else
	{
		GPIO_SetBits(GPIOC_PORT, BUZZER_GPIO_PIN);
	}
}

/*
 * @func:  		Buzzer_Play
 *
 * @brief:		The function controls Buzzer
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void Buzzer_Play (void)
{
	static uint8_t Buzz_state = 0;
	Buzz_state = !Buzz_state;
	BuzzerControl_SetBeep (Buzz_state);
}

/*
 * @func:  		LedBuzzer_SetStatus
 *
 * @brief:		The function controls RGB LED and activates Buzzer
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LedBuzzer_SetStatus (void)
{
	// Control LED blinking based on the required number of times------------------
	if (g_TimeID_LedRGB != NO_TIMER)
	{
		TimerStop(g_TimeID_LedRGB);
		g_TimeID_LedRGB = NO_TIMER;
	}
	g_TimeID_LedRGB = TimerStart("BlinkLed_RGB", PERIOD_LED, 10,
								  (void*) BlinkLed_RGB, NULL);

	// Control Buzzer activation based on the required number of times-------------
	if (g_TimeID_Buzzer != NO_TIMER)
	{
		TimerStop(g_TimeID_Buzzer);
		g_TimeID_Buzzer = NO_TIMER;
	}
	g_TimeID_Buzzer = TimerStart("Buzzer_Play", PERIOD_BUZZER, 4,
								(void*) Buzzer_Play, NULL);
}

/*
 * @func:  		CalculatorTime
 *
 * @brief:		The function calculates button press duration
 *
 * @param[1]:	dwTimeInit - Time when button press starts
 * @param[2]:	dwTimeCurrent - Current time
 *
 * @retval:		dwTimeTotal
 *
 * @note:		None
 */
uint32_t CalculatorTime (uint32_t dwTimeInit, uint32_t dwTimeCurrent)
{
	uint32_t dwTimeTotal;

	if (dwTimeCurrent >= dwTimeInit)
	{
		dwTimeTotal = dwTimeCurrent - dwTimeInit;
	}
	else
	{
		dwTimeTotal = 0xFFFFFFFFU + dwTimeCurrent - dwTimeInit;
	}

	return dwTimeTotal;
}

/*
 * @func:  		EXTI4_IRQHandler
 *
 * @brief:		The function handles the interrupt event for B3
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void EXTI4_IRQHandler (void)
{
	if (GPIO_ReadInputDataBit(GPIOA, BUTTON_B3_IT_PIN) == RESET)	// Press
	{
		g_B3TimePress = GetMilSecTick();
		g_B3CountPress++;
	}
	else	/* Release */
	{
		g_B3TimeRelease = GetMilSecTick();
	}

	// Clear bit PR Exit Interrupt-------------------------------------------------
	EXTI_ClearITPendingBit(EXTI_Line4);
}

/*
 * @func:  		ScanB3
 *
 * @brief:		The function continuously checks the B3 button press event
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void ScanB3 (void)
{
	if (g_B3CountPress != 0)
	{
		uint32_t TimeCurrent = GetMilSecTick();

		if (GPIO_ReadInputDataBit(GPIOA, BUTTON_B3_IT_PIN) == RESET)
		{
			if (CalculatorTime(g_B3TimePress, TimeCurrent) >= 500)
			{
				// Event when holding down
			}
		}
		else
		{
			if (CalculatorTime(g_B3TimePress, TimeCurrent) >= 400)
			{
				switch (g_B3CountPress)
				{
					case 1:
					{
						// Event when pressing once
					} break;

					case 2:
					{
						// Event when pressing twice
					} break;

					case 5:
					{
						LedBuzzer_SetStatus();
					} break;

					default:
						break;
				}

				g_B3CountPress = 0;
			}
		}
	}
}

/*
 * @func:  		EXTI3_IRQHandler
 *
 * @brief:		The function handles the interrupt event for B2
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void EXTI3_IRQHandler (void)
{
	if (GPIO_ReadInputDataBit(GPIOB, BUTTON_B2_IT_PIN) == RESET)	// Press
	{
		g_B2TimePress = GetMilSecTick();
		g_B2CountPress++;
	}
	else	/* Release */
	{
		g_B2TimeRelease = GetMilSecTick();
	}

	// Clear bit PR Exit Interrupt-------------------------------------------------
	EXTI_ClearITPendingBit(EXTI_Line3);
}

/*
 * @func:  		ScanB2
 *
 * @brief:		The function continuously checks the B2 button press event
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void ScanB2 (void)
{
	if (g_B2CountPress != 0)
	{
		uint32_t TimeCurrent = GetMilSecTick();

		if (GPIO_ReadInputDataBit(GPIOB, BUTTON_B2_IT_PIN) == RESET)
		{
			if (CalculatorTime(g_B2TimePress, TimeCurrent) > 500)
			{
				LedControl_SetState(LED_KIT_ID0, LED_COLOR_RED, 0);
				LedControl_SetState(LED_KIT_ID1, LED_COLOR_BLUE, 1);
			}
		}
		else
		{
			if (CalculatorTime(g_B2TimePress, TimeCurrent) >= 400)
			{
				switch (g_B2CountPress)
				{
					case 1:
					{
						LedControl_SetState(LED_KIT_ID1, LED_COLOR_BLUE, 0);
					} break;

					case 2:
					{
						LedControl_SetState(LED_KIT_ID0, LED_COLOR_RED, 0);
						LedControl_SetState(LED_KIT_ID1, LED_COLOR_BLUE, 1);
					} break;

					case 5:
					{
						// Event when pressing five times
					} break;

					default:
						break;
				}

				g_B2CountPress = 0;
			}
		}
	}
}

/*
 * @func:  		EXTI0_IRQHandler
 *
 * @brief:		The function handles the interrupt event for B4
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void EXTI0_IRQHandler (void)
{
	if (GPIO_ReadInputDataBit(GPIOB, BUTTON_B4_IT_PIN) == RESET)	// Press
	{
		g_B4TimePress = GetMilSecTick();
		g_B4CountPress++;
	}
	else	/* Release */
	{
		g_B4TimeRelease = GetMilSecTick();
	}

	// Clear bit PR Exit Interrupt-------------------------------------------------
	EXTI_ClearITPendingBit(EXTI_Line0);
}

/*
 * @func:  		ScanB4
 *
 * @brief:		The function continuously checks the B4 button press event
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void ScanB4 (void)
{
	if (g_B4CountPress != 0)
	{
		uint32_t TimeCurrent = GetMilSecTick();

		if (GPIO_ReadInputDataBit(GPIOB, BUTTON_B4_IT_PIN) == RESET)
		{
			if (CalculatorTime(g_B4TimePress, TimeCurrent) > 500)
			{
				LedControl_SetState(LED_KIT_ID0, LED_COLOR_RED, 1);
				LedControl_SetState(LED_KIT_ID1, LED_COLOR_BLUE, 0);
			}
		}
		else
		{
			if (CalculatorTime(g_B4TimePress, TimeCurrent) >= 400)
			{
				switch (g_B4CountPress)
				{
					case 1:
					{
						LedControl_SetState(LED_KIT_ID0, LED_COLOR_RED, 0);
					} break;

					case 2:
					{
						LedControl_SetState(LED_KIT_ID0, LED_COLOR_RED, 1);
						LedControl_SetState(LED_KIT_ID1, LED_COLOR_BLUE, 0);
					} break;

					case 5:
					{
						// Event when pressing five times
					} break;

					default:
						break;
				}

				g_B4CountPress = 0;
			}
		}
	}
}


/* END FILE */

