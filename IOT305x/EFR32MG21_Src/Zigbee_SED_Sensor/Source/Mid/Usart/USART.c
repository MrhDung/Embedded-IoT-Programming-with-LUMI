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

uint8_t 	g_byUartState = (uint8_t)USART_STATE_IDLE;
uint8_t 	g_RxBufState = RX_STATE_START_BYTE;
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

	uint8_t 	dataByte = 0;
	uint16_t 	bytesReceived = 0;        // Variable storing the number of received bytes

	// Get the number of received bytes
	uint16_t numOfByteAvail = emberSerialReadAvailable(COM_USART2);

	if(numOfByteAvail > 0)
	{
		// Find the start byte (0xFD)
		while(dataByte != 0xFD && numOfByteAvail > 0)
		{
			emberSerialReadByte(COM_USART2, &dataByte);
			numOfByteAvail--;
		}

		if(numOfByteAvail > 0 && dataByte == 0xFD)
		{
			g_strRxBuffer[bytesReceived++] = dataByte;		// Store the received byte in the buffer

			while (numOfByteAvail > 0 && bytesReceived < RX_BUFFER_SIZE)
			{
				emberSerialReadByte(COM_USART2, &dataByte);
				g_strRxBuffer[bytesReceived++] = dataByte;
				numOfByteAvail--;
			}

			processSerialHandle();
		}
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

	if (g_usart2StateCallback != NULL)
	{
		g_usart2StateCallback(RxState);
	}
}

/*
 * @func:  		PollRxBuff
 *
 * @brief:		The function to process received messages based on format
 *
 * @param:		port - USART port
 *
 * @retval:		g_byUartState - USART state
 *
 * @note:		None
 */
uint8_t PollRxBuff (uint8_t port)
{
	g_byUartState = USART_STATE_IDLE;
	g_IndexRxBuf = LD2410_INDEX_START_FRAME_ACK;

	while ((port == COM_USART2) && (g_byUartState == USART_STATE_IDLE))
	{
		// Variable to store value after retrieved from buffer
		uint8_t byRxData = g_strRxBuffer[g_IndexRxBuf];

		switch (g_RxBufState)
		{
			case RX_STATE_START_BYTE:
			{
				if (byRxData == CMD_START_BYTE_LD2410)
				{
					g_IndexRxBuf++;
					g_RxBufState = RX_STATE_DATA_BYTES;
				}
				else
				{
					g_byUartState = USART_STATE_EMPTY;	// Buffer EMPTY
				}

				break;
			}

			case RX_STATE_DATA_BYTES:
			{
				if (g_IndexRxBuf < RX_BUFFER_SIZE)
				{
					if(g_IndexRxBuf > LD2410_INDEX_START_FRAME_ACK + LD2410_SKIP_HEADER &&
					   g_strRxBuffer[g_IndexRxBuf - 4] == 0x04 &&
					   g_strRxBuffer[g_IndexRxBuf - 3] == 0x03 &&
					   g_strRxBuffer[g_IndexRxBuf - 2] == 0x02 &&
					   g_strRxBuffer[g_IndexRxBuf - 1] == 0x01)
					{
						g_RxBufState = RX_STATE_START_BYTE;
						g_byUartState = USART_STATE_ACK_RECEIVED;
					}

					g_IndexRxBuf++;
				}
				else
				{
					g_RxBufState = RX_STATE_START_BYTE;
					g_byUartState = USART_STATE_DATA_ERROR;
				}

				break;
			}

			default:
				g_RxBufState = RX_STATE_START_BYTE;
				break;
		}
	}

	return g_byUartState;
}

/*
 * @func:  		USART_GetFrame
 *
 * @brief:		The function retrieves the received frame
 *
 * @param:		None
 *
 * @retval:		Received frame (Start from the ACK frame)
 *
 * @note:		None
 */
uint8_t* USART_GetFrame (void)
{
	return g_strRxBuffer + LD2410_INDEX_START_FRAME_ACK;
}

/*
 * @func:  		USART_GetFrameLength
 *
 * @brief:		The function get frame length
 *
 * @param:		None
 *
 * @retval:		g_strRxBuffer
 *
 * @note:		None
 */
uint8_t USART_GetFrameLength (void)
{
	return g_strRxBuffer[LD2410_INDEX_START_FRAME_ACK + LD2410_SKIP_HEADER];
}

/*
 * @func:  		USART_SendFrame
 *
 * @brief:		The function send frame
 *
 * @param[1]:	frame
 * @param[2]:	length
 *
 * @retval:		None
 *
 * @note:		None
 */
void USART_SendFrame (uint8_t* frame, uint16_t length)
{
	emberSerialWriteData(COM_USART2, frame, length);
}

/* END FILE */
