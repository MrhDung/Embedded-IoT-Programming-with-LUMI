/*
 * main-led.c
 *
 *  Created on: Dec 1, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Main/main-led.h"

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
EmberEventControl keepAliveEventControl;

MainState_e g_systemState;

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
	RECEIVE_Init(Main_ReceiveLeaveHandle);
	Button_Init(Main_ButtonPressHandle, Main_ButtonHoldHandle);
	led_Init();
	Timer_Init(10);

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
			}

			emberEventControlSetActive(keepAliveEventControl);
			break;
		}

		case REPORT_STATE:
		{
			g_systemState = IDLE_STATE;
			SEND_ReportInfoToHC();
			break;
		}

		case IDLE_STATE:
			break;

		case LEAVE_NETWORK:
		{
			SEND_OnOffStateReport(ENDPOINT_1, 0);

			// Send the Leave Response message to the HC
			SEND_LeaveResponse();

			// Need create event timer for leave network produce
			// because after send msg leave network, it must need coor's response
			emberEventControlSetDelayMS(networkLeaveEventControl, 2000);
			break;
		}

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
void Main_NetworkHandle (NetworkState_e networkResult)
{
	emberAfCorePrintln("Main_NetworkHandle");

	static bool networkReady = false;

	switch(networkResult)
	{
		case NETWORK_HAS_PARENT:
		{
			emberAfCorePrintln("NETWORK_HAS_PARENT");
			toggleLed(LED_2, PINK, 3, 300, 300);
			networkReady = true;
			g_systemState = REPORT_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		}

		case NETWORK_JOIN_SUCCESS:
		{
			emberAfCorePrintln("NETWORK_JOIN_SUCCESS");
			toggleLed(LED_2, PINK, 3, 300, 300);
			networkReady = true;
			g_systemState = REPORT_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		}

		case NETWORK_JOIN_FAIL:
		{
			emberAfCorePrintln("NETWORK_JOIN_FAIL");
			toggleLed(LED_2, RED, 2, 500, 500);
			g_systemState = IDLE_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		}

		case NETWORK_LOST_PARENT:
		{
			emberAfCorePrintln("NETWORK_LOST_PARENT");
			toggleLed(LED_2, YELLOW, 3, 300, 300);
			g_systemState = IDLE_STATE;

			emberEventControlSetDelayMS(mainStateEventControl, 1000);
			break;
		}

		case NETWORK_OUT_NETWORK:
		{
			if(networkReady)
			{
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
				toggleLed(LED_2, BLUE, 3, 300, 300);
				g_systemState = LEAVE_NETWORK;

				emberEventControlSetDelayMS(mainStateEventControl, 1000);
			}

			break;
		}

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
	g_systemState = REBOOT_STATE;

	emberClearBindingTable();
	emberLeaveNetwork();

	emberEventControlSetDelayMS(mainStateEventControl, 2000);
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
	static bool bindingEnable = 0;

	if(button == SW1)
	{
		switch(pressCount)
		{
			case press_1:
			{
				SEND_KeepAliveToZc();
				emberAfCorePrintln("press_1");
				break;
			}

			case press_2:
			{
				NETWORK_FindAndJoin();
				break;
			}

			case press_3:
			{
				bindingEnable = !bindingEnable;

				if (bindingEnable == 1)
				{
					emberAfPluginFindAndBindTargetStart(1);
				}
				else
				{
					emberClearBindingTable();
				}

				break;
			}

			case press_5:
			{
				g_systemState = LEAVE_NETWORK;
				emberEventControlSetActive(mainStateEventControl);
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
				emberAfCorePrintln("SW2_press_1");
				break;

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
 * @func:		Main_ReceiveLeaveHandle
 *
 * @brief:		The function handles device removal messages from the Controller
 *
 * @params[1]:	nodeId
 * @params[2]:	receiveId
 * @params[3]:	payload
 * @params[4]:	payloadLength
 *
 * @retVal:		None
 *
 * @note:		None
 */
void Main_ReceiveLeaveHandle (EmberNodeId nodeId, 			\
							  RECEIVE_CMD_ID_e receiveId, 	\
							  uint8_t *payload, 			\
							  uint8_t payloadLength)
{
	switch (receiveId)
	{
		case DEVICE_LEAVE_NETWORK:
		{
			emberAfCorePrintln("DEVICE_LEAVE_NETWORK");
			g_systemState = LEAVE_NETWORK;
			emberEventControlSetActive(mainStateEventControl);
			break;
		}

		default:
			break;
	}
}

/*
 * @func:  		keepAliveEventHandler
 *
 * @brief:		The function periodically sends messages to the Coordinator
 * 				to maintain connection
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void keepAliveEventHandler (void)
{
	emberEventControlSetInactive(keepAliveEventControl);

	SEND_KeepAliveToZc();

	emberEventControlSetDelayMS(keepAliveEventControl, 1000 * 60 * 15);
}

/* END FILE */
