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

static uint8_t g_setDistance = 0;
static uint8_t g_setLatency = 0;
static uint8_t g_setSensitivity = 0;
static uint16_t g_setLight = 0;

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
boolean emberAfPreCommandReceivedCallback (EmberAfClusterCommand* cmd)
{
//	EmberNodeId nodeId = cmd->source;
	uint16_t clusterID = cmd->apsFrame->clusterId;
//	uint8_t commandID = cmd->commandId;
	uint16_t attrID = (uint16_t)(cmd->buffer[cmd->payloadStartIndex] | cmd->buffer[cmd->payloadStartIndex + 1] << 8);
	uint16_t bufIndex = cmd->payloadStartIndex + 4; 	// Buffer payload start index

	uint8_t payload[2];
	payload[0] = cmd->buffer[bufIndex];
	payload[1] = cmd->buffer[bufIndex + 1];

	switch(clusterID)
	{
		case ZCL_LD2410_DISTANCE_CLUSTER_ID:
		{
			if(attrID == ZCL_LD2410_DISTANCE_VALUE_ATTRIBUTE_ID)
			{
				uint8_t newDistance = (uint8_t)(payload[0]);

				// Store the configured Distance value
				g_setDistance = newDistance;

				// Send a message to set the Distance value
				// while preserving the configured Latency value
				LD2410_SetValueDistanceLatency(g_setDistance, g_setDistance, g_setLatency);
			}

			return true;
		} break;

		case ZCL_LD2410_LATENCY_CLUSTER_ID:
		{
			if(attrID == ZCL_LD2410_LATENCY_VALUE_ATTRIBUTE_ID)
			{
				uint8_t newLatency = (uint8_t)(payload[0]);

				// Store the configured Latency value
				g_setLatency = newLatency;

				// Send a message to set the Latency value
				// while preserving the configured Distance value
				LD2410_SetValueDistanceLatency(g_setDistance, g_setDistance, g_setLatency);
			}

			return true;
		} break;

		case ZCL_LD2410_SENSITIVITY_CLUSTER_ID:
		{
			if(attrID == ZCL_LD2410_SENSITIVITY_VALUE_ATTRIBUTE_ID)
			{
				g_setSensitivity = payload[0];

				// Send a message to set the Sensitivity value
				LD2410_SetValueSensitivity(g_setSensitivity, g_setSensitivity);
			}

			return true;
		} break;

		case ZCL_ILLUM_MEASUREMENT_CLUSTER_ID:
		{
			if(attrID == ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID)
			{
				// Store the light intensity value as a condition for controlling the LED On/Off
				g_setLight = (uint16_t)(payload[0] | (payload[1] << 8));
			}

			return true;
		} break;

		default:
			break;
	}

	return false;
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

/*
 * @func:		RECEIVE_SetValueLatency
 *
 * @brief:		The function retrieves the configured Latency value
 *
 * @params:		None
 *
 * @retVal:		g_setLatency
 *
 * @note:		None
 */
uint8_t RECEIVE_SetValueLatency (void)
{
	return g_setLatency;
}

/*
 * @func:		RECEIVE_SetValueLight
 *
 * @brief:		The function retrieves the configured light intensity value
 *
 * @params:		None
 *
 * @retVal:		g_setLight
 *
 * @note:		None
 */
uint16_t RECEIVE_SetValueLight (void)
{
	return g_setLight;
}

/* END FILE */
