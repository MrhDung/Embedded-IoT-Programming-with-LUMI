/*
 * main_Router2.c
 *
 *  Created on: Jul 26, 2024
 *      Author: Mr.hDung
 *
 *  Description: Router device functions as Target when configuring Binding
 *
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "Source/Mid/Led/led-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void ZclCommandHandler (uint8_t commandId);

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
void emberAfMainInitCallback(void)
{
	led_Init();
}

/*
 * @func:		ZclCommandHandler
 *
 * @brief:		Taking action when receive ZCL command
 *
 * @params:		commandId - ID of the command
 *
 * @retVal:		None
 *
 * @note:		None
 */
void ZclCommandHandler (uint8_t commandId)
{
	switch(commandId)
	{
		case ZCL_ON_COMMAND_ID:
		{
			led_turnOn(LED_1, GREEN);
		} break;

		case ZCL_OFF_COMMAND_ID:
		{
			led_turnOff(LED_1);
		} break;

		default:
			break;
	}
}

/*
 * @func:		emberAfPreCommandReceivedCallback
 *
 * @brief:		Process received commands
 *
 * @params:		cmd - Pointer to the types of received messages
 *
 * @retVal:		True/False
 *
 * @note:		None
 */
boolean emberAfPreCommandReceivedCallback (EmberAfClusterCommand* cmd)
{
	bool state = cmd->commandId;
	uint16_t clusterID = cmd -> apsFrame -> clusterId;

	switch(cmd->type)
	{
		case EMBER_INCOMING_UNICAST:
		{
			if (clusterID == ZCL_ON_OFF_CLUSTER_ID)
			{
				ZclCommandHandler(state);
				return true;
			}
		} break;

		case EMBER_INCOMING_MULTICAST:
		{
			if (clusterID == ZCL_ON_OFF_CLUSTER_ID)
			{
				ZclCommandHandler(state);
				return true;
			}
		} break;

		default:
			break;
	}

	return false;
}

/* END FILE */
