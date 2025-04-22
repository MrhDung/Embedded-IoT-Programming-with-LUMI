/*
 * button-user.h
 *
 *  Created on: Jul 21, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_MID_BUTTON_BUTTON_USER_H_
#define SOURCE_MID_BUTTON_BUTTON_USER_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "Source/Mid/Timer/timer-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define SW1							BUTTON0
#define SW2							BUTTON1


#define BUTTON_NUMBER				(2U)
#define BUTTON_PRESS_SCHEMATIC		(0U)
#define BUTTON_PIN_TO_INDEX			0xFF
#define BUTTON_INIT					{{BSP_BUTTON0_PORT, BSP_BUTTON0_PIN}, \
								 	 {BSP_BUTTON1_PORT, BSP_BUTTON1_PIN}}

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	release,
	press_1,
	press_2,
	press_3,
	press_4,
	press_5
} e_pressEvent;

typedef enum
{
	hold_1s = 1,
	hold_2s,
	hold_3s,
	hold_4s,
	hold_5s
} e_holdEvent;

typedef struct
{
	GPIO_Port_TypeDef 	port;
	uint8_t 			pin;
	uint32_t 			timePress;
	uint32_t 			timeRelease;
	uint16_t 			countButton;
	uint8_t 			holdFlag;
} s_Button;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
typedef void (*pPressButtonCallback)(uint8_t, uint8_t);
typedef void (*pHoldButtonCallback)(uint8_t, uint8_t);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void Button_Init (pPressButtonCallback pressCallback, pHoldButtonCallback holdCallback);
uint8_t GetButtonIndex (uint8_t pin);


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MID_BUTTON_BUTTON_USER_H_ */

/* END FILE */
