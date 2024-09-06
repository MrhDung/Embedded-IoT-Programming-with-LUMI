/*
 * main_Asm2_IOT303.c
 *
 *  Created on: May 12, 2024
 *      Author: Mr.hDung
 *
 *  Request: Interface with the temperature-humidity sensor and display the parameters
 *  		 on the LCD screen.
 */

/****************************************************************************************/
/*                                      INCLUDEs                                		*/
/****************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "timer.h"
#include "ucg.h"
#include "Ucglib.h"
#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_i2c.h"
#include "kalman_filter.h"


/****************************************************************************************/
/*                                       DEFINEs                        			    */
/****************************************************************************************/
#define I2C_SPEED					400000					// 400 KBit/s

#define I2C_MASTER_RCC				RCC_APB1Periph_I2C1
#define I2C_MASTER_INSTANCE			I2C1

#define I2C_MASTER_GPIO_RCC			RCC_AHB1Periph_GPIOB
#define I2C_MASTER_PORT				GPIOB
#define SCL_MASTER_PIN				GPIO_Pin_8				//PB8
#define SDA_MASTER_PIN				GPIO_Pin_9				//PB9

#define SENSOR_ADDR					0x40
#define TEMP_CMDCODE				0xE3
#define HUMI_CMDCODE				0xE5

#define TIME_WAIT_GET_TEMP			8
#define TIME_WAIT_GET_HUMI			8

#define PERIOD_UPDATE_SENSOR		1000
#define PERIOD_UPDATE_LCD			5000

#define CHANGE_VALUE_TEMP			2
#define CHANGE_VALUE_HUMI			2

/****************************************************************************************/
/*                                  STRUCTs AND ENUMs                           		*/
/****************************************************************************************/


/****************************************************************************************/
/*                                  GLOBAL VARIABLEs                 					*/
/****************************************************************************************/
static ucg_t ucg;
static char g_strTemp[30] = "";
static char g_strHumi[30] = "";
static double g_temp = 0;
static double g_humi = 0;

/****************************************************************************************/
/*                                 FUNCTIONs PROTOTYPE                                  */
/****************************************************************************************/
static void 	AppInitManager 				(void);
static void 	LCD_Setup 					(void);
static void 	Sensor_Init 				(void);
static void 	I2C_Start 					(void);
static void 	I2C_Address_Direction 		(uint8_t Address, uint8_t Direction);
static void 	I2C_TransmitData	 		(uint8_t data);
static uint8_t 	I2C_Receive_ACK 			(void);
static uint8_t 	I2C_Receive_NACK 			(void);
static void 	I2C_Stop 					(void);
static void 	TemHumSensor_readRegister 	(uint8_t AddrSensor, uint8_t pAddrReg,
											 uint8_t *pDataRead, uint8_t byDataLen,
											 uint16_t rDelay);
static double 	TemHumSensor_getTemp 		(void);
static double 	TemHumSensor_getHumi 		(void);
static void 	Update_ValueSensor 			(void);
static void		Update_LCD 					(void);
static void 	printDataToLCD 				(void);
static void 	delay_ms 					(uint32_t milisecond);

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
		Update_ValueSensor();
		Update_LCD();
	}

	return 0;
}

