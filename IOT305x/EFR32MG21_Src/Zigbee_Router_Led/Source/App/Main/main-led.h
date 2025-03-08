/*
 * main-led.h
 *
 *  Created on: Dec 1, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_APP_MAIN_MAIN_LED_H_
#define SOURCE_APP_MAIN_MAIN_LED_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "app/framework/include/af.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "Source/App/Network/nwk-user.h"
#include "Source/App/Receive/receive-user.h"
#include "Source/App/Send/send-user.h"
#include "Source/Mid/Button/button-user.h"
#include "Source/Mid/Led/led-user.h"
#include "Source/Mid/Timer/timer-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define	CMD_ENDPOINT		g_frame[4]
#define CMD_ID				g_frame[5]
#define CMD_TYPE			g_frame[6]

#define CMD_DATA_TEMP		g_frame[7]
#define CMD_DATA_HUMI		g_frame[7]
#define CMD_DATA_LIGHT		g_frame[7]
#define CMD_DATA_DETEC		g_frame[7]
#define	CMD_DATA_LED		g_frame[7]

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	POWER_ON_STATE,
	REPORT_STATE,
	IDLE_STATE,
	LEAVE_NETWORK,
	REBOOT_STATE
} MainState_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void Main_NetworkHandle (NetworkState_e networkResult);
void Main_ButtonPressHandle (uint8_t button, uint8_t pressCount);
void Main_ButtonHoldHandle (uint8_t button, uint8_t holdCount);
void Main_ReceiveLeaveHandle (EmberNodeId nodeId, 			\
							  RECEIVE_CMD_ID_e receiveId, 	\
							  uint8_t *payload, 			\
							  uint8_t payloadLength);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_APP_MAIN_MAIN_LED_H_ */

/* END FILE */
