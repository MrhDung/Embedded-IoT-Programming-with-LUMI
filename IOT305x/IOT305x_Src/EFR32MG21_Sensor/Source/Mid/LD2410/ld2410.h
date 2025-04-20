/*
 * ld2410.h
 *
 *  Created on: Sep 8, 2023
 *      Author: Administrator
 */

#ifndef SOURCE_MID_LD2410_LD2410_H_
#define SOURCE_MID_LD2410_LD2410_H_

/******************************************************************************/
/*                              INCLUDEs                                      */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "Source/App/Send/send-user.h"
#include "Source/App/Receive/receive-user.h"
#include "Source/Mid/LDR/ldr-user.h"
#include "Source/Mid/Led/led-user.h"
#include "Source/Mid/Usart/USART.h"

/******************************************************************************/
/*                               DEFINEs                              		  */
/******************************************************************************/
#define LD2410_PORT						gpioPortC
#define LD2410_PIN						(3U)
#define LD2410_EXTI_CHANNEL				(LD2410_PIN)

#define ENDPOINT_3						3

#define CONFIG_ENABLE					1
#define CONFIG_END						0

#define LD2410_SKIP_HEADER				4
#define LD2410_INDEX_START_FRAME_ACK	18

#define LD2410_DELAY_MAX				2000

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	LD2410_STATE_DEBOUNCE,
	LD2410_STATE_WAIT_UNMOTION,
	LD2410_STATE_UNMOTION
} LD2410_State_e;

typedef enum
{
	LD2410_UNMOTION,
	LD2410_MOTION
} LD2410_Action_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
//typedef void (*pLd2410ActionHandle)(uint8_t);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void LD2410_Init (void);
void LD2410_Enable (boolean en);
boolean LD2410_isMotionSignal (void);
void LD2410_INTSignalHandle (uint8_t intNo);
void LD2410_Handle (LD2410_Action_e action);
void LD2410_HeaderSendAndACK (void);
void LD2410_EndFrameSendAndACK (void);
void LD2410_FrameSetConfiguration (uint8_t enable);
void LD2410_SetValueDistanceLatency (uint8_t valueMovingDistance,	\
									 uint8_t valueRestingDistance,	\
									 uint8_t valueLatency);
void LD2410_SetValueSensitivity (uint8_t valueMotionSensitivity, uint8_t valueStaticSensitivity);
void LD2410_ReadParameterCommand (void);
uint8_t* LD2410_FrameAckDistanceLatency (void);
uint8_t* LD2410_FrameAckSensitivity (void);
uint8_t LD2410_CompareFrameAckDistanceLatencyWithBuffer (void);
uint8_t LD2410_CompareFrameAckSensitivityWithBuffer (void);
void LD2410_SetValueSuccess (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MID_LD2410_LD2410_H_ */

/* END FILE */
