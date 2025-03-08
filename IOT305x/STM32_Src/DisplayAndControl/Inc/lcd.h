/*
 * lcd.h
 *
 *  Created on: Dec 2, 2024
 *      Author: Mr.hDung
 */

#ifndef LCD_H_
#define LCD_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "ucg.h"
#include "Ucglib.h"
#include "usart.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define COLUMN_1				1
#define COLUMN_2				16
#define COLUMN_3				87

#define ROW_0					32
#define ROW_1					51
#define ROW_2					73
#define ROW_3					95
#define ROW_4					117

#define MOVE_DOWN				0
#define MOVE_UP					1

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	SCREEN_LOAD_STARTUP,
    SCREEN_HOME,
	SCREEN_DEVICE_MANAGER,
	SCREEN_LED_CONTROL,
    SCREEN_SENSOR_CONFIG,
    SCREEN_VALUE_DISTANCE,
	SCREEN_VALUE_LATENCY,
	SCREEN_VALUE_SENSITIVITY,
	SCREEN_VALUE_LIGHT
} screenType_e;

typedef enum
{
	MENU_DEVICE_LED = 1,
	MENU_DEVICE_SENSOR
} menuDevice_e;

typedef enum
{
	MENU_SENSOR_DISTANCE = 1,
	MENU_SENSOR_LATENCY,
	MENU_SENSOR_SENSITIVITY,
	MENU_SENSOR_LIGHT
} menuSensor_e;

typedef enum
{
	SELECT_INDEX_1 = 1,
	SELECT_INDEX_2,
	SELECT_INDEX_3,
	SELECT_INDEX_4
} selectIndex_e;

typedef enum
{
    DEVICE_OFFLINE,
    DEVICE_ONLINE,
    DEVICE_NOT_EXISTS
} deviceStatus_e;

// Device status storage structure
typedef struct
{
	deviceStatus_e ledState;
	deviceStatus_e sensorState;
} deviceState_t;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void LCD_Init (void);
void LCD_ClearScreen (void);
void LCD_ClearBuffer (void);
void LCD_DisplayNetworkState (void);
void LCD_DisplayTempValue (void);
void LCD_DisplayHumiValue (void);
void LCD_DisplayLightValue (void);
void LCD_DisplayDetectionState (void);
void LCD_DisplayLedState (void);
void LCD_DisplayConnectionLedState (deviceStatus_e connectionState);
void LCD_DisplayConnectionSensorState (deviceStatus_e connectionState);
void LCD_ScreenLoadOnStartup (void);
void LCD_ScreenHome (void);
void LCD_ScreenDeviceManager (void);
void LCD_ScreenLedControl (void);
void LCD_ScreenSensorConfig (void);
void LCD_ScreenValueDistance (void);
void LCD_ScreenValueLatency (void);
void LCD_ScreenValueSensitivity (void);
void LCD_ScreenValueLight (void);
void LCD_ScreenMoveUp (void);
void LCD_ScreenMoveDown (void);
void LCD_ScreenMovePosition (uint8_t maxIndex, uint8_t moveDirection);
void LCD_ScreenNextAndSet (void);
void LCD_ScreenBack (void);
void LCD_ScreenSetLedState (selectIndex_e index);
void LCD_ScreenSetValueDistance (selectIndex_e index);
void LCD_ScreenSetValueLatency (selectIndex_e index);
void LCD_ScreenSetValueSensitivity (selectIndex_e index);
void LCD_ScreenSetValueLight (selectIndex_e index);
void LCD_ScreenValueSetMarker (selectIndex_e index);
void LCD_ScreenSelectPosition (selectIndex_e index);
void LCD_ScreenClearSelectPosition (selectIndex_e index);
void LCD_ScreenSetValue (selectIndex_e index);
void LCD_ScreenClearSetValue (selectIndex_e index);
screenType_e LCD_GetCurrentScreen (void);
void LCD_UpdateDeviceState (const char *modelID, deviceStatus_e state);
uint8_t LCD_GetCurrentSelect (void);
selectIndex_e LCD_GetLedStateSelected (void);
selectIndex_e LCD_GetDistanceValueSelected (void);
selectIndex_e LCD_GetLatencyValueSelected (void);
selectIndex_e LCD_GetSensitivityValueSelected (void);
selectIndex_e LCD_GetLightValueSelected (void);
void LCD_GetNetworkStateReceived (uint8_t state);
void LCD_GetTempValueReceived (uint8_t value);
void LCD_GetHumiValueReceived (uint8_t value);
void LCD_GetLightValueReceived (uint16_t value);
void LCD_GetDetecStateReceived (uint8_t value);
void LCD_GetLedStateReceived (uint8_t state);
uint8_t LCD_ResetItemSelect (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* LCD_H_ */

/* END FILE */
