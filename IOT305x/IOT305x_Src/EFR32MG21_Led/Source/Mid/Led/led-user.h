/*
 * led-user.h
 *
 *  Created on: Jul 26, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_MID_LED_LED_USER_H_
#define SOURCE_MID_LED_LED_USER_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include <stdbool.h>
#include "app/framework/include/af.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define LED_RGB_COUNT		2
#define LED_RGB_ELEMENT		3

#define LED_PORT_1			gpioPortA
#define LED_RED_PIN_1		(4U)
#define LED_GREEN_PIN_1		(3U)
#define LED_BLUE_PIN_1		(0U)

#define LED_PORT_2			gpioPortD
#define LED_RED_PIN_2		(0U)
#define LED_GREEN_PIN_2		(1U)
#define LED_BLUE_PIN_2		(2U)

#define LED_RGB_1			{ {LED_PORT_1, LED_RED_PIN_1}, {LED_PORT_1, LED_GREEN_PIN_1}, {LED_PORT_1, LED_BLUE_PIN_1} }
#define LED_RGB_2			{ {LED_PORT_2, LED_RED_PIN_2}, {LED_PORT_2, LED_GREEN_PIN_2}, {LED_PORT_2, LED_BLUE_PIN_2} }


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef struct
{
	GPIO_Port_TypeDef 	port;
	uint16_t 			pin;
} LedStruct_t;

typedef enum
{
	LED_1,
	LED_2
} LedNumber_e;

typedef enum
{
	OFF 	= 0x00,
	RED 	= BIT(0),
	GREEN 	= BIT(1),
	BLUE 	= BIT(2),
	PINK 	= RED | BLUE,
	YELLOW 	= RED | GREEN,
	CYAN 	= GREEN | BLUE,
	WHITE 	= RED | BLUE | GREEN
} LedColor_e;

typedef enum
{
	LED_FREE,
	LED_TOGGLE,
	LED_DIM,
} LedMode_e;

typedef struct
{
	uint16_t 	blinkTime;
	uint32_t 	onTimeMs;
	uint32_t 	offTimeMs;
} LedToggle_t;

typedef struct
{
	uint8_t 	dimLevel;
	bool 		dimHigh;
} LedDim_t;

typedef struct
{
	LedMode_e 		ledMode;
	LedToggle_t		ledToggle;
	LedDim_t 		ledDim;
	LedColor_e 		color;
} LedAction_t;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void led_Init (void);
void led_turnOff (LedNumber_e index);
void led_turnOn (LedNumber_e index, LedColor_e color);
void toggleLed (LedNumber_e ledIndex, LedColor_e color, uint8_t toggleTime,
				uint32_t onTimeMs, uint32_t offTimeMs);
void toggleLedHandler (LedNumber_e ledIndex);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MID_LED_LED_USER_H_ */

/* END FILE */
