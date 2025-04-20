/*
 * USART.h
 *
 *  Created on: Sep 25, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_MID_USART_USART_H_
#define SOURCE_MID_USART_USART_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include "app/framework/include/af.h"
#include "Source/App/Network/nwk-user.h"
#include "Source/App/Receive/receive-user.h"
#include "Source/App/Send/send-user.h"
#include "Source/Mid/Usart/USART.h"
#include "Source/Utils/DeviceManager/DeviceManager.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
// The size of the array storing data retrieved from the buffer
#define RX_BUFFER_SIZE 					256

#define FRAME_START 					0xB1

#define CMD_ID_NETWORK					0x01
#define CMD_ID_CONNECTION				0x02

//#define CMD_ID_NETWORK_CREATING			0x01
//#define CMD_ID_NETWORK_OPENING			0x02
//#define CMD_ID_NETWORK_CLOSING			0x03
//#define CMD_ID_NETWORK_DELETE			0x04
//#define CMD_ID_NETWORK_REPORT			0x05

#define CMD_ID_SYNC_MESSAGE				0x05

#define CMD_ID_DEVICE_JOIN_NETWORK		0x06
#define CMD_ID_DEVICE_LEAVE_NETWORK		0x07
#define CMD_ID_DEVICE_CONNECTED			0x08
#define CMD_ID_DEVICE_DISCONNECTED		0x09
#define CMD_ID_DEVICE_DELETE			0x10

#define CMD_ID_LED						0x81
#define CMD_ID_TEMP_SENSOR 				0x82
#define CMD_ID_HUMI_SENSOR 				0x83
#define CMD_ID_LIGHT_SENSOR 			0x84
#define CMD_ID_DETEC					0x85

#define CMD_ID_SET_DISTANCE				0x06
#define CMD_ID_SET_LATENCY				0x07
#define CMD_ID_SET_SENSITIVITY			0x08
#define CMD_ID_SET_LIGHT				0x09

// Message retrieving information and status of the device from the host computer
#define CMD_TYPE_GET               		0x00

// Message responding with the device's information and status to the host computer
#define CMD_TYPE_RES               	 	0x01

// Message for controlling the device from the host computer
#define CMD_TYPE_SET               	 	0x02

#define CMD_ID							frame[5]
#define CMD_TYPE						frame[6]

#define CMD_DATA_NETWORK				frame[7]
#define CMD_DATA_TEMP					frame[7]
#define CMD_DATA_HUMI					frame[7]
#define CMD_DATA_DETEC					frame[7]
#define	CMD_DATA_LED					frame[7]
#define CMD_DATA_DISTANCE				frame[7]
#define CMD_DATA_LATENCY				frame[7]
#define CMD_DATA_SENSITIVITY			frame[7]
#define CMD_DATA_LIGHT_1				frame[7]
#define CMD_DATA_LIGHT_2				frame[8]
#define CMD_DATA_LIGHT					(CMD_DATA_LIGHT_1 | (CMD_DATA_LIGHT_2 << 8))

#define CMD_SEQUENCE					0x00
#define CXOR_INIT_VAL 					0xFF

#define NODE_ID_CONTROLLER				0x0000
#define NODE_ID_NOT_USE					0xFFEE
#define ENDPOINT_NOT_USE				0xFE

#define USART_PROCESS_SERIAL_INTERVAL	50

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
    RX_STATE_START_BYTE,
    RX_STATE_DATA_BYTES,
    RX_STATE_CXOR_BYTE
} RX_STATE_e;

typedef enum
{
    USART_STATE_IDLE,
	USART_STATE_EMPTY,
    USART_STATE_DATA_RECEIVED,
	USART_STATE_DATA_ERROR,
    USART_STATE_ERROR,
    USART_STATE_RX_TIMEOUT,
} USART_STATE_e;

typedef enum
{
	DETEC_UNMOTION,
	DETEC_MOTION
} detecState_e;

typedef enum
{
	VALUE_DISTANCE_1 = 1,
	VALUE_DISTANCE_2 = 2,
	VALUE_DISTANCE_3 = 3,
	VALUE_DISTANCE_4 = 4
} valueDistance_e;

typedef enum
{
	VALUE_LATENCY_1 = 2,
	VALUE_LATENCY_2 = 4,
	VALUE_LATENCY_3 = 6,
	VALUE_LATENCY_4 = 8
} valueLatency_e;

typedef enum
{
	VALUE_SENSITIVITY_1 = 20,
	VALUE_SENSITIVITY_2 = 40,
	VALUE_SENSITIVITY_3 = 60,
	VALUE_SENSITIVITY_4 = 80
} valueSensitivity_e;

typedef enum
{
	VALUE_LIGHT_1 = 10,
	VALUE_LIGHT_2 = 600,
	VALUE_LIGHT_3 = 1200,
	VALUE_LIGHT_4 = 2000
} valueLight_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
typedef void (*pUsartStateCallback)(USART_STATE_e);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void USART2_Init (pUsartStateCallback callback);
void processSerialHandle (void);
uint8_t PollRxBuff (uint8_t port);
uint8_t* USART_GetFrame (void);
void USART_SendPacket (EmberNodeId byNodeId,	\
					   uint8_t byEndpoint,		\
					   uint8_t byCmdId, 		\
					   uint8_t byCmdType,		\
					   uint8_t *pPayload,		\
					   uint8_t byLengthPayload);
void USART_SendNetworkState (uint8_t state);
void USART_SendDeviceJoinNetwork (EmberNodeId nodeId, uint8_t *payload, uint8_t length);
void USART_SendDeviceLeaveNetwork (EmberNodeId nodeId, uint8_t *payload, uint8_t length);
void USART_SendDeviceConnected (EmberNodeId nodeId, uint8_t *payload, uint8_t length);
void USART_SendDeviceDisconnected (EmberNodeId nodeId, uint8_t *payload, uint8_t length);
void USART_SendTempValue (uint8_t value);
void USART_SendHumiValue (uint8_t value);
void USART_SendLightValue (uint16_t value);
void USART_SendDetectionState (uint8_t state);
void USART_SendLedState (uint8_t state);
void USART_SendSyncMessage (void);
void USART_ReceivedData (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MID_USART_USART_H_ */

/* END FILE */
