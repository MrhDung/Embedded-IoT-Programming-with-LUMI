/*
 * receive-user.h
 *
 *  Created on: Jul 30, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_APP_RECEIVE_RECEIVE_USER_H_
#define SOURCE_APP_RECEIVE_RECEIVE_USER_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "Source/App/Send/send-user.h"
#include "Source/Mid/LD2410/ld2410.h"
#include "Source/Mid/Led/led-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define ENDPOINT_1		1

#define ZCL_LD2410_DISTANCE_CLUSTER_ID        		0x0C01
#define ZCL_LD2410_DISTANCE_VALUE_ATTRIBUTE_ID		0x0001

#define ZCL_LD2410_LATENCY_CLUSTER_ID               0x0C02
#define ZCL_LD2410_LATENCY_VALUE_ATTRIBUTE_ID       0x0001

#define ZCL_LD2410_SENSITIVITY_CLUSTER_ID           0x0C03
#define ZCL_LD2410_SENSITIVITY_VALUE_ATTRIBUTE_ID	0x0001

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	DEVICE_JOIN_NETWORK,
	DEVICE_CONNECTED,
	DEVICE_DISCONNECTED,
	LEVEL_CONTROL,
	DEVICE_LEAVE_NETWORK
} RECEIVE_CMD_ID_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
typedef void (*pReceiveHandler)(EmberNodeId nodeID,				\
								RECEIVE_CMD_ID_e receiveId,		\
								uint8_t *payload, 				\
								uint8_t payloadLength);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void RECEIVE_Init (pReceiveHandler callback);
uint8_t RECEIVE_SetValueLatency (void);
uint16_t RECEIVE_SetValueLight (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_APP_RECEIVE_RECEIVE_USER_H_ */

/* END FILE */
