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
 * @func:		SEND_ReportInfoToHC
 *
 * @brief:		The function sends information to the Controller
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_ReportInfoToHC (void)
{
	uint8_t modelID[] = {'S', 'E', 'N', 'S', 'O', 'R'};

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}

	// Send model to Controller
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 modelID,
								 sizeof(modelID),
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(HC_NETWORK_ADDRESS,
							SOURCE_ENDPOINT_PRIMARY,
							DESTINATION_ENDPOINT);
}

/*
 * @func:		SEND_OnOffStateReport
 *
 * @brief:		The function sends an LED on/off notification to the Controller
 *
 * @params[1]:	sourceEp - Source endpoint
 * @params[2]:	state - LED status
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_OnOffStateReport (uint8_t sourceEp, uint8_t state)
{
	SEND_FillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,
								 ZCL_ON_OFF_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*)&state,
								 1,
								 ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(HC_NETWORK_ADDRESS, sourceEp, DESTINATION_ENDPOINT);
}

/*
 * @func:		SEND_LevelStateReport
 *
 * @brief:		The function sends the LED brightness value to the Controller
 *
 * @params[1]:	sourceEp - Source endpoint
 * @params[2]:	value - LED brightness value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_LevelStateReport (uint8_t sourceEp, uint8_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_LEVEL_CONTROL_CLUSTER_ID,
								 ZCL_CURRENT_LEVEL_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*)&value,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(HC_NETWORK_ADDRESS, sourceEp, DESTINATION_ENDPOINT);
}

/*
 * @func:		SEND_TempValueReport
 *
 * @brief:		The function sends the measured temperature value to the Controller
 *
 * @params[1]:	sourceEp - Source endpoint
 * @params[2]:	value - Temperature value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_TempValueReport (uint8_t sourceEp, uint8_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
								 ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*)&value,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(HC_NETWORK_ADDRESS, sourceEp, DESTINATION_ENDPOINT);
}

/*
 * @func:		SEND_HumiValueReport
 *
 * @brief:		The function sends the measured humidity value to the Controller
 *
 * @params[1]:	sourceEp - Source endpoint
 * @params[2]:	value - Humidity value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_HumiValueReport (uint8_t sourceEp, uint8_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID,
								 ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*)&value,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(HC_NETWORK_ADDRESS, sourceEp, DESTINATION_ENDPOINT);
}

/*
 * @func:		SEND_LDRValueReport
 *
 * @brief:		The function sends the measured light intensity value to the Controller
 *
 * @params[1]:	sourceEp - Source endpoint
 * @params[2]:	value - Light value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_LDRValueReport (uint8_t sourceEp, uint16_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
								 ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint16_t*)&value,
								 2,
								 ZCL_INT16U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(HC_NETWORK_ADDRESS, sourceEp, DESTINATION_ENDPOINT);
}

/*
 * @func:		SEND_LD2410StateReport
 *
 * @brief:		The function sends the status received from the LD2410 sensor to the Controller
 *
 * @params[1]:	sourceEP - Source endpoint
 * @params[2]:	value - LD2410 value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_LD2410StateReport (uint8_t sourceEP, uint8_t value)
{
	SEND_FillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,
								 ZCL_ZONE_STATE_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*)&value,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(HC_NETWORK_ADDRESS, sourceEP, DESTINATION_ENDPOINT);

	emberAfWriteServerAttribute(sourceEP,
								ZCL_IAS_ZONE_CLUSTER_ID,
								ZCL_ZONE_STATE_ATTRIBUTE_ID,
								(uint8_t*)&value,
								ZCL_INT8U_ATTRIBUTE_TYPE);
}

/*
 * @func:		SEND_ResendZclCommandViaBinding
 *
 * @brief:		Generate ZCL command and send to all device appear in the binding table
 *
 * @params[1]: 	localEndpoint - local or source endpoint
 * @params[2]: 	remoteEndpoint - remote or destination endpoint
 * @params[3]: 	value - cluster on-off value
 * @params[4]: 	nodeID - source device nodeID
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
 * @func:		SEND_KeepAliveToZc
 *
 * @brief:		The function sends keep alive message to the Controller
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_KeepAliveToZc (void)
{
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}

	// Send Keep Alive to ZC
	SEND_FillBufferGlobalCommand(ZCL_KEEPALIVE_CLUSTER_ID,
								 ZCL_KEEPALIVE_BASE_ATTRIBUTE_ID,
								 0,
								 NULL,
								 0,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(HC_NETWORK_ADDRESS,
							SOURCE_ENDPOINT_PRIMARY,
							DESTINATION_ENDPOINT);
}

/*
 * @func:		SEND_LeaveResponse
 *
 * @brief:		The function sends a ZDO leave network notification to the Controller
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_LeaveResponse (void)
{
	uint8_t contents[2];
	contents[1] = 0x00;

	// Transmit ZDO message
	emberSendZigDevRequest(HC_NETWORK_ADDRESS,
						   LEAVE_RESPONSE,
						   EMBER_AF_DEFAULT_APS_OPTIONS,
						   contents,
						   sizeof(contents));
}

/* END FILE */
