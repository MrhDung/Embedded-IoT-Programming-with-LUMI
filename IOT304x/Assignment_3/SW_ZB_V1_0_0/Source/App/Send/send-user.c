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
void SEND_FillBufferGlobalCommand (EmberAfClusterId clusterId,
							 	   EmberAfAttributeId attributeId,
								   uint8_t commandId,
								   uint8_t *pDataRead,
								   uint8_t length,
								   uint8_t dataType)
{
	uint8_t data[MAX_DATA_COMMAND_SIZE];
	data[0] = (uint8_t)(attributeId & 0x00FF);
	data[1] = (uint8_t)((attributeId & 0xFF00) >> 8);
	data[2] = EMBER_SUCCESS;
	data[3] = dataType;

	memcpy(&data[4], pDataRead, length);

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
 * @params[1]:	sourceEp - Source endpoint
 * @params[2]:	desEp - Destination endpoint
 * @params[3]:	nwkAddress - Network address
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_SendCommandUnicast (uint8_t sourceEp, uint8_t desEp, uint32_t nwkAddress)
{
	emberAfSetCommandEndpoints(sourceEp, desEp);
	(void)emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nwkAddress);
}

/*
 * @func:		SEND_ZigDevRequest
 *
 * @brief:		The function sends a ZDO leave network notification to the Home Controller
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_ZigDevRequest (void)
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

/*
 * @func:		SEND_ReportInfoToHC
 *
 * @brief:		The function sends information to the Home Controller
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_ReportInfoToHC (void)
{
	uint8_t modelID[] = {'S', 'W', '2', '_', 'L', 'M', '1'};
	uint8_t manufactureID[] = {'L', 'u', 'm', 'i'};
	uint8_t version[] = {'1'};

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}

	// Send model to HC
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 modelID,
								 7,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATION_ENDPOINT,
							HC_NETWORK_ADDRESS);

	// Send manufacture to HC
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 manufactureID,
								 4,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATION_ENDPOINT,
							HC_NETWORK_ADDRESS);

	// Send version to HC
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 version,
								 1,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATION_ENDPOINT,
							HC_NETWORK_ADDRESS);
}

/*
 * @func:		SEND_OnOffStateReport
 *
 * @brief:		The function sends an LED on/off notification to the Home Controller
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

	SEND_SendCommandUnicast(sourceEp,
							DESTINATION_ENDPOINT,
							HC_NETWORK_ADDRESS);
}

/*
 * @func:		SEND_LevelStateReport
 *
 * @brief:		The function sends the LED brightness value to the Home Controller
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

	SEND_SendCommandUnicast(sourceEp,
							DESTINATION_ENDPOINT,
							HC_NETWORK_ADDRESS);
}

/*
 * @func:		SEND_LDRValueReport
 *
 * @brief:		The function sends the measured light intensity value to the Home Controller
 *
 * @params[1]:	sourceEp - Source endpoint
 * @params[2]:	value - Sensor value
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
								 (uint16_t*) &value,
								 2,
								 ZCL_INT16U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(sourceEp,
							DESTINATION_ENDPOINT,
							HC_NETWORK_ADDRESS);
}

/*
 * @func:		SEND_ResendZclCommandViaBinding
 *
 * @brief:		Generate ZCL command and send to all device appear in the binding table
 *
 * @params[1]: 	localEndpoint - local or source endpoint
 * @params[2]: 	remoteEndpoint - remote or destination endpoint
 * @params[3]: 	value - cluster on-off value
 * @params[4]: 	nodeID - the device node ID
 *
 * @retVal:		None
 *
 * @note:		None
 */
void SEND_ResendZclCommandViaBinding (uint8_t localEndpoint, uint8_t remoteEndpoint,
									  bool value, uint16_t nodeID)
{
	EmberStatus status = EMBER_INVALID_BINDING_INDEX;

	for(uint8_t i = 0; i < EMBER_BINDING_TABLE_SIZE; i++)
	{
		EmberBindingTableEntry binding;		//an entry or a row in binding table

		status = emberGetBinding(i, &binding);
		EmberNodeId bindingNodeID = emberGetBindingRemoteNodeId(i);	// Get nodeID from the Binding Table

		if(status != EMBER_SUCCESS)
		{
			return;
		}
		else if((bindingNodeID == nodeID) && (binding.local == localEndpoint) && (binding.remote == remoteEndpoint))
		{
			//ignore source node, duplicated
			continue;
		}
		else if(bindingNodeID == emberAfGetNodeId())
		{
			//ignore self node, self binding
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
					// ON message
					emberAfFillCommandOnOffClusterOn();
				}
				else
				{
					// OFF message
					emberAfFillCommandOnOffClusterOff();
				}

				// Set the Endpoint
				emberAfSetCommandEndpoints(binding.local, binding.remote);
				// Send the message to the Target
				emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, bindingNodeID);
			}
		}
	}
}

///*
// * @func:		SEND_PIRStateReport
// *
// * @brief:		The function sends the status received from the PIR sensor to the Home Controller
// *
// * @params[1]:	sourceEP - Source endpoint
// * @params[2]:	value - Sensor value
// *
// * @retVal:		None
// *
// * @note:		None
// */
//void SEND_PIRStateReport (uint8_t sourceEP, uint8_t value)
//{
//	SEND_FillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,
//								 ZCL_ZONE_STATE_ATTRIBUTE_ID,
//								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
//								 (uint8_t*)&value,
//								 1,
//								 ZCL_INT8U_ATTRIBUTE_TYPE);
//
//	SEND_SendCommandUnicast(sourceEP,
//							DESTINATION_ENDPOINT,
//							HC_NETWORK_ADDRESS);
//
//	emberAfWriteServerAttribute(sourceEP,
//								ZCL_IAS_ZONE_CLUSTER_ID,
//								ZCL_ZONE_STATE_ATTRIBUTE_ID,
//								(uint8_t*) &value,
//								ZCL_INT8U_ATTRIBUTE_TYPE);
//}

/* END FILE */
