/*
 * main-displaycontrol.h
 *
 *  Created on: Dec 2, 2024
 *      Author: Mr.hDung
 */

#ifndef MAIN_DISPLAYCONTROL_H_
#define MAIN_DISPLAYCONTROL_H_

/****************************************************************************************/
/*                                      INCLUDEs                                		*/
/****************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "system_stm32f4xx.h"
#include "timer.h"
#include "eventbutton.h"
#include "button.h"
#include "lcd.h"
#include "usart.h"

/****************************************************************************************/
/*                                       DEFINEs                        			    */
/****************************************************************************************/


/****************************************************************************************/
/*                                  STRUCTs AND ENUMs                           		*/
/****************************************************************************************/
typedef enum
{
	EVENT_EMPTY,
	EVENT_APP_INIT,
	EVENT_APP_FLUSHMEM_READY
} event_api_e, *event_api_p;

typedef enum
{
	STATE_APP_STARTUP,
	STATE_APP_IDLE,
	STATE_APP_RESET
} state_app_e;

/****************************************************************************************/
/*                                  GLOBAL VARIABLEs                 					*/
/****************************************************************************************/


/****************************************************************************************/
/*                                 FUNCTIONs PROTOTYPE                                  */
/****************************************************************************************/
void Main_AppInitManager (void);
void Main_AppStateManager (uint8_t event);
void Main_SetStateApp (state_app_e state);
state_app_e Main_GetStateApp (void);
void Main_DeviceStateMachine (uint8_t event);
void Main_Usart2RxHandle (USART_STATE_e UsartStateRx);

/****************************************************************************************/
/*                                      FUNCTIONs                                       */
/****************************************************************************************/


#endif /* MAIN_DISPLAYCONTROL_H_ */

/* END FILE */
