/*
 * main_Asm4_IOT303.c
 *
 *  Created on: May 31, 2024
 *      Author: Mr.hDung
 *
 *  Request: Write a program to simulate the automatic screen brightness adjustment feature
 *  		 on a smartphone.
 */


/****************************************************************************************/
/*                                      INCLUDEs                                		*/
/****************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "system_stm32f4xx.h"
#include "timer.h"
#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_adc.h"
#include "stm32f401re_tim.h"
#include "kalman_filter.h"


/****************************************************************************************/
/*                                       DEFINEs                        			    */
/****************************************************************************************/
#define ADC_GPIO_PORT		GPIOC
#define ADC_GPIO_PIN		GPIO_Pin_5
#define ADCx_SENSOR			ADC1
#define ADCx_CLOCK			RCC_APB2Periph_ADC1

#define LED_GREEN_PORT		GPIOA
#define LED_GREEN_PIN_1		GPIO_Pin_0
#define LED_GREEN_PIN_2		GPIO_Pin_11
#define LED_GREEN_CLOCK		RCC_AHB1Periph_GPIOA

#define TIM_PERIOD			8399

#define VALUE_LIGHT_MAX		3400

#define BRIGHTNESS_STEP		70


/****************************************************************************************/
/*                                  STRUCTs AND ENUMs                           		*/
/****************************************************************************************/


/****************************************************************************************/
/*                                  GLOBAL VARIABLEs                 					*/
/****************************************************************************************/
static uint16_t 	g_AdcValue;
static uint16_t 	g_AdcValueUpdate;
uint8_t				g_idTimerStepBrightness = NO_TIMER;


/****************************************************************************************/
/*                                 FUNCTIONs PROTOTYPE                                  */
/****************************************************************************************/
void 		AppInitManager (void);
void 		LightSensor_AdcInit (void);
void		LedControl_TimerOCInit (void);
uint16_t 	LightSensor_AdcPollingRead (void);
void 		TimerOCSetPwm (uint32_t Compare);
void 		ABL_Process (void);
void 		ABL_StepBrightness (void);
void 		ABL_LedControl (uint8_t dutyCycle);


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

	if(g_idTimerStepBrightness != NO_TIMER)
	{
		TimerStop(g_idTimerStepBrightness);
		g_idTimerStepBrightness = NO_TIMER;
	}

	g_idTimerStepBrightness = TimerStart("ABL_StepBrightness",
										100,
										TIMER_REPEAT_FOREVER,
										(void *)ABL_StepBrightness,
										NULL);

	while (1)
	{
		processTimerScheduler();
		ABL_Process();
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
	// Initialize the system clock to 84MHz---------------------------------------------
	SystemCoreClockUpdate();

	// Initialize the system tick timer to handle time events---------------------------
	TimerInit();

	// Initialize the ADC peripheral----------------------------------------------------
	LightSensor_AdcInit();

	// Initialize the TIMER peripheral--------------------------------------------------
	LedControl_TimerOCInit();

	// Initialize the Kalman filter-----------------------------------------------------
	KalmanFilterInit(0.2, 0.8, 0.5);
}

/*
 * @func:  		LightSensor_AdcInit
 *
 * @brief		The function configures the ADC peripheral in continuous scan polling mode
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LightSensor_AdcInit (void)
{
	// Initialize the common ADC and specific ADC struct variables----------------------
	GPIO_InitTypeDef		GPIO_InitStruct;
	ADC_CommonInitTypeDef	ADC_CommonInitStruct;
	ADC_InitTypeDef			ADC_InitStruct;

	/* Initialize GPIO with Analog function---------------------------------------------*/
	// Enable the clock for GPIO--------------------------------------------------------
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	// Configure the GPIO pin-----------------------------------------------------------
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStruct.GPIO_Pin = ADC_GPIO_PIN;
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStruct);

	/* Initialize the ADC----------------------------------------------------------------*/
	// Enable the clock for ADC----------------------------------------------------------
	RCC_APB2PeriphClockCmd(ADCx_CLOCK, ENABLE);

	// Reset ADC values to default-------------------------------------------------------
	ADC_DeInit();

	// Configure the common ADC----------------------------------------------------------
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;

	ADC_CommonInit(&ADC_CommonInitStruct);

	// Configure ADC1--------------------------------------------------------------------
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;

	ADC_Init(ADCx_SENSOR, &ADC_InitStruct);

	// Initialize the channel to be used and the sample time-----------------------------
	ADC_RegularChannelConfig(ADCx_SENSOR, ADC_Channel_15, 1, ADC_SampleTime_15Cycles);

	// Enable ADC1 operation-------------------------------------------------------------
	ADC_Cmd(ADCx_SENSOR, ENABLE);
}