/*
 * @func:  		AppInitManager
 *
 * @brief:		Initialization function for required resources.
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void AppInitManager (void)
{
	// Set the system clock to 84MHz----------------------------------------------
	SystemCoreClockUpdate();

	// Initialize Timer-----------------------------------------------------------
	TimerInit();

	// Configure settings for the LCD screen--------------------------------------
	LCD_Setup();

	// Initialize communication with the sensor-----------------------------------
	Sensor_Init();

	// Initialize the Kalman filter-----------------------------------------------
	KalmanFilterInit(0.5, 0, 0.5);
}

/*
 * @func:  		LCD_Setup
 *
 * @brief:		The function sets the parameters for the LCD
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void LCD_Setup (void)
{
	// Configure the SPI pins of the STM32 for the LCD screen---------------------
	Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);

	// Clear the LCD display------------------------------------------------------
	ucg_ClearScreen(&ucg);

	// Set the ncenR10_hr font for display----------------------------------------
	ucg_SetFont(&ucg, ucg_font_ncenR10_hf);

	// Set the color to white for the displayed text------------------------------
	ucg_SetColor(&ucg, 0, 255, 255, 255);

	// Set the color to black for the display background--------------------------
	ucg_SetColor(&ucg, 1, 0, 0, 0);

	// Rotate the screen by 180 degrees-------------------------------------------
	ucg_SetRotate180(&ucg);
}

/*
 * @func:  		Sensor_Init
 *
 * @brief:		The function communicates with the sensor (initializes I2C1 in master mode)
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void Sensor_Init (void)
{
	// Declare variables of type GPIO, I2C structures-----------------------------
	GPIO_InitTypeDef 	GPIO_InitStruct;
	I2C_InitTypeDef 	I2C_InitStruct;

	/* Initialize GPIO with alternate function in open-drain mode-----------------*/
	// Enable I2C-----------------------------------------------------------------
	RCC_AHB1PeriphClockCmd(I2C_MASTER_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(I2C_MASTER_RCC, ENABLE);

	// Initialize the GPIO alternate function for I2C-----------------------------
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStruct.GPIO_Pin = SCL_MASTER_PIN | SDA_MASTER_PIN;
	GPIO_Init(I2C_MASTER_PORT, &GPIO_InitStruct);

	// Connect PB6 to I2C1_SCL----------------------------------------------------
	GPIO_PinAFConfig(I2C_MASTER_PORT, GPIO_PinSource8, GPIO_AF_I2C1);

	// Connect PB7 to I2C1_SDA----------------------------------------------------
	GPIO_PinAFConfig(I2C_MASTER_PORT, GPIO_PinSource9, GPIO_AF_I2C1);

	// Configure I2C3 in Master mode----------------------------------------------
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = I2C_SPEED;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;

	I2C_Init(I2C_MASTER_INSTANCE, &I2C_InitStruct);

	// Enable I2C operation-------------------------------------------------------
	I2C_Cmd(I2C_MASTER_INSTANCE, ENABLE);
}

/*
 * @func:  		I2C_Start
 *
 * @brief:		The function sends the Start bit
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void I2C_Start (void)
{
	// Wait until the line is not busy--------------------------------------------
	while (I2C_GetFlagStatus(I2C_MASTER_INSTANCE, I2C_FLAG_BUSY));

	// Send the START bit---------------------------------------------------------
	I2C_GenerateSTART(I2C_MASTER_INSTANCE, ENABLE);

	// Wait for the START bit transmission to complete (wait for SET bit EV5)-----
	while (!I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_MODE_SELECT));
}

/*
 * @func:  		I2C_Address_Direction
 *
 * @brief:		The function sends the address bit
 *
 * @param[1]:	Address - the address to be transmitted
 * @param[2]:	Direction - transmit or receive data
 *
 * @retval:		None
 *
 * @note:		None
 */
