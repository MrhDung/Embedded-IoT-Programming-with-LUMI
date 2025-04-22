/*
 * ldr-user.c
 *
 *  Created on: Jul 24, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Mid/LDR/ldr-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl LdrEventControl;

static pLDRcallbackFunction g_LDRcallback;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		LDR_Init
 *
 * @brief:		Initializes LDR component
 *
 * @params:		callback
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LDR_Init (pLDRcallbackFunction callback)
{
  // Declare initial structures
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

  // Enable IADC clock
  CMU_ClockEnable(cmuClock_IADC0, true);

  // Reset IADC to reset configuration in case it has been modified
  IADC_reset(IADC0);

  // Configure IADC clock source for use while in EM2
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Configuration 0 is used by both scan and single conversions by default
  // Use unbuffered AVDD as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                    CLK_ADC_FREQ,
                                                                    0,
                                                                    iadcCfgModeNormal,
                                                                    init.srcClkPrescale);

  // Set oversampling rate to 32x
  // resolution formula res = 11 + log2(oversampling * digital averaging)
  // in this case res = 11 + log2(32*1) = 16
  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed32x;

  // Single initialization
  initSingle.dataValidLevel = _IADC_SINGLEFIFOCFG_DVL_VALID1;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = iadcPosInputPortCPin5;
  initSingleInput.negInput = iadcNegInputGnd;

  // Initialize IADC
  // Note oversampling and digital averaging will affect the offset correction
  // This is taken care of in the IADC_init() function in the emlib
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Scan
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_BUS |= GPIO_CDBUSALLOC_CDODD0_ADC0;  //IADC_INPUT_BUSALLOC

  g_LDRcallback = callback;

  emberEventControlSetActive(LdrEventControl);
}

/*
 * @func:		LDR_Read
 *
 * @brief:		Polling read light intensity value
 *
 * @params:		luxValue - Pointer to store lux value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LDR_Read (uint32_t* luxValue)
{
	uint32_t registor;
	IADC_Result_t iadcResult;

	// Enable ADC0 to start single conversion
	IADC_command(IADC0, iadcCmdStartSingle);

	// Read data from the FIFO
	while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK | _IADC_STATUS_SINGLEFIFODV_MASK)) != _IADC_STATUS_SINGLEFIFODV_MASK);

	// Get the conversion data of ADC0
	iadcResult = IADC_pullSingleFifoResult(IADC0);

	// registor  = 10K*ADC / (4095 -ADC)
	registor= 10000*(3300 - iadcResult.data)/iadcResult.data;

	*luxValue = 316*pow(10,5)*pow(registor,-1.4);
}

/*
 * @func:		LdrEventHandler
 *
 * @brief:		The function executes events based on the measured light intensity value
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LdrEventHandler (void)
{
	emberEventControlSetInactive(LdrEventControl);

	if(g_LDRcallback != NULL)
	{
		g_LDRcallback();
	}

	emberEventControlSetDelayMS(LdrEventControl, 1000 * 60);
}

/* END_FILE */
