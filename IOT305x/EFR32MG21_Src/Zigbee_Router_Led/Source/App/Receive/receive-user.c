/*
 * receive-user.c
 *
 *  Created on: Jul 30, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Receive/receive-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
static pReceiveHandler g_receiveHandler = NULL;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		RECEIVE_Init
 *
 * @brief:		Initialize the function to receive values
 *
 * @params:		callback
 *
 * @retVal:		None
 *
 * @note:		None
 */
void RECEIVE_Init (pReceiveHandler callback)
{
	g_receiveHandler = callback;
}

/*
 * @func:		emberAfPreMessageReceivedCallback
 *
 * @brief:		This function is called when any Zigbee message is received and
 * 				indicates whether the message has been processed or not
 *
 * @params:		incommingMessage
 *
 * @retVal:		True / False
 *
 * @note:		None
 */
boolean emberAfPreMessageReceivedCallback (EmberAfIncomingMessage* incommingMessage)
{
	return false;
}

/*
 * @func:		emberAfPreCommandReceivedCallback
 *
 * @brief:		The function handles incoming messages
 *
 * @params:		cmd - Pointer to the received command
 *
 * @retVal:		True / False
 *
 * @note:		None
 */
boolean emberAfPreCommandReceivedCallback (EmberAfClusterCommand *cmd)
{
	uint16_t 	clusterID = cmd -> apsFrame -> clusterId;

	switch(cmd->type)
	{
		case EMBER_INCOMING_UNICAST:
		{
			if (clusterID == ZCL_ON_OFF_CLUSTER_ID)
			{
				RECEIVE_OnOffClusterHandle(cmd);
				return true;
			}

			break;
		}

		default:
			break;
	}

	return false;
}

/*
 * @func:		RECEIVE_OnOffClusterHandle
 *
 * @brief:		The function executes ZCL On/Off
 *
 * @params:		cmd - Pointer to the received command
 *
 * @retVal:		None
 *
 * @note:		None
 */
void RECEIVE_OnOffClusterHandle (EmberAfClusterCommand* cmd)
{
	uint8_t commandID = cmd->commandId;
	uint8_t desEndpoint = cmd->apsFrame->destinationEndpoint;

	switch(commandID)
	{
		case ZCL_ON_COMMAND_ID:
		{
			if (desEndpoint == ENDPOINT_1)
			{
				led_turnOn(LED_2, CYAN);
				SEND_OnOffStateReport(desEndpoint, 1);
			}

			break;
		}

		case ZCL_OFF_COMMAND_ID:
		{
			if (desEndpoint == ENDPOINT_1)
			{
				led_turnOff(LED_2);
				SEND_OnOffStateReport(desEndpoint, 0);
			}

			break;
		}

		default:
			break;
	}
}

/*
 * @func:		emberAfPreZDOMessageReceivedCallback
 *
 * @brief:		The function receives ZDO messages and forwards them to the application
 *
 * @params[1]:	emberNodeId
 * @params[2]:	apsFrame
 * @params[3]:	message
 * @params[4]:	length
 *
 * @retVal:		True / False
 *
 * @note:		None
 */
bool emberAfPreZDOMessageReceivedCallback (EmberNodeId emberNodeId,
										   EmberApsFrame* apsFrame,
										   uint8_t* message,
										   uint16_t length)
{
	if (apsFrame->profileId == EMBER_ZDO_PROFILE_ID)
	{
		switch(apsFrame->clusterId)
		{
			case LEAVE_RESPONSE:
			{
				if (g_receiveHandler)
				{
					g_receiveHandler(emberNodeId, DEVICE_LEAVE_NETWORK, NULL, 0);
				}
				return true;
			} break;

			default:
				break;
		}
	}

	return false;
}

/* END FILE */
