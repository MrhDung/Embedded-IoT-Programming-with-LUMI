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
#include "Source/Mid/LD2410/ld2410.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
// The size of the array storing data retrieved from the buffer
#define RX_BUFFER_SIZE 					256

#define CMD_START_BYTE_LD2410 			0xFD
#define REPORT_START_BYTE_LD2410		0xF4

#define CMD_ID_NETWORK_CREATING			0x01
#define CMD_ID_NETWORK_OPENING			0x02
#define CMD_ID_NETWORK_CLOSING			0x03
#define CMD_ID_NETWORK_DELETE			0x04
#define CMD_ID_NETWORK_REPORT			0x05

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

#define CMD_ID_DISTANCE					0x06
#define CMD_ID_SENSITIVITY				0x07
#define CMD_ID_LATENCY					0x08
#define CMD_ID_LIGHT					0x09

// Message retrieving information and status of the device from the host computer
#define CMD_TYPE_GET               		0x00

// Message responding with the device's information and status to the host computer
#define CMD_TYPE_RES               	 	0x01

// Message for controlling the device from the host computer
#define CMD_TYPE_SET               	 	0x02

#define CMD_DATA_UNMOTION				0x00
#define CMD_DATA_MOTION					0x01
#define CMD_DATA_LED_OFF				0x00
#define CMD_DATA_LED_ON					0x01

#define CMD_ID							frame[5]
#define CMD_TYPE						frame[6]

#define CMD_DATA_TEMP					frame[7]
#define CMD_DATA_HUMI					frame[7]
#define CMD_DATA_LIGHT					frame[7]
#define CMD_DATA_DETEC					frame[7]
#define	CMD_DATA_LED					frame[7]

#define CMD_DATA_DISTANCE_1				(uint8_t)(1)
#define CMD_DATA_DISTANCE_2				(uint8_t)(2)
#define CMD_DATA_DISTANCE_3				(uint8_t)(3)
#define CMD_DATA_DISTANCE_4				(uint8_t)(4)

#define CMD_DATA_LATENCY_1				(uint8_t)(2)
#define CMD_DATA_LATENCY_2				(uint8_t)(4)
#define CMD_DATA_LATENCY_3				(uint8_t)(6)
#define CMD_DATA_LATENCY_4				(uint8_t)(8)

#define CMD_DATA_SENSITIVITY_1			(uint8_t)(20)
#define CMD_DATA_SENSITIVITY_2			(uint8_t)(40)
#define CMD_DATA_SENSITIVITY_3			(uint8_t)(60)
#define CMD_DATA_SENSITIVITY_4			(uint8_t)(80)

#define CMD_DATA_LIGHT_1				(uint16_t)(10)
#define CMD_DATA_LIGHT_2				(uint16_t)(200)
#define CMD_DATA_LIGHT_3				(uint16_t)(600)
#define CMD_DATA_LIGHT_4				(uint16_t)(1000)

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
	USART_STATE_ACK_RECEIVED,
    USART_STATE_ERROR,
    USART_STATE_RX_TIMEOUT,
} USART_STATE_e;


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
uint8_t USART_GetFrameLength (void);
void USART_SendFrame (uint8_t* frame, uint16_t length);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MID_USART_USART_H_ */

/* END FILE */
