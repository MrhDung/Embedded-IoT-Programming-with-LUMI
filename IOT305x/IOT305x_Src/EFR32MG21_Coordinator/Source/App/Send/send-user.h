/*
 * send-user.h
 *
 *  Created on: Jul 30, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_APP_SEND_SEND_USER_H_
#define SOURCE_APP_SEND_SEND_USER_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define SOURCE_ENDPOINT_PRIMARY			1
#define DESTINATION_ENDPOINT_1			1
#define DESTINATION_ENDPOINT_2			2
#define DESTINATION_ENDPOINT_3			3
#define HC_NETWORK_ADDRESS				0x0000

#define MAX_DATA_COMMAND_SIZE			50

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void SEND_FillBufferGlobalCommand (EmberAfClusterId clusterId,		\
							 	   EmberAfAttributeId attributeId,	\
								   uint8_t commandId,				\
								   uint8_t *pDataRead,				\
								   uint8_t length,					\
								   uint8_t dataType);
void SEND_SendCommandUnicast (EmberNodeId nodeId, uint8_t sourceEp, uint8_t desEp);
void SEND_SetOnOffState (EmberNodeId nodeId, uint8_t sourceEp, uint8_t state);
void SEND_ResendZclCommandViaBinding (uint8_t localEndpoint,	\
									  uint8_t remoteEndpoint,	\
									  bool value, 				\
									  uint16_t nodeID);
void SEND_SetValueDistance (EmberNodeId nodeId, uint8_t sourceEp, uint8_t value);
void SEND_SetValueLatency (EmberNodeId nodeId, uint8_t sourceEp, uint8_t value);
void SEND_SetValueSensitivity (EmberNodeId nodeId, uint8_t sourceEp, uint8_t value);
void SEND_SetValueLight (EmberNodeId nodeId, uint8_t sourceEp, uint16_t value);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_APP_SEND_SEND_USER_H_ */

/* END FILE */
