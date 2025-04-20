/*
 * USART.c
 *
 *  Created on: Sep 25, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Mid/Usart/USART.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl usart2ScanEventControl;

pUsartStateCallback g_usart2StateCallback = NULL;

// Variable storing the position of an element in the array holding data retrieved
// from the buffer
uint8_t 	g_IndexRxBuf = 0;

// Received data storage array
uint8_t 	g_strRxBuffer[RX_BUFFER_SIZE] = {0};

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:  		USART2_Init
 *
 * @brief:		Initialize USART
 *
 * @param:		callback
 *
 * @retval:		None
 *
 * @note:		+ USART2: Tx - PC01
 * 				+ USART2: Rx - PC02
 */
void USART2_Init (pUsartStateCallback callback)
{
	emberSerialInit(COM_USART2, 115200, PARITY_NONE, 1);

	g_usart2StateCallback = callback;

	// Start the scan event, with a delay on the first call: waiting for peripheral initialization
	emberEventControlSetDelayMS(usart2ScanEventControl, 1000);
}

/*
 * @func:  		usart2ScanEventHandler
 *
 * @brief:		Periodic data retrieval function
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void usart2ScanEventHandler (void)
{
	emberEventControlSetInactive(usart2ScanEventControl);

	uint8_t 	dataByte;
	uint16_t 	bytesReceived = 0;        // Variable storing the number of received bytes

	// Get the number of received bytes
	uint16_t numOfByteAvail = emberSerialReadAvailable(COM_USART2);

	if(numOfByteAvail > 0)
	{
		while (numOfByteAvail > 0 && bytesReceived < RX_BUFFER_SIZE)
		{
			emberSerialReadByte(COM_USART2, &dataByte);

			g_strRxBuffer[bytesReceived++] = dataByte;		// Store the received byte in the buffer
			numOfByteAvail--;
		}

		processSerialHandle();
	}

	emberEventControlSetDelayMS(usart2ScanEventControl, USART_PROCESS_SERIAL_INTERVAL);
}

/*
 * @func:  		processSerialHandle
 *
 * @brief:		The function retrieves and sends data to the main function for event execution
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void processSerialHandle (void)
{
	USART_STATE_e	RxState = PollRxBuff(COM_USART2);

	if (RxState == USART_STATE_DATA_RECEIVED || RxState == USART_STATE_ERROR)
	{
		if (g_usart2StateCallback != NULL)
		{
			g_usart2StateCallback(RxState);
		}
	}
}

/*
 * @func:  		PollRxBuff
 *
 * @brief:		The function to process received messages based on format
 *
 * @param:		port - USART port
 *
 * @retval:		byUartState - USART state
 *
 * @note:		None
 */
uint8_t PollRxBuff (uint8_t port)
{
	uint8_t 	byUartState = (uint8_t) USART_STATE_IDLE;
	uint8_t 	RxBufState = RX_STATE_START_BYTE;
	uint8_t 	byCheckXorRxBuf = 0;

	g_IndexRxBuf = 0;

	while ((port == COM_USART2) && (byUartState == USART_STATE_IDLE))
	{
		// Variable to store value after retrieved from buffer
		uint8_t 	byRxData = g_strRxBuffer[g_IndexRxBuf];

		switch (RxBufState)
		{
			case RX_STATE_START_BYTE:
			{
				if (byRxData == FRAME_START)
				{
					g_IndexRxBuf++;
					byCheckXorRxBuf = CXOR_INIT_VAL;
					RxBufState = RX_STATE_DATA_BYTES;
				}
				else
				{
					byUartState = USART_STATE_EMPTY;	// Buffer EMPTY
				}
			} break;

			case RX_STATE_DATA_BYTES:
			{
				if (g_IndexRxBuf < RX_BUFFER_SIZE)
				{
					if (g_IndexRxBuf > 1)
					{
						byCheckXorRxBuf ^= byRxData;		// Calculate CXOR
					}

					if (g_IndexRxBuf == *(g_strRxBuffer + 1))	// g_IndexRxBuf == FRAME_LENGTH
					{
						RxBufState = RX_STATE_CXOR_BYTE;
					}

					g_IndexRxBuf++;
				}
				else
				{
					RxBufState = RX_STATE_START_BYTE;
					byUartState = USART_STATE_DATA_ERROR;
				}
			} break;

			case RX_STATE_CXOR_BYTE:
			{
				if (byRxData == byCheckXorRxBuf)
				{
					byUartState = USART_STATE_DATA_RECEIVED;
				}
				else
				{
					byUartState = USART_STATE_ERROR;
				}

				RxBufState = RX_STATE_START_BYTE;
			} break;

			default:
				RxBufState = RX_STATE_START_BYTE;
				break;
		}
	}

	return byUartState;
}

