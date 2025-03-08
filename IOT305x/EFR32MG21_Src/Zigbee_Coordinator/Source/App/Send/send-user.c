/*
 * send-user.c
 *
 *  Created on: Jul 30, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Send/send-user.h"

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


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		SEND_FillBufferGlobalCommand
 *
 * @brief:		The function creates a frame and places it into the buffer
 *
 * @params[1]:	clusterId
 * @params[2]:	attributeId
 * @params[3]:	commandId
 * @params[4]:	pDataRead - Payload pointer
 * @params[5]:	length - Payload length
 * @params[6]:	dataType - Data attribute type
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_FillBufferGlobalCommand (EmberAfClusterId clusterId,		\
							 	   EmberAfAttributeId attributeId,	\
								   uint8_t commandId,				\
								   uint8_t *pDataRead,				\
								   uint8_t length,					\
								   uint8_t dataType)
{
	uint8_t data[MAX_DATA_COMMAND_SIZE];
	data[0] = (uint8_t)(attributeId & 0x00FF);
	data[1] = (uint8_t)((attributeId & 0xFF00) >> 8);
	data[2] = EMBER_SUCCESS;
	data[3] = dataType;

	if(pDataRead != NULL)
	{
		memcpy(&data[4], pDataRead, length);
	}


	(void)emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND |
									 ZCL_FRAME_CONTROL_CLIENT_TO_SERVER |
									 ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
									 clusterId, commandId, "b", data, length + 4);
}

/*
 * @func:		SEND_SendCommandUnicast
 *
 * @brief:		The function sets the source endpoint and destination endpoint for the command,
 * 				then sends it via Unicast
 *
 * @params[1]:	nodeId - nodeID of destination device
 * @params[2]:	sourceEp - Source endpoint
 * @params[3]:	desEp - Destination endpoint
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_SendCommandUnicast (EmberNodeId nodeId, uint8_t sourceEp, uint8_t desEp)
{
	emberAfSetCommandEndpoints(sourceEp, desEp);
	(void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
}

/*
 * @func:		SEND_SetOnOffState
 *
 * @brief:		The function sends the LED On/Off control message
 *
 * @params[1]:	nodeId - nodeID of destination device
 * @params[2]:	sourceEp - Source endpoint
 * @params[3]:	state - LED state
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_SetOnOffState (EmberNodeId nodeId, uint8_t sourceEp, uint8_t state)
{
	if(state == 1)
	{
		emberAfFillCommandOnOffClusterOn();		// ON message
	}
	else
	{
		emberAfFillCommandOnOffClusterOff();	// OFF message
	}

	SEND_SendCommandUnicast(nodeId, sourceEp, DESTINATION_ENDPOINT_1);
}

/*
 * @func:		SEND_ResendZclCommandViaBinding
 *
 * @brief:		Generate ZCL command and send to all device appear in the binding table
 *
 * @params[1]: 	localEndpoint - local or source endpoint
 * @params[2]: 	remoteEndpoint - remote or destination endpoint
 * @params[3]: 	value - cluster on-off value
 * @params[4]: 	nodeID - nodeID of source device
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_ResendZclCommandViaBinding (uint8_t localEndpoint,	\
									  uint8_t remoteEndpoint,	\
									  bool value, 				\
									  uint16_t nodeID)
{
	EmberStatus status = EMBER_INVALID_BINDING_INDEX;

	for(uint8_t i = 0; i < EMBER_BINDING_TABLE_SIZE; i++)
	{
		EmberBindingTableEntry binding;		// An entry or a row in binding table

		status = emberGetBinding(i, &binding);
		EmberNodeId bindingNodeID = emberGetBindingRemoteNodeId(i);	// Get nodeID from the Binding Table

		if(status != EMBER_SUCCESS)
		{
			return;
		}
		else if((bindingNodeID == nodeID) && (binding.local == localEndpoint) && (binding.remote == remoteEndpoint))
		{
			// ignore source node, duplicated
			continue;
		}
		else if(bindingNodeID == emberAfGetNodeId())
		{
			// ignore self node, self binding
			continue;
		}
		else if((bindingNodeID != EMBER_SLEEPY_BROADCAST_ADDRESS) &&
				(bindingNodeID != EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS) &&
				(bindingNodeID != EMBER_BROADCAST_ADDRESS))
		{
			if(binding.local == localEndpoint)
			{
				if(value == 1)
				{
					emberAfFillCommandOnOffClusterOn();		// ON message
				}
				else
				{
					emberAfFillCommandOnOffClusterOff();	// OFF message
				}

				// Set the Endpoint
				emberAfSetCommandEndpoints(binding.local, binding.remote);

				// Send the message to the Target
				emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, bindingNodeID);
			}
		}
	}
}

/*
 * @func:		SEND_SetValueDistance
 *
 * @brief:		The function sends the detection distance threshold setting message
 *
 * @params[1]:	nodeId - nodeID of destination device
 * @params[2]:	sourceEp - Source endpoint
 * @params[3]:	value - Distance value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_SetValueDistance (EmberNodeId nodeId, uint8_t sourceEp, uint8_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_LD2410_DISTANCE_CLUSTER_ID,
								 ZCL_LD2410_DISTANCE_VALUE_ATTRIBUTE_ID,
								 ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*)&value,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(nodeId, sourceEp, DESTINATION_ENDPOINT_3);
}

/*
 * @func:		SEND_SetValueLatency
 *
 * @brief:		The function sends the delay value setting message
 *
 * @params[1]:	nodeId - nodeID of destination device
 * @params[2]:	sourceEp - Source endpoint
 * @params[3]:	value - Latency value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_SetValueLatency (EmberNodeId nodeId, uint8_t sourceEp, uint8_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_LD2410_LATENCY_CLUSTER_ID,
								 ZCL_LD2410_LATENCY_VALUE_ATTRIBUTE_ID,
								 ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*)&value,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(nodeId, sourceEp, DESTINATION_ENDPOINT_3);
}

/*
 * @func:		SEND_SetValueSensitivity
 *
 * @brief:		The function sends the sensitivity value setting message
 *
 * @params[1]:	nodeId - nodeID of destination device
 * @params[2]:	sourceEp - Source endpoint
 * @params[3]:	value - Sensitivity value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_SetValueSensitivity (EmberNodeId nodeId, uint8_t sourceEp, uint8_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_LD2410_SENSITIVITY_CLUSTER_ID,
								 ZCL_LD2410_SENSITIVITY_VALUE_ATTRIBUTE_ID,
								 ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*)&value,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(nodeId, sourceEp, DESTINATION_ENDPOINT_3);
}

/*
 * @func:		SEND_SetValueLight
 *
 * @brief:		The function sends the light intensity value setting message
 *
 * @params[1]:	nodeId - nodeID of destination device
 * @params[2]:	sourceEp - Source endpoint
 * @params[3]:	value - Light value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_SetValueLight (EmberNodeId nodeId, uint8_t sourceEp, uint16_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
								 ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_WRITE_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint16_t*)&value,
								 2,
								 ZCL_INT16U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(nodeId, sourceEp, DESTINATION_ENDPOINT_2);
}

/* END FILE */
