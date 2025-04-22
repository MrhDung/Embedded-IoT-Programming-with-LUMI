/*
 * main_Asm3.c
 *
 *  Created on: Aug 8, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Main/main_Asm3.h"

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
EmberEventControl networkLeaveEventControl;

e_MainState systemState;

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

	NETWORK_Init(USER_NetworkHandle);

	Receive_Init(USER_ReceiveLeaveHandle);

	Button_Init(USER_ButtonPressHandle, USER_ButtonHoldHandle);

	LDR_Init(USER_LdrUpdateValueLight);

	led_Init();

	Timer_Init(10);

	systemState = POWER_ON_STATE;

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

	switch(systemState)
	{
		case POWER_ON_STATE:
		{
			systemState = IDLE_STATE;

			nwkCurrentStatus = emberAfNetworkState();	// Check the network status

			if(nwkCurrentStatus == EMBER_NO_NETWORK)
			{
				toggleLed(LED_1, RED, 3, 300, 300);
			}
		} break;

		case REPORT_STATE:
		{
			systemState = IDLE_STATE;
			SEND_ReportInfoToHC();
			emberAfCorePrintln("Report State\n");
		} break;

		case IDLE_STATE:
			break;

		case LEAVE_NETWORK:
		{
			// Send the Leave Response message to the HC
			SEND_ZigDevRequest();

			// Need create event timer for leave network produce
			// because after send msg leave network, it must need coor's response
			emberEventControlSetDelayMS(networkLeaveEventControl, 2000);
		} break;

		case REBOOT_STATE:
		{
			systemState = IDLE_STATE;
			halReboot();
		} break;

		default:
			break;
	}
}

/*
 * @func:		USER_NetworkHandle
 *
 * @brief:		The function handles events at the Network layer
 *
 * @params:		networkResult - Network states
 *
 * @retVal:		None
 *
 * @note:		None
 */
void USER_NetworkHandle (e_NetworkState networkResult)
{
	emberAfCorePrintln("USER_NetworkHandle");

	static bool networkReady = false;

	switch(networkResult)
	{
		case NETWORK_HAS_PARENT:
		{
			emberAfCorePrintln("NETWORK_HAS_PARENT");
			toggleLed(LED_1, PINK, 3, 300, 300);
			networkReady = true;
			systemState = REPORT_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
		} break;

		case NETWORK_JOIN_SUCCESS:
		{
			emberAfCorePrintln("NETWORK_JOIN_SUCCESS");
			toggleLed(LED_1, PINK, 3, 300, 300);
			networkReady = true;
			systemState = REPORT_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
		} break;

		case NETWORK_JOIN_FAIL:
		{
			emberAfCorePrintln("NETWORK_JOIN_FAIL");
			toggleLed(LED_1, RED, 2, 500, 500);
			systemState = IDLE_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
		} break;

		case NETWORK_LOST_PARENT:
		{
			emberAfCorePrintln("NETWORK_LOST_PARENT");
			toggleLed(LED_1, YELLOW, 3, 300, 300);
			systemState = IDLE_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
		} break;

		case NETWORK_OUT_NETWORK:
		{
			if(networkReady)
			{
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
				toggleLed(LED_1, BLUE, 3, 300, 300);
				systemState = LEAVE_NETWORK;

				emberEventControlSetDelayMS(mainStateEventControl, 1000);
			}
		} break;

		default:
			break;
	}
}

/*
 * @func:		networkLeaveEventHandler
 *
 * @brief:		The function handles when the device leaves the network
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void networkLeaveEventHandler (void)
{
	emberEventControlSetInactive(networkLeaveEventControl);

	emberAfCorePrintln("Leaving network");
	systemState = REBOOT_STATE;

	emberClearBindingTable();
	emberLeaveNetwork();

	emberEventControlSetDelayMS(mainStateEventControl, 2000);
}

/*
 * @func:		USER_ButtonPressHandle
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
void USER_ButtonPressHandle (uint8_t button, uint8_t pressCount)
{
	if(button == SW1)
	{
		switch(pressCount)
		{
			case press_1:
			{
				emberAfCorePrintln("Turn on LED_1: BLUE");
				led_turnOn(LED_1, BLUE);
				SEND_OnOffStateReport(ENDPOINT_1, 1);
				SEND_ResendZclCommandViaBinding(1, ENDPOINT_1, 1, emberAfGetNodeId());
			} break;

			case press_2:
			{
				emberAfCorePrintln("Turn off LED_1");
				led_turnOff(LED_1);
				SEND_OnOffStateReport(ENDPOINT_1, 0);
				SEND_ResendZclCommandViaBinding(1, ENDPOINT_1, 0, emberAfGetNodeId());
			} break;

			case press_3:
			{
				emberAfPluginFindAndBindTargetStart(ENDPOINT_1);
			} break;

			case press_4:
			{
				emberAfPluginFindAndBindInitiatorStart(ENDPOINT_1);
			} break;

			case press_5:
			{
				systemState = LEAVE_NETWORK;
				emberEventControlSetActive(mainStateEventControl);
			} break;

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
				emberAfCorePrintln("Turn on LED_2: BLUE");
				led_turnOn(LED_2, BLUE);
				SEND_OnOffStateReport(ENDPOINT_2, 1);
				SEND_ResendZclCommandViaBinding(2, ENDPOINT_2, 1, emberAfGetNodeId());
			} break;

			case press_2:
			{
				emberAfCorePrintln("Turn off LED_2");
				led_turnOff(LED_2);
				SEND_OnOffStateReport(ENDPOINT_2, 0);
				SEND_ResendZclCommandViaBinding(2, ENDPOINT_2, 0, emberAfGetNodeId());
			} break;

			case press_3:
				break;

			case press_4:
				break;

			case press_5:
				break;

			default:
				break;
		}
	}
}

/*
 * @func:		USER_ButtonHoldHandle
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
void USER_ButtonHoldHandle (uint8_t button, uint8_t holdCount)
{
	if(button == SW1)
	{
		switch(holdCount)
		{
			case hold_1s:
				break;

			case hold_2s:
				break;

			case hold_3s:
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
			{
				NETWORK_FindAndJoin();
				emberAfCorePrintln("SW2 is held down 1 seconds");
			} break;

			case hold_2s:
				break;

			case hold_3s:
				break;

			default:
				break;
		}
	}
}

/*
 * @func:		emberAfPreCommandReceivedCallback
 *
 * @brief:		The function handles incoming messages
 *
 * @params:		cmd - Pointer to the received command
 *
 * @retVal:		true / false
 *
 * @note:		None
 */