static void I2C_Address_Direction (uint8_t Address, uint8_t Direction)
{
	// Transmit the address-------------------------------------------------------
	I2C_Send7bitAddress(I2C_MASTER_INSTANCE, Address, Direction);

	// Check the condition: transmit or receive data------------------------------
	if (Direction == I2C_Direction_Transmitter)
	{
		// Wait for TxE = 1-------------------------------------------------------
		while (!I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if (Direction == I2C_Direction_Receiver)
	{
		// Wait for RxNE = 1------------------------------------------------------
		while (!I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

/*
 * @func:  		I2C_TransmitData
 *
 * @brief:		The function transmits data of the sensor registers
 *
 * @param:		data
 *
 * @retval:		None
 *
 * @note:		None
 */
static void I2C_TransmitData (uint8_t data)
{
	// Transmit data--------------------------------------------------------------
	I2C_SendData(I2C_MASTER_INSTANCE, data);

	// Wait for the transmission to complete (wait for I2C_EV8_2)-----------------
	while (!I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}


/*
 * @func:  		I2C_Receive_ACK
 *
 * @brief:		The function receives data with ACK
 *
 * @param:		None
 *
 * @retval:		data_receive
 *
 * @note:		None
 */
static uint8_t I2C_Receive_ACK (void)
{
	uint8_t data_receive;

	// Use the ACK bit------------------------------------------------------------
	I2C_AcknowledgeConfig(I2C_MASTER_INSTANCE, ENABLE);

	// Wait for the data reception to complete------------------------------------
	while (!I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_BYTE_RECEIVED));

	data_receive = I2C_ReceiveData(I2C_MASTER_INSTANCE);

	return data_receive;
}

/*
 * @func:  		I2C_Receive_NACK
 *
 * @brief:		The function receives data with NACK
 *
 * @param:		None
 *
 * @retval:		data_receive
 *
 * @note:		None
 */
static uint8_t I2C_Receive_NACK (void)
{
	uint8_t data_receive;

	// Do not use the ACK bit-----------------------------------------------------
	I2C_AcknowledgeConfig(I2C_MASTER_INSTANCE, DISABLE);

	// Wait for the data reception to complete------------------------------------
	while (!I2C_CheckEvent(I2C_MASTER_INSTANCE, I2C_EVENT_MASTER_BYTE_RECEIVED));

	data_receive = I2C_ReceiveData(I2C_MASTER_INSTANCE);

	return data_receive;
}

/*
 * @func:  		I2C_Stop
 *
 * @brief:		The function sends the STOP bit
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void I2C_Stop (void)
{
	// Create a stop condition----------------------------------------------------
	I2C_GenerateSTOP(I2C_MASTER_INSTANCE, ENABLE);
}

/*
 * @func:  		TemHumSensor_readRegister
 *
 * @brief:		The function communicates with the sensor registers to read values
 *
 * @param[1]:	AddrSensor - Peripheral address (sensor)
 * @param[2]:	pAddrReg - Address of the register containing temperature and humidity data (CmdCode)
 * @param[3]:	pDataRead - Data read from the corresponding register (stored in an array)
 * @param[4]:	byDataLen - Data size
 * @param[5]:	rDelay - Delay time to read data
 *
 * @retval:		None
 *
 * @note:		None
 */
static void TemHumSensor_readRegister (uint8_t AddrSensor, uint8_t pAddrReg,
									   uint8_t *pDataRead, uint8_t byDataLen,
									   uint16_t rDelay)
{
	I2C_Start();
	I2C_Address_Direction(AddrSensor << 1, I2C_Direction_Transmitter);
	I2C_TransmitData(pAddrReg);

	// Delay to wait for data transmission to complete----------------------------
	if (rDelay > 0)
	{
		delay_ms(rDelay);
	}

	I2C_Stop();

	// Receive data from the Slave------------------------------------------------
	I2C_Start();
	I2C_Address_Direction(AddrSensor << 1, I2C_Direction_Receiver);

	for(uint8_t i = 0 ; i < byDataLen; i++)
	{
		// If it is the last byte, return NACK: signal to the Slave not to send more data
		if (i == byDataLen - 1)
		{
			pDataRead[i] = I2C_Receive_NACK();
		}
		else		// Return ACK: request the slave to continue sending data
		{
			pDataRead[i] = I2C_Receive_ACK();
		}
	}

	I2C_Stop();
}

/*
 * @func:  		TemHumSensor_getTemp
 *
 * @brief:		The function processes temperature data
 *
 * @param:		None
 *
 * @retval:		temperature
 *
 * @note:		None
 */
static double TemHumSensor_getTemp (void)
{
	double temperature = 0;
	uint8_t strTemp[2] = {0};	// Array storing values received from the corresponding register

	// Get the temperature value from the corresponding register------------------
	TemHumSensor_readRegister(SENSOR_ADDR, TEMP_CMDCODE, strTemp, 2, TIME_WAIT_GET_TEMP);

	// The Temp_Code value returned by the Si7020---------------------------------
	uint16_t tempCode = (uint16_t)(strTemp[0] << 8) | strTemp[1];

	// Convert the measured temperature value to °C-------------------------------
	temperature =  (double)((175.72 * tempCode) / 65536 - 46.85);

	return temperature;
}

/*
 * @func:  		TemHumSensor_getHumi
 *
 * @brief:		The function processes humidity data
 *
 * @param:		None
 *
 * @retval:		humidity
 *
 * @note:		None
 */
static double TemHumSensor_getHumi (void)
{
	double humidity = 0;
	uint8_t strHumi[2] = {0};

	// Get the humidity value from the corresponding register---------------------
	TemHumSensor_readRegister(SENSOR_ADDR, HUMI_CMDCODE, strHumi, 2, TIME_WAIT_GET_HUMI);

	// The RH_Code value returned by the Si7020-----------------------------------
	uint16_t RHCode = (uint16_t)((strHumi[0] << 8) | strHumi[1]);

	// Convert the measured humidity value to relative humidity percentage--------
	humidity = (double)((125 * RHCode) / 65536 - 6);

	return humidity;
}

/*
 * @func:  		Update_ValueSensor
 *
 * @brief:		The function updates temperature and humidity data according to the
 * 				requested time interval
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void Update_ValueSensor (void)
{
	static uint32_t TimeCurrent, TimeInit;
	static uint32_t TimeTotal;

	TimeCurrent = GetMilSecTick();

	if (TimeCurrent >= TimeInit)
	{
		TimeTotal += TimeCurrent - TimeInit;
	}
	else
	{
		TimeTotal += 0xFFFFFFFFU - TimeCurrent + TimeInit;
	}

	if (TimeTotal >= PERIOD_UPDATE_SENSOR)
	{
		// Time scan 1s-----------------------------------------------------------
		g_temp = TemHumSensor_getTemp();
		g_humi = TemHumSensor_getHumi();

		TimeTotal = 0;
	}

	TimeInit = TimeCurrent;
}

/*
 * @func:  		Update_LCD
 *
 * @brief:		The function updates data on the LCD screen according to the requested time interval
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void Update_LCD (void)
{
	static uint32_t TimeCurrent, TimeInit;
	static uint32_t TimeTotal;

	double currentTemp = TemHumSensor_getTemp();
	double currentHumi = TemHumSensor_getHumi();

	TimeCurrent = GetMilSecTick();

	if (TimeCurrent >= TimeInit)
	{
		TimeTotal += TimeCurrent - TimeInit;
	}
	else
	{
		TimeTotal += 0xFFFFFFFFU + TimeCurrent - TimeInit;
	}

	// Update the temperature value immediately when there is a significant change
	if (((currentTemp > g_temp) && (currentTemp - g_temp >= CHANGE_VALUE_TEMP)) ||
		((currentTemp < g_temp) && (g_temp - currentTemp >= CHANGE_VALUE_TEMP)))
	{
		g_temp = TemHumSensor_getTemp();
		printDataToLCD();
	}

	// Update the humidity value immediately when there is a significant change---
	if (((currentHumi > g_humi) && (currentHumi - g_humi >= CHANGE_VALUE_HUMI)) ||
		((currentHumi < g_humi) && (g_humi - currentHumi >= CHANGE_VALUE_HUMI)))
	{
		g_humi = TemHumSensor_getHumi();
		printDataToLCD();
	}

	// Update the temperature and humidity values on the LCD screen with a 5-second interval
	if (TimeTotal >= PERIOD_UPDATE_LCD)
	{
		g_temp = TemHumSensor_getTemp();
		g_humi = TemHumSensor_getHumi();
		printDataToLCD();

		TimeTotal = 0;
	}

	TimeInit = TimeCurrent;
}

/*
 * @func:  		printDataToLCD
 *
 * @brief:		The function prints data to the LCD screen
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
static void printDataToLCD (void)
{
	// Store the temperature and humidity values----------------------------------
	memset(g_strTemp, 0, sizeof(g_strTemp));
	memset(g_strHumi, 0, sizeof(g_strHumi));

	sprintf(g_strTemp, "Temp = %.2f oC     ", g_temp);
	sprintf(g_strHumi, "Humi = %.2f %%     ", g_humi);

	// Display on the LCD---------------------------------------------------------
	ucg_DrawString(&ucg, 17, 15, 0, "Assignment 2");
	ucg_DrawString(&ucg, 0, 50, 0, g_strTemp);
	ucg_DrawString(&ucg, 0, 75, 0, g_strHumi);
}

/*
 * @func:  		delay_ms
 *
 * @brief:		The function creates a delay
 *
 * @param:		milisecond - Delay time
 *
 * @retval:		None
 *
 * @note:		None
 */
static void delay_ms (uint32_t milisecond)
{
	for (uint32_t i = 0; i < milisecond; i++)
	{
		for (uint32_t j = 0; j < 5000; j++);
	}
}


/* END FILE */

