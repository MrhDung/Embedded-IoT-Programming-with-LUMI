/*
 * TemHumSensor.h
 *
 *  Created on: Jul 24, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_MIDDLE_SENSOR_TEMHUMSENSOR_H_
#define SOURCE_MIDDLE_SENSOR_TEMHUMSENSOR_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include <inttypes.h>
#include "app/framework/include/af.h"
#include "stdbool.h"
#include "em_i2c.h"
#include "Source/Middle/Kalman-Filter/kalman_filter.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define I2C0_SCL_PORT          (gpioPortB)
#define I2C0_SCL_PIN           (0U)
#define I2C0_SDA_PORT          (gpioPortB)
#define I2C0_SDA_PIN           (1U)

/* I2C device address for Si7020 */
#define SI7020_ADDR            0x80

/* Device ID value for Si7020 */
#define SI7020_DEVICE_ID       0x14

/* Si7013 Read Temperature Command */
#define SI7020_READ_TEMP       0xE0

/* Si7013 Read RH Command */
#define SI7020_READ_RH         0xE5

/* SI7013 ID */
#define SI7020_READ_ID_1       0xFC
#define SI7020_READ_ID_2       0xC9

#define CHANGE_VALUE_TEMP		2
#define CHANGE_VALUE_HUMI		2

#define PERIOD_UPDATE_SI7020	1000
#define PERIOD_UPDATE_PC		5000

/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void i2cInit (void);
boolean Si7020_Init (void);
boolean Si7020_Measure (uint32_t *buffer, uint8_t command);
boolean Si7020_MeasureTempAndHumi (uint32_t *humiData, uint32_t *tempData);
void UpdateValueTempHumi (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_MIDDLE_SENSOR_TEMHUMSENSOR_H_ */

/* END FILE */
