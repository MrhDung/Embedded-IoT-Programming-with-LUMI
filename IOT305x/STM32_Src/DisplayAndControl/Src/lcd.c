/*
 * lcd.c
 *
 *  Created on: Dec 2, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "lcd.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
ucg_t 			g_ucg;
uint8_t 		g_ledCount = 0;
uint8_t 		g_sensorCount = 0;

uint8_t			g_networkState = 0;
uint8_t 		g_tempValue = 0;
uint8_t 		g_humiValue = 0;
uint8_t 		g_detecState = 0;
uint8_t 		g_ledState = 0;
uint16_t 		g_lightValue = 0;

static char 	g_networkStr[20] = "";
static char 	g_strTemp[30] = "";
static char 	g_strHumi[30] = "";
static char 	g_strLight[30] = "";
static char		g_strDetec[30] = "";
static char 	g_strLed[30] = "";
static char 	g_strLedConStatus[30] = "";
static char 	g_strSensorConStatus[30] = "";

static char 	g_symbolStr[] = ">";
static char 	g_clearSymbolStr[] = "   ";
static char 	g_setStr[] = "--- Ok";
static char 	g_clearSetStr[] = "          ";

screenType_e 	g_currentScreen = SCREEN_HOME;
uint8_t 		g_currentSelect = SELECT_INDEX_1;

selectIndex_e	g_deviceSelected = SELECT_INDEX_1;
selectIndex_e	g_sensorItemSelected = SELECT_INDEX_1;

selectIndex_e	g_ledStateSelected = SELECT_INDEX_1;
selectIndex_e 	g_distanceValueSelected = SELECT_INDEX_1;
selectIndex_e 	g_latencyValueSelected = SELECT_INDEX_1;
selectIndex_e 	g_sensitivityValueSelected = SELECT_INDEX_1;
selectIndex_e 	g_lightValueSelected = SELECT_INDEX_1;

static deviceState_t g_deviceState =
{
    .ledState = DEVICE_NOT_EXISTS,
    .sensorState = DEVICE_NOT_EXISTS
};

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		LCD_Init
 *
 * @brief:		The function initializes the LCD
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_Init (void)
{
	Ucglib4WireSWSPI_begin(&g_ucg, UCG_FONT_MODE_SOLID);
	ucg_ClearScreen(&g_ucg);
	ucg_SetFont(&g_ucg, ucg_font_ncenR10_hf);
	ucg_SetColor(&g_ucg, 0, 255, 255, 255);
	ucg_SetColor(&g_ucg, 1, 0, 0, 0);
	ucg_SetRotate180(&g_ucg);
}

/*
 * @func:		LCD_ClearScreen
 *
 * @brief:		The function clears the screen
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ClearScreen (void)
{
	ucg_ClearScreen(&g_ucg);
	ucg_SetFont(&g_ucg, ucg_font_ncenR10_hf);
}

/*
 * @func:		LCD_ClearBuffer
 *
 * @brief:		The function clears the buffer
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ClearBuffer (void)
{
	memset(g_networkStr,0,  sizeof(g_networkStr));
	memset(g_strTemp,0, sizeof(g_strTemp));
	memset(g_strHumi,0,  sizeof(g_strHumi));
	memset(g_strLight,0,  sizeof(g_strLight));
	memset(g_strDetec, 0, sizeof(g_strDetec));
	memset(g_strLed, 0, sizeof(g_strLed));
	memset(g_strLedConStatus, 0, sizeof(g_strLedConStatus));
	memset(g_strSensorConStatus, 0, sizeof(g_strSensorConStatus));
}

/*
 * @func:		LCD_DisplayNetworkState
 *
 * @brief:		The function displays the network status
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayNetworkState (void)
{
	if (g_currentScreen == SCREEN_HOME)
	{
		memset(g_networkStr,0,  sizeof(g_networkStr));

		if (g_networkState == NETWORK_NO_NETWORK)
		{
			sprintf(g_networkStr,  "Nwk: No        ");
		}
		else if (g_networkState == NETWORK_CREATING)
		{
			sprintf(g_networkStr,  "Nwk: Yes       ");
		}

		ucg_DrawString(&g_ucg, 2, 36, 0, g_networkStr);
	}
}

/*
 * @func:		LCD_DisplayTempValue
 *
 * @brief:		The function displays the temperature value
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayTempValue (void)
{
	if (g_currentScreen == SCREEN_HOME)
	{
		// Store temperature values------------------------------------------------------------
		memset(g_strTemp, 0, sizeof(g_strTemp));

		sprintf(g_strTemp, "Temp: %d oC        ", g_tempValue);

		// Display on LCD----------------------------------------------------------------------
		ucg_DrawString(&g_ucg, 2, 53, 0, g_strTemp);
	}
}

/*
 * @func:		LCD_DisplayHumiValue
 *
 * @brief:		The function displays the humidity value
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayHumiValue (void)
{
	if (g_currentScreen == SCREEN_HOME)
	{
		// Store humidity values---------------------------------------------------------------
		memset(g_strHumi, 0, sizeof(g_strHumi));

		sprintf(g_strHumi, "Humi: %d %%        ", g_humiValue);

		// Display on LCD----------------------------------------------------------------------
		ucg_DrawString(&g_ucg, 2, 70, 0, g_strHumi);
	}
}

/*
 * @func:		LCD_DisplayLightValue
 *
 * @brief:		The function displays the light intensity value
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayLightValue (void)
{
	if (g_currentScreen == SCREEN_HOME)
	{
		// Store light intensity values--------------------------------------------------------
		memset(g_strLight, 0, sizeof(g_strLight));

		sprintf(g_strLight, "Light: %d Lux        ", g_lightValue);

		// Display on LCD----------------------------------------------------------------------
		ucg_DrawString(&g_ucg, 2, 87, 0, g_strLight);
	}
}

/*
 * @func:		LCD_DisplayDetectionState
 *
 * @brief:		The function displays the detected status
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayDetectionState (void)
{
	if (g_currentScreen == SCREEN_HOME)
	{
		memset(g_strDetec, 0, sizeof(g_strDetec));

		if (g_detecState)
		{
			sprintf(g_strDetec, "Motion: Yes     ");
		}
		else
		{
			sprintf(g_strDetec, "Motion: No      ");
		}

		ucg_DrawString(&g_ucg, 2, 104, 0, g_strDetec);
	}
}

/*
 * @func:		LCD_DisplayLedState
 *
 * @brief:		The function displays the LED status
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayLedState (void)
{
	if (g_currentScreen == SCREEN_HOME)
	{
		memset(g_strLed, 0, sizeof(g_strLed));

		if (g_ledState)
		{
			sprintf(g_strLed, "Led: On      ");
		}
		else
		{
			sprintf(g_strLed, "Led: Off     ");
		}

		ucg_DrawString(&g_ucg, 2, 121, 0, g_strLed);
	}
}

/*
 * @func:		LCD_DisplayConnectionLedState
 *
 * @brief:		The function displays the connection status of the LED
 *
 * @param:		connectionState
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayConnectionLedState (deviceStatus_e connectionState)
{
	if (connectionState == DEVICE_OFFLINE)
	{
		sprintf(g_strLedConStatus, "1. Led           Offl  ");
	}
	else if (connectionState == DEVICE_ONLINE)
	{
		sprintf(g_strLedConStatus, "1. Led           Onl   ");
		g_ledCount++;
	}
	else
	{
		sprintf(g_strLedConStatus, "1. Led           Not   ");
	}

	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, g_strLedConStatus);
}

/*
 * @func:		LCD_DisplayConnectionSensorState
 *
 * @brief:		The function displays the connection status of the SENSOR device
 *
 * @param:		connectionState
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_DisplayConnectionSensorState (deviceStatus_e connectionState)
{
	if (connectionState == DEVICE_OFFLINE)
	{
		sprintf(g_strSensorConStatus, "2. Sensor      Offl  ");
	}
	else if (connectionState == DEVICE_ONLINE)
	{
		sprintf(g_strSensorConStatus, "2. Sensor      Onl  ");
		g_sensorCount++;
	}
	else
	{
		sprintf(g_strSensorConStatus, "2. Sensor      Not  ");
	}

	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, g_strSensorConStatus);
}

/*
 * @func:		LCD_ScreenLoadOnStartup
 *
 * @brief:		The function displays information when the device starts up
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenLoadOnStartup (void)
{
	g_currentScreen = SCREEN_LOAD_STARTUP;

	ucg_DrawString(&g_ucg, 33, 40, 0, "IOT305x");
	ucg_DrawString(&g_ucg, 55, 65, 0, "by");
	ucg_DrawString(&g_ucg, 22, 90, 0, "Hoang Dung");
}

/*
 * @func:		LCD_ScreenHome
 *
 * @brief:		The function displays the HOME screen
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenHome (void)
{
	g_currentScreen = SCREEN_HOME;
//	uint8_t *frame = USART_GetFrame();

	LCD_ClearScreen();
	LCD_ClearBuffer();

	ucg_DrawString(&g_ucg, 42, 15, 0, "HOME");
	LCD_DisplayNetworkState();
	LCD_DisplayTempValue();
	LCD_DisplayHumiValue();
	LCD_DisplayLightValue();
	LCD_DisplayDetectionState();
	LCD_DisplayLedState();
}

/*
 * @func:		LCD_ScreenDeviceManager
 *
 * @brief:		The function displays the device management interface in the network
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenDeviceManager (void)
{
	g_currentScreen = SCREEN_DEVICE_MANAGER;
	g_currentSelect = g_deviceSelected;

	LCD_ClearScreen();
	LCD_ClearBuffer();

	ucg_DrawString(&g_ucg, 10, 15, 0, "Device Manager");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	LCD_DisplayConnectionLedState(g_deviceState.ledState);
	LCD_DisplayConnectionSensorState(g_deviceState.sensorState);
	LCD_ScreenSelectPosition(g_deviceSelected);
}



/*
 * @func:		LCD_ScreenLedControl
 *
 * @brief:		The function displays the LED control interface
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenLedControl (void)
{
	g_currentScreen = SCREEN_LED_CONTROL;
	g_currentSelect = g_ledStateSelected;

	LCD_ClearScreen();
	LCD_ClearBuffer();

	ucg_DrawString(&g_ucg, 26, 15, 0, "Led Control");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "1. OFF                      ");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "2. ON                      ");

	LCD_ScreenValueSetMarker(g_ledStateSelected);
}

/*
 * @func:		LCD_ScreenSensorConfig
 *
 * @brief:		The function displays the SENSOR configuration interface
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSensorConfig (void)
{
	g_currentScreen = SCREEN_SENSOR_CONFIG;
	g_currentSelect = g_sensorItemSelected;

	LCD_ClearScreen();
	LCD_ClearBuffer();

	ucg_DrawString(&g_ucg, 19, 15, 0, "Sensor Config");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "1. Distance");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "2. Latency");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "3. Sensitivity");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "4. Light");

	LCD_ScreenSelectPosition(g_sensorItemSelected);
}

/*
 * @func:		LCD_ScreenValueDistance
 *
 * @brief:		The function displays the interface for setting the detection distance value
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenValueDistance (void)
{
	g_currentScreen = SCREEN_VALUE_DISTANCE;
	g_currentSelect = g_distanceValueSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 14, 15, 0, "Distance Value");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "0.2m");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "0.4m");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "0.6m");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "0.8m");

	LCD_ScreenValueSetMarker(g_distanceValueSelected);
}

/*
 * @func:		LCD_ScreenValueLatency
 *
 * @brief:		The function displays the interface for setting the latency value
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenValueLatency (void)
{
	g_currentScreen = SCREEN_VALUE_LATENCY;
	g_currentSelect = g_latencyValueSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 16, 15, 0, "Latency Value");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "2s");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "4s");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "6s");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "8s");

	LCD_ScreenValueSetMarker(g_latencyValueSelected);
}

/*
 * @func:		LCD_ScreenValueSensitivity
 *
 * @brief:		The function displays the interface for setting the Ssnsitivity value
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenValueSensitivity (void)
{
	g_currentScreen = SCREEN_VALUE_SENSITIVITY;
	g_currentSelect = g_sensitivityValueSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 9, 15, 0, "Sensitivity Value");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "20");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "40");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "60");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "80");

	LCD_ScreenValueSetMarker(g_sensitivityValueSelected);
}

/*
 * @func:		LCD_ScreenValueLight
 *
 * @brief:		The function displays the interface for setting the light intensity value
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenValueLight (void)
{
	g_currentScreen = SCREEN_VALUE_LIGHT;
	g_currentSelect = g_lightValueSelected;

	LCD_ClearScreen();
	ucg_DrawString(&g_ucg, 25, 15, 0, "Light Value");
	ucg_DrawString(&g_ucg, COLUMN_1, ROW_0, 0, "-------------------------");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_1, 0, "10 Lux");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_2, 0, "600 Lux");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_3, 0, "1200 Lux");
	ucg_DrawString(&g_ucg, COLUMN_2, ROW_4, 0, "2000 Lux");

	LCD_ScreenValueSetMarker(g_lightValueSelected);
}

/*
 * @func:		LCD_ScreenMoveUp
 *
 * @brief:		The function moves the selection position up according to each corresponding screen
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenMoveUp (void)
{
	switch (g_currentScreen)
	{
		case SCREEN_HOME:
			return;

		case SCREEN_DEVICE_MANAGER:
		case SCREEN_LED_CONTROL:
		{
			LCD_ScreenMovePosition(2, MOVE_UP);
			return;
		}

		case SCREEN_SENSOR_CONFIG:
		case SCREEN_VALUE_DISTANCE:
		case SCREEN_VALUE_LATENCY:
		case SCREEN_VALUE_SENSITIVITY:
		case SCREEN_VALUE_LIGHT:
		{
			LCD_ScreenMovePosition(4, MOVE_UP);
			return;
		}

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenMoveDown
 *
 * @brief:		The function moves the selection position down according to each corresponding screen
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenMoveDown (void)
{
	switch (g_currentScreen)
	{
		case SCREEN_HOME:
			return;

		case SCREEN_DEVICE_MANAGER:
		case SCREEN_LED_CONTROL:
		{
			LCD_ScreenMovePosition(2, MOVE_DOWN);
			return;
		}

		case SCREEN_SENSOR_CONFIG:
		case SCREEN_VALUE_DISTANCE:
		case SCREEN_VALUE_LATENCY:
		case SCREEN_VALUE_SENSITIVITY:
		case SCREEN_VALUE_LIGHT:
		{
			LCD_ScreenMovePosition(4, MOVE_DOWN);
			return;
		}

		default:
			break;
	}

}

/*
 * @func:		LCD_ScreenMovePosition
 *
 * @brief:		The function updates the selection position on the screen based on the movement direction
 *
 * @param[1]:	maxIndex
 * @param[2]:	moveDirection
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenMovePosition (uint8_t maxIndex, uint8_t moveDirection)
{
    // Clear the selection at the previous position
	LCD_ScreenClearSelectPosition(g_currentSelect);

	if (moveDirection == MOVE_UP)
	{
	    // When on the first value, pressing up will move to the last value
	    if (g_currentSelect == SELECT_INDEX_1)
	    {
	    	g_currentSelect = maxIndex;
	    }
	    else
	    {
	    	g_currentSelect--;
	    }
	}
	else if (moveDirection == MOVE_DOWN)
	{
	    // When on the last value, pressing down will move to the first value
	    if (g_currentSelect == maxIndex)
	    {
	    	g_currentSelect = SELECT_INDEX_1;
	    }
	    else
	    {
	    	g_currentSelect++;
	    }
	}

    // Place the selection mark at the new position
    LCD_ScreenSelectPosition(g_currentSelect);
}

/*
 * @func:		LCD_ScreenNextAndSet
 *
 * @brief:		The function handles selection and value setting operations on the interface screen
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		Navigate to a submenu or send the selected value via USART depending on the current screen
 */
