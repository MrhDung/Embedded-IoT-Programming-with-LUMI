/*
 * kalman_filter.c
 *
 *  Created on: Jul 24, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Middle/Kalman-Filter/kalman_filter.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		KalmanFilter
 *
 * @brief:		Initializes Kalman Filter
 *
 * @params[1]:	fCurEst - Pointer to the estimated value of the previous state
 * @params[2]:	fMeasureValue - Measured value from the sensor
 * @params[3]:	fMeasureNoise - Measured noise
 * @params[4]:	fProcessNoise - Process noise
 *
 * @retVal:		fEstValue - Filtered value
 *
 * @note:		None
 */
float KalmanFilter (float* fCurEst, float fMeasureValue, float fMeasureNoise, float fProcessNoise)
{
	const float R = fMeasureNoise * fMeasureNoise;
	const float Q = fProcessNoise * fProcessNoise;
//	static float fCurEstVari = 0;
	static float fEstValue = 50;

	float fNewEstVari  = *fCurEst + Q;
	float fKGain = fNewEstVari / (fNewEstVari + R);

	fEstValue = fEstValue + fKGain * (fMeasureValue - fEstValue);
	*fCurEst = (1 - fKGain) * fNewEstVari;

	return fEstValue;
}

/* END FILE */