/*
 * @func:  		USART_GetFrame
 *
 * @brief:		The function get frame
 *
 * @param:		None
 *
 * @retval:		g_strRxBuffer
 *
 * @note:		None
 */
uint8_t* USART_GetFrame (void)
{
	return g_strRxBuffer;
}

/*
 * @func:  		USART_SendPacket
 *
 * @brief:		The function sends data in the specified format
 *
 * @param[1]:	byNodeId - Byte nodeID of the frame
 * @param[2]:	byEndpoint - Byte endpoint of the frame
 * @param[3]:	byCmdId - Byte cmdId of the frame
 * @param[4]:	byCmdType - Byte cmdType of the frame
 * @param[5]:	pPayload - Byte Data of the frame
 * @param[6]:	byLengthPayload - Data length
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendPacket (EmberNodeId byNodeId,	\
					   uint8_t byEndpoint,		\
					   uint8_t byCmdId, 		\
					   uint8_t byCmdType,		\
					   uint8_t *pPayload,		\
					   uint8_t byLengthPayload)
{
	uint8_t index = 0;
	uint8_t byLength = byLengthPayload + 7;		// nodeid, endpoint, id, type, seq, cxor
	static uint8_t bySequence = CMD_SEQUENCE;
	uint8_t byCXOR = CXOR_INIT_VAL;
	uint8_t Frame[byLength + 2];

	Frame[index++] = FRAME_START;
	Frame[index++] = byLength;
	Frame[index++] = (byNodeId >> 8) & 0xFF;
	Frame[index++] = byNodeId & 0xFF;
	Frame[index++] = byEndpoint;
	Frame[index++] = byCmdId;
	Frame[index++] = byCmdType;

	for (uint8_t i = 0; i < byLengthPayload; i++)
	{
		Frame[index++] = pPayload[i];
	}

	Frame[index++] = bySequence++;

	for (uint8_t j = 2; j < index; j++)
	{
		byCXOR ^= Frame[j];
	}
	Frame[index++] = byCXOR;

	emberSerialWriteData(COM_USART2, Frame, index);
}

/*
 * @func:  		USART_SendNetworkState
 *
 * @brief:		The function sends the network status (to the STM32)
 *
 * @param:		state - Network state
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendNetworkState (uint8_t state)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_NETWORK,
					 CMD_TYPE_RES,
					 &state,
					 1);
}

/*
 * @func:  		USART_SendDeviceJoinNetwork
 *
 * @brief:		The function sends a device joining network message (to the STM32)
 *
 * @param[1]:	nodeId
 * @param[2]:	payload
 * @param[3]:	length
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendDeviceJoinNetwork (EmberNodeId nodeId, uint8_t *payload, uint8_t length)
{
	USART_SendPacket(nodeId,
					 0x01,
					 CMD_ID_DEVICE_JOIN_NETWORK,
					 CMD_TYPE_RES,
					 payload,
					 length);
}

/*
 * @func:  		USART_SendDeviceLeaveNetwork
 *
 * @brief:		The function sends a device leaving network message (to the STM32)
 *
 * @param[1]:	nodeId
 * @param[2]:	payload
 * @param[3]:	length
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendDeviceLeaveNetwork (EmberNodeId nodeId, uint8_t *payload, uint8_t length)
{
	USART_SendPacket(nodeId,
					 0x01,
					 CMD_ID_DEVICE_LEAVE_NETWORK,
					 CMD_TYPE_RES,
					 payload,
					 length);
}

/*
 * @func:  		USART_SendDeviceConnected
 *
 * @brief:		The function sends a message indicating the device is connecting
 * 				to the system (to the STM32)
 *
 * @param[1]:	nodeId
 * @param[2]:	payload
 * @param[3]:	length
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendDeviceConnected (EmberNodeId nodeId, uint8_t *payload, uint8_t length)
{
	USART_SendPacket(nodeId,
					 0x01,
					 CMD_ID_DEVICE_CONNECTED,
					 CMD_TYPE_RES,
					 payload,
					 length);
}

/*
 * @func:  		USART_SendDeviceDisconnected
 *
 * @brief:		The function sends a message indicating the device has lost connection
 * 				to the system (to the STM32)
 *
 * @param[1]:	nodeId
 * @param[2]:	payload
 * @param[3]:	length
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendDeviceDisconnected (EmberNodeId nodeId, uint8_t *payload, uint8_t length)
{
	USART_SendPacket(nodeId,
					 0x01,
					 CMD_ID_DEVICE_DISCONNECTED,
					 CMD_TYPE_RES,
					 payload,
					 length);
}

/*
 * @func:  		USART_SendTempValue
 *
 * @brief:		The function sends the temperature value (to the STM32)
 *
 * @param:		value - Temperature value
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendTempValue (uint8_t value)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_TEMP_SENSOR,
					 CMD_TYPE_RES,
					 (uint8_t*)&value,
					 1);
}

/*
 * @func:  		USART_SendHumiValue
 *
 * @brief:		The function sends the humidity value (to the STM32)
 *
 * @param:		value - Humidity value
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendHumiValue (uint8_t value)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_HUMI_SENSOR,
					 CMD_TYPE_RES,
					 (uint8_t*)&value,
					 1);
}

/*
 * @func:  		USART_SendLightValue
 *
 * @brief:		The function sends the light intensity value (to the STM32)
 *
 * @param:		value - Light value
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendLightValue (uint16_t value)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_LIGHT_SENSOR,
					 CMD_TYPE_RES,
					 (uint8_t*)&value,
					 2);
}

/*
 * @func:  		USART_SendDetectionState
 *
 * @brief:		The function sends the detection status (to the STM32)
 *
 * @param:		state - Detection state
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendDetectionState (uint8_t state)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_DETEC,
					 CMD_TYPE_RES,
					 (uint8_t*)&state,
					 1);
}

/*
 * @func:  		USART_SendLedState
 *
 * @brief:		The function sends the LED On/Off status (to the STM32)
 *
 * @param:		state - Led state
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendLedState (uint8_t state)
{
	USART_SendPacket(NODE_ID_CONTROLLER,
					 0x01,
					 CMD_ID_LED,
					 CMD_TYPE_RES,
					 (uint8_t*)&state,
					 1);
}

/*
 * @func:		USART_SendSyncMessage
 *
 * @brief:		The function sends a synchronization message (to the STM32)
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendSyncMessage (void)
{
	// Send the network status
	EmberNetworkStatus nwkCurStatus = emberAfNetworkState();
	if(nwkCurStatus == EMBER_JOINED_NETWORK)
	{
		USART_SendNetworkState(NETWORK_CREATING);
	}

	// Send the measured values
	USART_SendTempValue(RECEIVE_GetValueTemp());
	USART_SendHumiValue(RECEIVE_GetValueHumi());
	USART_SendLightValue(RECEIVE_GetValueLight());
	USART_SendDetectionState(RECEIVE_GetDetecState());
	USART_SendLedState(RECEIVE_GetLedState());

	// Send the connection status of the devices
	deviceInfo_t *listDevice = DeviceManager_GetDeviceList();
	uint8_t length = DeviceManager_GetDeviceCountToList();

	for (uint8_t i = 0; i < length; i++)
	{
		deviceInfo_t device = *(listDevice + i);
		EmberNodeId nodeId = device.nodeID;
		char* modelId = device.modelID;
		connectionStatus_e connection = device.connectionStatus;

		if (connection == OFFLINE)
		{
			USART_SendDeviceDisconnected(nodeId, (uint8_t*)modelId, strlen(modelId));
		}
		else if (connection == ONLINE)
		{
			USART_SendDeviceConnected(nodeId, (uint8_t*)modelId, strlen(modelId));
		}
	}
}

/*
 * @func:  		USART_ReceivedData
 *
 * @brief:		The function processes incoming messages (from the STM32)
 *
 * @param:		None
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_ReceivedData (void)
{
	uint8_t *frame = g_strRxBuffer;

	if(frame == NULL)
	{
		emberAfCorePrintln("NULL frame");
		return;
	}

	EmberNodeId nodeId1 = DeviceManager_GetNodeID("LED");
	EmberNodeId nodeId2 = DeviceManager_GetNodeID("SENSOR");

	switch (CMD_ID)
	{
		case CMD_ID_LED:
		{
			if (CMD_TYPE == CMD_TYPE_SET)
			{
				if (CMD_DATA_LED == LED_OFF)
				{
					SEND_SetOnOffState(nodeId1, 0x01, LED_OFF);
				}
				else if (CMD_DATA_LED == LED_ON)
				{
					SEND_SetOnOffState(nodeId1, 0x01, LED_ON);
				}
			}

			break;
		}

		case CMD_ID_SET_DISTANCE:
		{
			if (CMD_TYPE == CMD_TYPE_SET)
			{
				if (CMD_DATA_DISTANCE == VALUE_DISTANCE_1)
				{
					SEND_SetValueDistance(nodeId2, 0x01, VALUE_DISTANCE_1);
				}
				else if (CMD_DATA_DISTANCE == VALUE_DISTANCE_2)
				{
					SEND_SetValueDistance(nodeId2, 0x01, VALUE_DISTANCE_2);
				}
				else if (CMD_DATA_DISTANCE == VALUE_DISTANCE_3)
				{
					SEND_SetValueDistance(nodeId2, 0x01, VALUE_DISTANCE_3);
				}
				else if (CMD_DATA_DISTANCE == VALUE_DISTANCE_4)
				{
					SEND_SetValueDistance(nodeId2, 0x01, VALUE_DISTANCE_4);
				}
			}

			break;
		}

		case CMD_ID_SET_LATENCY:
		{
			if (CMD_TYPE == CMD_TYPE_SET)
			{
				if (CMD_DATA_LATENCY == VALUE_LATENCY_1)
				{
					SEND_SetValueLatency(nodeId2, 0x01, VALUE_LATENCY_1);
				}
				else if (CMD_DATA_LATENCY == VALUE_LATENCY_2)
				{
					SEND_SetValueLatency(nodeId2, 0x01, VALUE_LATENCY_2);
				}
				else if (CMD_DATA_LATENCY == VALUE_LATENCY_3)
				{
					SEND_SetValueLatency(nodeId2, 0x01, VALUE_LATENCY_3);
				}
				else if (CMD_DATA_LATENCY == VALUE_LATENCY_4)
				{
					SEND_SetValueLatency(nodeId2, 0x01, VALUE_LATENCY_4);
				}
			}

			break;
		}

		case CMD_ID_SET_SENSITIVITY:
		{
			if (CMD_TYPE == CMD_TYPE_SET)
			{
				if (CMD_DATA_SENSITIVITY == VALUE_SENSITIVITY_1)
				{
					SEND_SetValueSensitivity(nodeId2, 0x01, VALUE_SENSITIVITY_1);
				}
				else if (CMD_DATA_SENSITIVITY == VALUE_SENSITIVITY_2)
				{
					SEND_SetValueSensitivity(nodeId2, 0x01, VALUE_SENSITIVITY_2);
				}
				else if (CMD_DATA_SENSITIVITY == VALUE_SENSITIVITY_3)
				{
					SEND_SetValueSensitivity(nodeId2, 0x01, VALUE_SENSITIVITY_3);
				}
				else if (CMD_DATA_SENSITIVITY == VALUE_SENSITIVITY_4)
				{
					SEND_SetValueSensitivity(nodeId2, 0x01, VALUE_SENSITIVITY_4);
				}
			}

			break;
		}

		case CMD_ID_SET_LIGHT:
		{
			if (CMD_TYPE == CMD_TYPE_SET)
			{
				if (CMD_DATA_LIGHT == VALUE_LIGHT_1)
				{
					SEND_SetValueLight(nodeId2, 0x01, VALUE_LIGHT_1);
				}
				else if (CMD_DATA_LIGHT == VALUE_LIGHT_2)
				{
					SEND_SetValueLight(nodeId2, 0x01, VALUE_LIGHT_2);
				}
				else if (CMD_DATA_LIGHT == VALUE_LIGHT_3)
				{
					SEND_SetValueLight(nodeId2, 0x01, VALUE_LIGHT_3);
				}
				else if (CMD_DATA_LIGHT == VALUE_LIGHT_4)
				{
					SEND_SetValueLight(nodeId2, 0x01, VALUE_LIGHT_4);
				}
			}

			break;
		}

		case CMD_ID_SYNC_MESSAGE:
		{
			if (CMD_TYPE == CMD_TYPE_GET)
			{
				USART_SendSyncMessage();
				emberAfCorePrintln("CMD_ID_SYNC_MESSAGE");
			}

			break;
		}

		default:
			break;
	}
}

/* END FILE */
