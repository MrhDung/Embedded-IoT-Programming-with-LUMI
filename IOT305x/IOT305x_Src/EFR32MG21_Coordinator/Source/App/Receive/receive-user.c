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

static uint8_t g_valueTemp = 0;
static uint8_t g_valueHumi = 0;
static uint8_t g_detecState = 0;
static uint8_t g_ledState = 0;
static uint16_t g_valueLight = 0;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		Receive_Init
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
boolean emberAfPreMessageReceivedCallback (EmberAfIncomingMessage *incommingMessage)
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
	EmberNodeId nodeId = cmd->source;
	uint16_t clusterID = cmd->apsFrame->clusterId;
	uint16_t attrID = (uint16_t)(cmd->buffer[cmd->payloadStartIndex] |
								 cmd->buffer[cmd->payloadStartIndex + 1] << 8);
	uint16_t bufIndex = cmd->payloadStartIndex + 4; 	// buffer payload start index
	
	uint8_t payload[2];
	payload[0] = cmd->buffer[bufIndex];
	payload[1] = cmd->buffer[bufIndex + 1];

	switch(clusterID)
	{
		case ZCL_BASIC_CLUSTER_ID:
		{
			// If receive a model id from a device
			if(attrID == ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID)
			{
				// Get the model id from payload
				uint8_t modelID[20] = {0};
				uint8_t index = 0;

				for(; bufIndex < cmd->bufLen; index++, bufIndex++)
				{
					modelID[index] = cmd->buffer[bufIndex];
				}

				if(g_receiveHandler != NULL)
				{
					if(DeviceManager_CheckDeviceFromList(cmd->source) != DEVICE_INDEX_INVALID)
					{
						g_receiveHandler(nodeId, DEVICE_CONNECTED, modelID, index);
					}
					else
					{
						g_receiveHandler(nodeId, DEVICE_JOIN_NETWORK, modelID, index);
					}
				}
			}
			return true;
		} break;

		case ZCL_ON_OFF_CLUSTER_ID:
		{
			g_ledState = payload[0];

			if (payload[0] == 1)
			{
				g_receiveHandler(nodeId, DEVICE_LED_ON, &payload[0], 1);
			}
			else if (payload[0] == 0)
			{
				g_receiveHandler(nodeId, DEVICE_LED_OFF, &payload[0], 1);
			}

			return true;
		} break;

		case ZCL_TEMP_MEASUREMENT_CLUSTER_ID:
		{
			if(attrID == ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID)
			{
				g_valueTemp = payload[0];

				if(g_receiveHandler != NULL)
				{
					g_receiveHandler(nodeId, DEVICE_SENSOR_TEMP_VALUE, &payload[0], 1);
				}
			}

			return true;
		} break;

		case ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID:
		{
			if(attrID == ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID)
			{
				g_valueHumi = payload[0];

				if(g_receiveHandler != NULL)
				{
					g_receiveHandler(nodeId, DEVICE_SENSOR_HUMI_VALUE, &payload[0], 1);
				}
			}

			return true;
		} break;

		case ZCL_ILLUM_MEASUREMENT_CLUSTER_ID:
		{
			if(attrID == ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID)
			{
				g_valueLight = payload[0] << 8 | payload[1];

				if(g_receiveHandler != NULL)
				{
					g_receiveHandler(nodeId, DEVICE_SENSOR_LIGHT_VALUE, payload, 2);
				}
			}

			return true;
		} break;

		case ZCL_IAS_ZONE_CLUSTER_ID:
		{
			if(attrID == ZCL_ZONE_STATE_ATTRIBUTE_ID)
			{
				g_detecState = payload[0];

				if (payload[0] == 1)
				{
					if(g_receiveHandler != NULL)
					{
						g_receiveHandler(nodeId, DEVICE_SENSOR_LD2410_MOTION, &payload[0], 1);
					}
				}
				else if (payload[0] == 0)
				{
					if(g_receiveHandler != NULL)
					{
						g_receiveHandler(nodeId, DEVICE_SENSOR_LD2410_UNMOTION, &payload[0], 1);
					}
				}
			}

			return true;
		} break;

		case ZCL_KEEPALIVE_CLUSTER_ID:
		{
			if(attrID == ZCL_KEEPALIVE_BASE_ATTRIBUTE_ID)
			{
				g_receiveHandler(nodeId, DEVICE_KEEPALIVE, NULL, 0);
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
bool emberAfPreZDOMessageReceivedCallback (EmberNodeId emberNodeId,	\
										   EmberApsFrame* apsFrame,	\
										   uint8_t* message,		\
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
 * @func:		RECEIVE_GetValueTemp
 *
 * @brief:		The function retrieves the received temperature value
 *
 * @params:		None
 *
 * @retVal:		g_valueTemp
 *
 * @note:		None
 */
uint8_t RECEIVE_GetValueTemp (void)
{
	return g_valueTemp;
}

/*
 * @func:		RECEIVE_GetValueHumi
 *
 * @brief:		The function retrieves the received humidity value
 *
 * @params:		None
 *
 * @retVal:		g_valueHumi
 *
 * @note:		None
 */
uint8_t RECEIVE_GetValueHumi (void)
{
	return g_valueHumi;
}

/*
 * @func:		RECEIVE_GetValueLight
 *
 * @brief:		The function retrieves the received light intensity value
 *
 * @params:		None
 *
 * @retVal:		g_valueLight
 *
 * @note:		None
 */
uint16_t RECEIVE_GetValueLight (void)
{
	return g_valueLight;
}

/*
 * @func:		RECEIVE_GetDetecState
 *
 * @brief:		The function retrieves the detected status
 *
 * @params:		None
 *
 * @retVal:		g_detecState
 *
 * @note:		None
 */
uint8_t RECEIVE_GetDetecState (void)
{
	return g_detecState;
}

/*
 * @func:		RECEIVE_GetLedState
 *
 * @brief:		The function retrieves the LED status
 *
 * @params:		None
 *
 * @retVal:		g_ledState
 *
 * @note:		None
 */
uint8_t RECEIVE_GetLedState (void)
{
	return g_ledState;
}

/* END FILE */
