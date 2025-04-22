/*
 * main_Asm3.h
 *
 *  Created on: Aug 8, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_APP_MAIN_MAIN_ASM3_H_
#define SOURCE_APP_MAIN_MAIN_ASM3_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "Source/App/Network/nwk-user.h"
#include "Source/App/Receive/receive-user.h"
#include "Source/App/Send/send-user.h"
#include "Source/Mid/Button/button-user.h"
#include "Source/Mid/Kalman-Filter/kalman_filter.h"
#include "Source/Mid/LDR/ldr-user.h"
#include "Source/Mid/Led/led-user.h"
#include "Source/Mid/Timer/timer-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define ENDPOINT_1					1
#define ENDPOINT_2					2
#define ENDPOINT_3					3

#define THRESHOLD_LUX_REPORT		30
#define THRESHOLD_LUX_CONTROL_LED	500

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
} e_MainState;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void USER_NetworkHandle (e_NetworkState networkResult);
void USER_ButtonPressHandle (uint8_t button, uint8_t pressCount);
void USER_ButtonHoldHandle (uint8_t button, uint8_t holdCount);
void USER_ReceiveOnOffClusterHandle (EmberAfClusterCommand* cmd);
void USER_ReceiveLeaveHandle (EmberNodeId nodeId, e_RECEIVE_CMD_ID receiveId);
void USER_LdrUpdateValueLight (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_APP_MAIN_MAIN_ASM3_H_ */

/* END FILE */
