/*
 * main-coor.c
 *
 *  Created on: Dec 26, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Main/main-coor.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl mainStateEventControl;
EmberEventControl updateConnectStatusEventControl;

MainState_e g_systemState;

const char g_modelIdArray[10][10] =
{
	"LED",
	"SENSOR"
};

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup.
 * Any code that you would normally put into the top of the application's
 * main() routine should be put into this function.
        Note: No callback
 * in the Application Framework is associated with resource cleanup. If you
 * are implementing your application on a Unix host where resource cleanup is
 * a consideration, we expect that you will use the standard Posix system
 * calls, including the use of atexit() and handlers for signals such as
 * SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal()
 * function to register your signal handler, please mind the returned value
 * which may be an Application Framework function. If the return value is
 * non-null, please make sure that you call the returned function from your
 * handler to avoid negating the resource cleanup of the Application Framework
 * itself.
 *
 */
void emberAfMainInitCallback (void)
{
	emberAfCorePrintln("emberAfMainInitCallback");

	NETWORK_Init(Main_NetworkHandle);
	RECEIVE_Init(Main_ReceiveHandle);
	Button_Init(Main_ButtonPressHandle, Main_ButtonHoldHandle);
	led_Init();
	Timer_Init(10);
	USART2_Init(Main_Usart2RxHandle);

	g_systemState = POWER_ON_STATE;

	emberEventControlSetActive(mainStateEventControl);
}

/*
 * @func:		mainStateEventHandler
 *
 * @brief:		The function handles the program states
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void mainStateEventHandler (void)
{
	emberAfCorePrintln("mainStateEventHandler");
	emberEventControlSetInactive(mainStateEventControl);

	EmberNetworkStatus nwkCurrentStatus;

	switch(g_systemState)
	{
		case POWER_ON_STATE:
		{
			g_systemState = IDLE_STATE;
			nwkCurrentStatus = emberAfNetworkState();	// Check the network status

			if(nwkCurrentStatus == EMBER_NO_NETWORK)
			{
				toggleLed(LED_2, RED, 3, 300, 300);
				USART_SendNetworkState(NETWORK_NO_NETWORK);
			}
			else if (nwkCurrentStatus == EMBER_JOINED_NETWORK)
			{
				toggleLed(LED_2, PINK, 3, 300, 300);
				USART_SendNetworkState(NETWORK_CREATING);
			}

			emberEventControlSetActive(updateConnectStatusEventControl);
			break;
		}

		case IDLE_STATE:
			break;

		case REBOOT_STATE:
		{
			g_systemState = IDLE_STATE;
			halReboot();
			break;
		}

		default:
			break;
	}
}

/*
 * @func:		Main_ButtonPressHandle
 *
 * @brief:		The function handles events when a button is pressed
 *
 * @params[1]:	button - Button to be used
 * @params[2]:	pressCount - Number of button presses
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Main_ButtonPressHandle (uint8_t button, uint8_t pressCount)
{
	if(button == SW1)
	{
		switch(pressCount)
		{
			case press_1:
			{
				NETWORK_CreateNetwork();
				break;
			}

			case press_2:
			{
				NETWORK_OpenNetwork();
				break;
			}

			case press_3:
			{
				NETWORK_CloseNetwork();
				break;
			}

			case press_5:
			{
				NETWORK_DeleteNetwork();
				break;
			}

			default:
				break;
		}
	}
	else if (button == SW2)
	{
		switch(pressCount)
		{
			case press_1:
			{
				DeviceManager_PrintDeviceList();
				emberAfCorePrintln("SW2 press_1");
				break;
			}

			default:
				break;
		}
	}
}

/*
 * @func:		Main_ButtonHoldHandle
 *
 * @brief:		The function handles events when a button is held down
 *
 * @params[1]:	button - Button to be used
 * @params[2]:	holdCount - Button hold time
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Main_ButtonHoldHandle (uint8_t button, uint8_t holdCount)
{
	if(button == SW1)
	{
		switch(holdCount)
		{
			case hold_1s:
				break;

			default:
				break;
		}
	}
	else if (button == SW2)
	{
		switch(holdCount)
		{
			case hold_1s:
				break;

			default:
				break;
		}
	}
}

/*
 * @func:		Main_NetworkHandle
 *
 * @brief:		The function handles events at the Network layer
 *
 * @params:		networkResult - Network states
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Main_NetworkHandle (networkState_e networkResult)
{
	emberAfCorePrintln("Main_NetworkHandle");

	switch(networkResult)
	{
		case NETWORK_CREATING:
		{
			emberAfCorePrintln("NETWORK_CREATING");
			toggleLed(LED_2, PINK, 3, 300, 300);
			USART_SendNetworkState(NETWORK_CREATING);
			break;
		}

		case NETWORK_OPENING:
		{
			emberAfCorePrintln("NETWORK_OPENING");
			toggleLed(LED_2, GREEN, 3, 300, 300);
			break;
		}

		case NETWORK_CLOSING:
		{
			emberAfCorePrintln("NETWORK_CLOSING");
			toggleLed(LED_2, YELLOW, 3, 300, 300);
			break;
		}

		case NETWORK_NO_NETWORK:
		{
			emberAfCorePrintln("NETWORK_NO_NETWORK");
			toggleLed(LED_2, RED, 3, 300, 300);
			USART_SendNetworkState(NETWORK_NO_NETWORK);
			g_systemState = REBOOT_STATE;
			break;
		}

		default:
			break;
	}
}

/*
 * @func:  		Main_ReceiveHandle
 *
 * @brief:		The function processes ZCL messages sent from the device
 *
 * @param[1]:	nodeId
 * @param[2]:	receiveId
 * @param[3]:	payload
 * @param[4]:	length
 *
 * @retval:		None
 *
 * @note:		None
 */