/*
 * @func:  		LedControl_TimerOCInit
 *
 * @brief:		The function configures the TIMER peripheral in Output Compare mode
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LedControl_TimerOCInit (void)
{
	// Declare the struct data types GPIO, Timer Base and Timer Output Compare----------
	GPIO_InitTypeDef			GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef			TIM_OCInitStruct;

	/* Initialize GPIO with Analog function---------------------------------------------*/
	// Enable GPIO clock----------------------------------------------------------------
	RCC_AHB1PeriphClockCmd(LED_GREEN_CLOCK, ENABLE);

	// Configure GPIO pin---------------------------------------------------------------
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStruct.GPIO_Pin = LED_GREEN_PIN_1 | LED_GREEN_PIN_2;
	GPIO_Init(LED_GREEN_PORT, &GPIO_InitStruct);

	GPIO_PinAFConfig(LED_GREEN_PORT, GPIO_PinSource0, GPIO_AF_TIM2);
	GPIO_PinAFConfig(LED_GREEN_PORT, GPIO_PinSource11, GPIO_AF_TIM1);

	/* Initialize Timer-----------------------------------------------------------------*/
	// Enable Timer clock---------------------------------------------------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	// Configure Timer Base to generate a frequency of 10 KHz---------------------------
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = TIM_PERIOD;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

	// Configure Timer Output Compare---------------------------------------------------
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;

	TIM_OC4Init(TIM1, &TIM_OCInitStruct);
	TIM_OC1Init(TIM2, &TIM_OCInitStruct);

	// Enable Timer operation-----------------------------------------------------------
	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	// Enable Timer with PWM function---------------------------------------------------
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

/*
 * @func:		LightSensor_AdcPollingRead
 *
 * @brief:		The function retrieves the light intensity value from the light sensor
 *
 * @param:		None
 *
 * @retval:		result - The ADC value after conversion
 *
 * @note:		None
 */
uint16_t LightSensor_AdcPollingRead (void)
{
	uint16_t result = 0;

	// Start the ADC data conversion process--------------------------------------------
	ADC_SoftwareStartConv(ADCx_SENSOR);

	// Wait for the conversion process to complete--------------------------------------
	while (ADC_GetFlagStatus(ADCx_SENSOR, ADC_FLAG_EOC) == RESET);

	// The ADC value after conversion---------------------------------------------------
	result = ADC_GetConversionValue(ADCx_SENSOR);

	return result;
}

/*
 * @func:  		TimerOCSetPwm
 *
 * @brief:		The function sets the pulse width of the Timer
 *
 * @param:		Compare - Pulse width
 *
 * @retval:		None
 *
 * @note:		None
 */
void TimerOCSetPwm (uint32_t Compare)
{
	TIM_SetCompare4(TIM1, Compare);
	TIM_SetCompare1(TIM2, Compare);
}

/*
 * @func:  		ABL_Process
 *
 * @brief:		The function updates the light intensity value with a 100 ms period
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void ABL_Process (void)
{
 	uint32_t dwTimeCurrent;
 	static uint32_t dwTimeTotal, dwTimeInit;

 	dwTimeCurrent = GetMilSecTick();

 	if (dwTimeCurrent >= dwTimeInit)
 	{
 		dwTimeTotal += dwTimeCurrent - dwTimeInit;
 	}
 	else
 	{
 		dwTimeTotal += 0xFFFFFFFFU + dwTimeCurrent - dwTimeInit;
 	}

 	if (dwTimeTotal >= 100)
 	{
 		// Time scan 100ms
 		dwTimeTotal = 0;

 		g_AdcValueUpdate = LightSensor_AdcPollingRead();
 		g_AdcValueUpdate = KalmanFilter_updateEstimate(g_AdcValueUpdate);
 	}

 	dwTimeInit = dwTimeCurrent;
}

/*
 * @func:  		ABL_StepBrightness
 *
 * @brief:		The function checks for changes in the ADC value and updates the PWM value
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		The function's purpose is:
 *				- Gradually increase/decrease the ADC value to control the brightness of the LED
 *			  	without sudden increments/decrements
 *				- Convert the value to the corresponding percentage ratio (0 - 3400 <=> 0 - 100%)
 *			  	to fit the range of the dutyCycle value
 */
void ABL_StepBrightness (void)
{
	static uint8_t valueScale = 0;		// Variable to store the converted value

	if (g_AdcValue < g_AdcValueUpdate)
	{
		g_AdcValue += BRIGHTNESS_STEP;

		if (g_AdcValue > g_AdcValueUpdate)
		{
			g_AdcValue = g_AdcValueUpdate;
		}
	}
	else if (g_AdcValue > g_AdcValueUpdate)
	{
		g_AdcValue -= BRIGHTNESS_STEP;

		if (g_AdcValue < g_AdcValueUpdate || g_AdcValue > VALUE_LIGHT_MAX)
		{
			g_AdcValue = g_AdcValueUpdate;
		}
	}

	// Convert the value to the corresponding percentage ratio--------------------------
	valueScale = (uint8_t)((g_AdcValue * 100) / VALUE_LIGHT_MAX);

	ABL_LedControl(valueScale);
}

/*
 * @func:  		ABL_LedControl
 *
 * @brief:		The function controls the brightness of the LED
 *
 * @param:		dutyCycle - Operating period
 *
 * @retval:		None
 *
 * @note:		None
 */
void ABL_LedControl (uint8_t dutyCycle)
{
	static uint32_t pulse_length = 0;

	if (dutyCycle > 100)	return;

	// Calculator pulse_length----------------------------------------------------------
	pulse_length = ((TIM_PERIOD * dutyCycle) / 100);

	// Control the brightness of the LED------------------------------------------------
	TimerOCSetPwm(pulse_length);
}


/* END FILE */

