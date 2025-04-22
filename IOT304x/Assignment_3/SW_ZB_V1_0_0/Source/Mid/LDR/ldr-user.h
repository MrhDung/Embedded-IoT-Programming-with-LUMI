/*
 * ldr-user.h
 *
 *  Created on: Jul 24, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_MIDDLE_LDR_LDR_USER_H_
#define SOURCE_MIDDLE_LDR_LDR_USER_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include <inttypes.h>
#include "math.h"
#include "app/framework/include/af.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "em_iadc.h"
#include "em_bus.h"
#include "Source/Mid/Kalman-Filter/kalman_filter.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
// Set HFRCOEM23 to lowest frequency (1MHz)
#define HFRCOEM23_FREQ          cmuHFRCOEM23Freq_1M0Hz

// Set CLK_ADC to 10kHz (this corresponds to a sample rate of 1ksps)
#define CLK_SRC_ADC_FREQ        1000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ            10000   // CLK_ADC

// When changing GPIO port/pins above, make sure to change xBUSALLOC macro's accordingly.
#define IADC_INPUT_BUS          CDBUSALLOC
#define IADC_INPUT_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0

#define CHANGE_VALUE_LIGHT		20

#define PERIOD_UPDATE_LDR		1000
#define PERIOD_UPDATE_PC		5000

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
typedef void (*pLDRcallbackFunction) (void);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void LDR_Init (pLDRcallbackFunction callback);
void LDR_Read (uint32_t* luxValue);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MIDDLE_LDR_LDR_USER_H_ */

/* END FILE */