void Main_ReceiveHandle (EmberNodeId nodeId,			\
						 RECEIVE_CMD_ID_e receiveId,	\
						 uint8_t *payload, 				\
						 uint8_t length)
{
	bool checkTime	= false;

	switch (receiveId)
	{
		case DEVICE_JOIN_NETWORK:
		{
			checkTime = true;

			DeviceManager_AddDeviceToList(nodeId, (char*)payload);
			USART_SendDeviceJoinNetwork(nodeId, payload, length);
			break;
		}

		case DEVICE_LEAVE_NETWORK:
		{
			char *modelID = DeviceManager_GetModelID(nodeId);

			emberAfCorePrintln("Set device %s to leave network", modelID);

			USART_SendDeviceLeaveNetwork(nodeId, (uint8_t*)modelID, strlen(modelID));
			DeviceManager_RemoveDeviceFromList(nodeId);
			break;
		}

		case DEVICE_CONNECTED:
		{
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, payload, length);
			break;
		}

		case DEVICE_DISCONNECTED:
		{
			char *modelID = DeviceManager_GetModelID(nodeId);
			checkTime = true;

			emberAfCorePrintln("Set device %s to offline", modelID);

			DeviceManager_SetDeviceOffline(nodeId);
			USART_SendDeviceDisconnected(nodeId, (uint8_t*)modelID, strlen(modelID));
			break;
		}

		case DEVICE_SENSOR_TEMP_VALUE:
		{
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, payload, length);
			USART_SendTempValue(*payload);
			break;
		}

		case DEVICE_SENSOR_HUMI_VALUE:
		{
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, payload, length);
			USART_SendHumiValue(*payload);
			break;
		}

		case DEVICE_SENSOR_LIGHT_VALUE:
		{
			uint16_t value = ((*payload) << 8) | *(payload + 1);
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, payload, length);
			USART_SendLightValue(value);
			break;
		}

		case DEVICE_SENSOR_LD2410_MOTION:
		{
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, payload, length);
			USART_SendDetectionState(*payload);
			break;
		}

		case DEVICE_SENSOR_LD2410_UNMOTION:
		{
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, payload, length);
			USART_SendDetectionState(*payload);
			break;
		}

		case DEVICE_LED_ON:
		{
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, payload, length);

			DeviceManager_SetStateLedOn(nodeId);
			USART_SendLedState(*payload);
			break;
		}

		case DEVICE_LED_OFF:
		{
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, payload, length);

			DeviceManager_SetStateLedOff(nodeId);
			USART_SendLedState(*payload);
			break;
		}

		case DEVICE_KEEPALIVE:
		{
			char* modelId = DeviceManager_GetModelID(nodeId);
			checkTime = true;

			DeviceManager_SetDeviceOnline(nodeId);
			USART_SendDeviceConnected(nodeId, (uint8_t*)modelId, (uint8_t)strlen(modelId));
			break;
		}

		default:
			break;
	}

	// Save last communicate time
	if(checkTime)
	{
		uint32_t time = Timer_GetMilSecTick();
		DeviceManager_UpdateLastTime(nodeId, time);
	}
}

/*
 * @func:  		Main_Usart2RxHandle
 *
 * @brief:		The function executes the event upon receiving the corresponding message
 * 				in the specified format
 *
 * @param:		UsartStateRx - Received status
 *
 * @retval:		None
 *
 * @note:		None
 */
void Main_Usart2RxHandle (USART_STATE_e UsartStateRx)
{
	if (UsartStateRx != USART_STATE_IDLE)
	{
		switch (UsartStateRx)
		{
			case USART_STATE_EMPTY:
			{
				emberAfCorePrintln("USART_STATE_EMPTY\n");
				break;
			}

			case USART_STATE_DATA_RECEIVED:
			{
				emberAfCorePrintln("USART_STATE_DATA_RECEIVED\n");
				USART_ReceivedData();
				break;
			}

			case USART_STATE_DATA_ERROR:
			{
				emberAfCorePrintln("USART_STATE_DATA_ERROR\n");
				break;
			}

			case USART_STATE_ERROR:
			case USART_STATE_RX_TIMEOUT:
			{
				emberAfCorePrintln("USART_STATE_ERROR_OR_RX_TIMEOUT\n");
				break;
			}

			default:
				break;
		}
	}
}

/*
 * @func:  		updateConnectStatusEventHandler
 *
 * @brief:		The function checks the time to update the device's connection status
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void updateConnectStatusEventHandler (void)
{
	emberEventControlSetInactive(updateConnectStatusEventControl);

	deviceInfo_t *deviceList = DeviceManager_GetDeviceList();
	uint8_t length = DeviceManager_GetDeviceCountToList();

	for (uint8_t i = 0; i < length; i++)
	{
		uint32_t lastTime = deviceList[i].lastTime;
		uint32_t currentTime = Timer_GetMilSecTick();
		connectionStatus_e connection = deviceList[i].connectionStatus;

		if (Timer_CalculatorTime(lastTime, currentTime) >= DEVICE_OFFLINE_THRESHHOLD_INTERVAL && 	\
			connection == ONLINE)
		{
			DeviceManager_SetDeviceOffline(deviceList[i].nodeID);
			Main_ReceiveHandle(deviceList[i].nodeID, DEVICE_DISCONNECTED, NULL, 0);
		}
	}

	emberEventControlSetDelayMS(updateConnectStatusEventControl, 1000 * 60);
}

/* END FILE */