boolean emberAfPreCommandReceivedCallback (EmberAfClusterCommand* cmd)
{
	bool 		commandID = cmd -> commandId;
	uint16_t 	clusterID = cmd -> apsFrame -> clusterId;
	uint8_t 	desEndpoint = cmd -> apsFrame -> destinationEndpoint;

	switch(cmd->type)
	{
		case EMBER_INCOMING_UNICAST:
		{
			if (clusterID == ZCL_ON_OFF_CLUSTER_ID)
			{
				USER_ReceiveOnOffClusterHandle(cmd);
				SEND_ResendZclCommandViaBinding(desEndpoint, desEndpoint, commandID, cmd->source);
				return true;
			}
		} break;

		case EMBER_INCOMING_MULTICAST:
		{
			if (clusterID == ZCL_ON_OFF_CLUSTER_ID)
			{
				USER_ReceiveOnOffClusterHandle(cmd);
				return true;
			}
		} break;

		default:
			break;
	}

	return false;
}

/*
 * @func:		USER_ReceiveOnOffClusterHandle
 *
 * @brief:		The function executes ZCL on/off
 *
 * @params:		cmd - Pointer to the received command
 *
 * @retVal:		None
 *
 * @note:		None
 */
void USER_ReceiveOnOffClusterHandle (EmberAfClusterCommand* cmd)
{
	uint8_t commandID = cmd -> commandId;
	uint8_t desEndpoint = cmd -> apsFrame -> destinationEndpoint;

//	emberAfCorePrintln("USER_ReceiveOnOffClusterHandle SourEndpoint = %d, CommandID = %d\n",desEndPoint, commandID);

	switch(commandID)
	{
		case ZCL_ON_COMMAND_ID:
		{
			if (desEndpoint == ENDPOINT_1)
			{
				led_turnOn(LED_1, BLUE);
			}
			else if (desEndpoint == ENDPOINT_2)
			{
				led_turnOn(LED_2, BLUE);
			}

			SEND_OnOffStateReport(desEndpoint, 1);
		} break;

		case ZCL_OFF_COMMAND_ID:
		{
			if (desEndpoint == ENDPOINT_1)
			{
				led_turnOff(LED_1);
			}
			else if (desEndpoint == ENDPOINT_2)
			{
				led_turnOff(LED_2);
			}

			SEND_OnOffStateReport(desEndpoint, 0);
		} break;

		default:
			break;
	}
}

/*
 * @func:		USER_ReceiveLeaveHandle
 *
 * @brief:		The function handles device removal messages from the Home Controller
 *
 * @params[1]:	nodeId - ZigBee network address
 * @params[2]:	receiveId - Received command
 *
 * @retVal:		None
 *
 * @note:		None
 */
void USER_ReceiveLeaveHandle (EmberNodeId nodeId, e_RECEIVE_CMD_ID receiveId)
{
	switch (receiveId)
	{
		case DEVICE_LEAVE_NETWORK:
		{
			emberAfCorePrintln("DEVICE_LEAVE_NETWORK");
			systemState = LEAVE_NETWORK;
			emberEventControlSetActive(mainStateEventControl);
		} break;

		default:
			break;
	}
}

/*
 * @func:		USER_LdrUpdateValueLight
 *
 * @brief:		The function to update light intensity value
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void USER_LdrUpdateValueLight (void)
{
	static float luxCurEst = 0;
	static uint32_t prevLux = 0;

	uint32_t currentLux;

	// Get light intensity value
	LDR_Read(&currentLux);

	// Filter noise from the measured value
	currentLux = (uint32_t)KalmanFilter(&luxCurEst,
									   (float)currentLux,
									   MEASURE_NOISE_INIT,
									   PROCESS_NOISE_INIT);

	/* Update the light intensity value if there is a great change, every 60 seconds */
	if (((currentLux > prevLux) && (currentLux - prevLux >= THRESHOLD_LUX_REPORT)) 	||
		((currentLux < prevLux) && (prevLux - currentLux >= THRESHOLD_LUX_REPORT)))
	{
		SEND_LDRValueReport(ENDPOINT_3, currentLux);
		emberAfCorePrintln("Light: %"PRIu32" Lux", currentLux);
	}

	if (currentLux >= THRESHOLD_LUX_CONTROL_LED)
	{
		led_turnOn(LED_2, GREEN);
	}
	else
	{
		led_turnOff(LED_2);
	}

	prevLux = currentLux;
}

/* END FILE */