void LCD_ScreenNextAndSet (void)
{
	switch (g_currentScreen)
	{
		case SCREEN_HOME:
			return;

		case SCREEN_DEVICE_MANAGER:
		{
			switch (g_currentSelect)
			{

				case MENU_DEVICE_LED:
				{
					LCD_ScreenLedControl();
					return;
				}

				case MENU_DEVICE_SENSOR:
				{
					g_sensorItemSelected = SELECT_INDEX_1;
					LCD_ScreenSensorConfig();
					return;
				}

				default:
					break;
			}

			break;
		}

		case SCREEN_LED_CONTROL:
		{
			if (g_ledCount > 0)
			{
				LCD_ScreenSetLedState(g_ledStateSelected);

				if (g_ledStateSelected == SELECT_INDEX_1)
				{
					USART_SendLedState(LED_OFF);
				}
				else if (g_ledStateSelected == SELECT_INDEX_2)
				{
					USART_SendLedState(LED_ON);
				}
			}

			return;
		}

		case SCREEN_SENSOR_CONFIG:
		{
			switch (g_currentSelect)
			{
				case MENU_SENSOR_DISTANCE:
				{
					LCD_ScreenValueDistance();
					return;
				}

				case MENU_SENSOR_LATENCY:
				{
					LCD_ScreenValueLatency();
					return;
				}

				case MENU_SENSOR_SENSITIVITY:
				{
					LCD_ScreenValueSensitivity();
					return;
				}

				case MENU_SENSOR_LIGHT:
				{
					LCD_ScreenValueLight();
					return;
				}

				default:
					break;
			}
		}

		case SCREEN_VALUE_DISTANCE:
		{
			if (g_sensorCount > 0)
			{
				LCD_ScreenSetValueDistance(g_distanceValueSelected);

				switch (g_distanceValueSelected)
				{
					case SELECT_INDEX_1:
					{
						USART_SendValueDistance(VALUE_DISTANCE_1);
						return;
					}

					case SELECT_INDEX_2:
					{
						USART_SendValueDistance(VALUE_DISTANCE_2);
						return;
					}

					case SELECT_INDEX_3:
					{
						USART_SendValueDistance(VALUE_DISTANCE_3);
						return;
					}

					case SELECT_INDEX_4:
					{
						USART_SendValueDistance(VALUE_DISTANCE_4);
						return;
					}

					default:
						break;
				}
			}

			return;
		}

		case SCREEN_VALUE_LATENCY:
		{
			if (g_sensorCount > 0)
			{
				LCD_ScreenSetValueLatency(g_latencyValueSelected);

				switch (g_latencyValueSelected)
				{
					case SELECT_INDEX_1:
					{
						USART_SendValueLatency(VALUE_LATENCY_1);
						return;
					}

					case SELECT_INDEX_2:
					{
						USART_SendValueLatency(VALUE_LATENCY_2);
						return;
					}

					case SELECT_INDEX_3:
					{
						USART_SendValueLatency(VALUE_LATENCY_3);
						return;
					}

					case SELECT_INDEX_4:
					{
						USART_SendValueLatency(VALUE_LATENCY_4);
						return;
					}

					default:
						break;
				}
			}

			return;
		}

		case SCREEN_VALUE_SENSITIVITY:
		{
			if (g_sensorCount > 0)
			{
				LCD_ScreenSetValueSensitivity(g_sensitivityValueSelected);

				switch (g_sensitivityValueSelected)
				{
					case SELECT_INDEX_1:
					{
						USART_SendValueSensitivity(VALUE_SENSITIVITY_1);
						return;
					}

					case SELECT_INDEX_2:
					{
						USART_SendValueSensitivity(VALUE_SENSITIVITY_2);
						return;
					}

					case SELECT_INDEX_3:
					{
						USART_SendValueSensitivity(VALUE_SENSITIVITY_3);
						return;
					}

					case SELECT_INDEX_4:
					{
						USART_SendValueSensitivity(VALUE_SENSITIVITY_4);
						return;
					}

					default:
						break;
				}
			}

			return;
		}

		case SCREEN_VALUE_LIGHT:
		{
			if (g_sensorCount > 0)
			{
				LCD_ScreenSetValueLight(g_lightValueSelected);

				switch (g_lightValueSelected)
				{
					case SELECT_INDEX_1:
					{
						USART_SendValueLight(VALUE_LIGHT_1);
						return;
					}

					case SELECT_INDEX_2:
					{
						USART_SendValueLight(VALUE_LIGHT_2);
						return;
					}

					case SELECT_INDEX_3:
					{
						USART_SendValueLight(VALUE_LIGHT_3);
						return;
					}

					case SELECT_INDEX_4:
					{
						USART_SendValueLight(VALUE_LIGHT_4);
						return;
					}

					default:
						break;
				}
			}

			return;
		}

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenBack
 *
 * @brief:		The function returns to the previous screen
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenBack (void)
{
	switch (g_currentScreen)
	{
		case SCREEN_HOME:
			return;

		case SCREEN_DEVICE_MANAGER:
			return;

		case SCREEN_LED_CONTROL:
		{
			g_deviceSelected = SELECT_INDEX_1;
			LCD_ScreenDeviceManager();
			return;
		}

		case SCREEN_SENSOR_CONFIG:
		{
			g_deviceSelected = SELECT_INDEX_2;
			LCD_ScreenDeviceManager();
			return;
		}

		case SCREEN_VALUE_DISTANCE:
		{
			g_sensorItemSelected = SELECT_INDEX_1;
			LCD_ScreenSensorConfig();
			return;
		}

		case SCREEN_VALUE_LATENCY:
		{
			g_sensorItemSelected = SELECT_INDEX_2;
			LCD_ScreenSensorConfig();
			return;
		}

		case SCREEN_VALUE_SENSITIVITY:
		{
			g_sensorItemSelected = SELECT_INDEX_3;
			LCD_ScreenSensorConfig();
			return;
		}

		case SCREEN_VALUE_LIGHT:
		{
			g_sensorItemSelected = SELECT_INDEX_4;
			LCD_ScreenSensorConfig();
			return;
		}

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenSetLedState
 *
 * @brief:		Set the LED status (ON/OFF)
 *
 * @param:		index - Currently selected position
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetLedState (selectIndex_e index)
{
	// Clear the confirmation mark at the previous position
	LCD_ScreenClearSetValue(index);

	// Store the set value
	g_ledStateSelected = g_currentSelect;

	// Display the confirmation mark at the new position
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_ScreenSetValueDistance
 *
 * @brief:		The function sets the detection distance value
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetValueDistance (selectIndex_e index)
{
	// Clear the confirmation mark at the previous position
	LCD_ScreenClearSetValue(index);

	// Store the set value
	g_distanceValueSelected = g_currentSelect;

	// Display the confirmation mark at the new position
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_ScreenSetValueLatency
 *
 * @brief:		The function sets the detection latency value
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetValueLatency (selectIndex_e index)
{
	// Clear the confirmation mark at the previous position
	LCD_ScreenClearSetValue(index);

	// Store the set value
	g_latencyValueSelected = g_currentSelect;

	// Display the confirmation mark at the new position
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_ScreenSetValueSensitivity
 *
 * @brief:		HThe function sets the detection sensitivity value
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetValueSensitivity (selectIndex_e index)
{
	// Clear the confirmation mark at the previous position
	LCD_ScreenClearSetValue(index);

	// Store the set value
	g_sensitivityValueSelected = g_currentSelect;

	// Display the confirmation mark at the new position
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_ScreenSetValueLight
 *
 * @brief:		The function sets the light intensity value
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetValueLight (selectIndex_e index)
{
	// Clear the confirmation mark at the previous position
	LCD_ScreenClearSetValue(index);

	// Store the set value
	g_lightValueSelected = g_currentSelect;

	// Display the confirmation mark at the new position
	LCD_ScreenSetValue(g_currentSelect);
}

/*
 * @func:		LCD_ScreenValueSetMarker
 *
 * @brief:		The function displays the previously set value when re-entering the corresponding settings screen
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenValueSetMarker (selectIndex_e index)
{
	switch (index)
	{
		case SELECT_INDEX_1:
		{
	        LCD_ScreenSelectPosition(SELECT_INDEX_1);
	        LCD_ScreenSetValue(SELECT_INDEX_1);
	        break;
		}

		case SELECT_INDEX_2:
		{
	        LCD_ScreenSelectPosition(SELECT_INDEX_2);
	        LCD_ScreenSetValue(SELECT_INDEX_2);
	        break;
		}

		case SELECT_INDEX_3:
		{
	        LCD_ScreenSelectPosition(SELECT_INDEX_3);
	        LCD_ScreenSetValue(SELECT_INDEX_3);
	        break;
		}

		case SELECT_INDEX_4:
		{
	        LCD_ScreenSelectPosition(SELECT_INDEX_4);
	        LCD_ScreenSetValue(SELECT_INDEX_4);
	        break;
		}

		default:
			break;
	}
}


/*
 * @func:		LCD_ScreenSelectPosition
 *
 * @brief:		The function displays a marking symbol at the selected position
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSelectPosition (selectIndex_e index)
{
	switch (index)
	{
		case SELECT_INDEX_1:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_1, 0, g_symbolStr);
			break;

		case SELECT_INDEX_2:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_2, 0, g_symbolStr);
			break;

		case SELECT_INDEX_3:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_3, 0, g_symbolStr);
			break;

		case SELECT_INDEX_4:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_4, 0, g_symbolStr);
			break;

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenClearSelectPosition
 *
 * @brief:		The function clears the marking symbol at the selected position
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenClearSelectPosition (selectIndex_e index)
{
	switch (index)
	{
		case SELECT_INDEX_1:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_1, 0, g_clearSymbolStr);
			break;

		case SELECT_INDEX_2:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_2, 0, g_clearSymbolStr);
			break;

		case SELECT_INDEX_3:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_3, 0, g_clearSymbolStr);
			break;

		case SELECT_INDEX_4:
			ucg_DrawString(&g_ucg, COLUMN_1, ROW_4, 0, g_clearSymbolStr);
			break;

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenSetValue
 *
 * @brief:		The function displays the confirmation status (OK) at the selected position
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenSetValue (selectIndex_e index)
{
	switch (index)
	{
		case SELECT_INDEX_1:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_1, 0, g_setStr);
			break;

		case SELECT_INDEX_2:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_2, 0, g_setStr);
			break;

		case SELECT_INDEX_3:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_3, 0, g_setStr);
			break;

		case SELECT_INDEX_4:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_4, 0, g_setStr);
			break;

		default:
			break;
	}
}

/*
 * @func:		LCD_ScreenClearSetValue
 *
 * @brief:		The function clears the confirmation status (OK) from the selected position
 *
 * @param:		index
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_ScreenClearSetValue (selectIndex_e index)
{
	switch (index)
	{
		case SELECT_INDEX_1:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_1, 0, g_clearSetStr);
			break;

		case SELECT_INDEX_2:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_2, 0, g_clearSetStr);
			break;

		case SELECT_INDEX_3:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_3, 0, g_clearSetStr);
			break;

		case SELECT_INDEX_4:
			ucg_DrawString(&g_ucg, COLUMN_3, ROW_4, 0, g_clearSetStr);
			break;

		default:
			break;
	}
}

/*
 * @func:		LCD_UpdateDeviceState
 *
 * @brief:		The function updates the device status
 *
 * @param[1]:	modelID
 * @param[2]:	state
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_UpdateDeviceState (const char *modelID, deviceStatus_e state)
{
    if (strncmp(modelID, "LED", 3) == 0)
    {
    	g_ledCount = 0;
    	g_deviceState.ledState = state;

    	if (g_currentScreen == SCREEN_DEVICE_MANAGER)
    	{
        	LCD_DisplayConnectionLedState(state);
    	}
    }
    else if (strncmp(modelID, "SENSOR", 6) == 0)
    {
    	g_sensorCount = 0;
    	g_deviceState.sensorState = state;

    	if (g_currentScreen == SCREEN_DEVICE_MANAGER)
    	{
        	LCD_DisplayConnectionSensorState(state);
    	}
    }
}

/*
 * @func:		LCD_GetCurrentScreen
 *
 * @brief:		The function retrieves the current screen
 *
 * @param:		None
 *
 * @retval:		g_currentScreen
 *
 * @note:		None
 */
screenType_e LCD_GetCurrentScreen (void)
{
	return g_currentScreen;
}

/*
 * @func:		LCD_GetCurrentSelect
 *
 * @brief:		Retrieve the currently selected item's value
 *
 * @param:		None
 *
 * @retval:		g_currentSelect
 *
 * @note:		None
 */
uint8_t LCD_GetCurrentSelect (void)
{
	return g_currentSelect;
}

/*
 * @func:		LCD_GetLedStateSelected
 *
 * @brief:		The function retrieves the selected LED status
 *
 * @param:		None
 *
 * @retval:		g_ledStateSelected
 *
 * @note:		None
 */
selectIndex_e LCD_GetLedStateSelected (void)
{
	return g_ledStateSelected;
}

/*
 * @func:		LCD_GetDistanceValueSelected
 *
 * @brief:		The function retrieves the selected detection distance value
 *
 * @param:		None
 *
 * @retval:		g_distanceValueSelected
 *
 * @note:		None
 */
selectIndex_e LCD_GetDistanceValueSelected (void)
{
	return g_distanceValueSelected;
}

/*
 * @func:		LCD_GetLatencyValueSelected
 *
 * @brief:		The function retrieves the selected latency value
 *
 * @param:		None
 *
 * @retval:		g_latencyValueSelected
 *
 * @note:		None
 */
selectIndex_e LCD_GetLatencyValueSelected (void)
{
	return g_latencyValueSelected;
}

/*
 * @func:		LCD_GetSensitivityValueSelected
 *
 * @brief:		The function retrieves the selected sensitivity value
 *
 * @param:		None
 *
 * @retval:		g_sensitivityValueSelected
 *
 * @note:		None
 */
selectIndex_e LCD_GetSensitivityValueSelected (void)
{
	return g_sensitivityValueSelected;
}

/*
 * @func:		LCD_GetLightValueSelected
 *
 * @brief:		The function retrieves the selected light intensity value
 *
 * @param:		None
 *
 * @retval:		g_lightValueSelected
 *
 * @note:		None
 */
selectIndex_e LCD_GetLightValueSelected (void)
{
	return g_lightValueSelected;
}

/*
 * @func:		LCD_GetNetworkStateReceived
 *
 * @brief:		The function retrieves the received network status
 *
 * @param:		state
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_GetNetworkStateReceived (uint8_t state)
{
	g_networkState = state;
}

/*
 * @func:		LCD_GetTempValueReceived
 *
 * @brief:		The function retrieves the received temperature value
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_GetTempValueReceived (uint8_t value)
{
	g_tempValue = value;
}

/*
 * @func:		LCD_GetHumiValueReceived
 *
 * @brief:		The function retrieves the received humidity value
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_GetHumiValueReceived (uint8_t value)
{
	g_humiValue = value;
}

/*
 * @func:		LCD_GetLightValueReceived
 *
 * @brief:		The function retrieves the received light intensity value
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_GetLightValueReceived (uint16_t value)
{
	g_lightValue = value;
}

/*
 * @func:		LCD_GetDetecStateReceived
 *
 * @brief:		The function retrieves the received detection status
 *
 * @param:		value
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_GetDetecStateReceived (uint8_t value)
{
	g_detecState = value;
}

/*
 * @func:		LCD_GetLedStateReceived
 *
 * @brief:		The function retrieves the received LED status
 *
 * @param:		state
 *
 * @retval:		None
 *
 * @note:		None
 */
void LCD_GetLedStateReceived (uint8_t state)
{
	g_ledState = state;
}

/*
 * @func:		LCD_ResetItemSelect
 *
 * @brief:		The function reinitializes the selected value
 *
 * @param:		None
 *
 * @retval:		g_deviceSelected
 * 				g_sensorItemSelected
 *
 * @note:		None
 */
uint8_t LCD_ResetItemSelect (void)
{
	return g_deviceSelected = SELECT_INDEX_1, g_sensorItemSelected = SELECT_INDEX_1;
}

/* END FILE */
