/*
 * ldr-user.c
 *
 *  Created on: Jul 24, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Middle/LDR/ldr-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl getADCPollingEventControl;

static uint32_t g_light = 0;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		LDRInit
 *
 * @brief:		Initializes LDR component
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void LDRInit (void)
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

  emberEventControlSetActive(getADCPollingEventControl);
}

/*
 * @func:		readLDR
 *
 * @brief:		Polling read light intensity value
 *
 * @params:		luxValue - Pointer to store lux value
 *
 * @retVal:		None
 *
 * @note:		None
 */
void readLDR (uint32_t* luxValue)
{
	uint32_t registor;
	IADC_Result_t iadcResult;

	// Enable ADC0 to start single conversion
	IADC_command(IADC0, iadcCmdStartSingle);

	// Read data from the FIFO
	while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK | _IADC_STATUS_SINGLEFIFODV_MASK)) != _IADC_STATUS_SINGLEFIFODV_MASK);

	// Get the conversion data of ADC0
	iadcResult = IADC_pullSingleFifoResult(IADC0);

	registor= 10000*(3300 - iadcResult.data)/iadcResult.data;    // registor  = 10K*ADC / (4095 -ADC)

	*luxValue = 316*pow(10,5)*pow(registor,-1.4);
}

/*
 * @func:		UpdateValueLight
 *
 * @brief:		The function to update light intensity value
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void UpdateValueLight (void)
{
	static float luxCurEst = 0;
	static uint32_t countLight = 0;
	uint32_t currentLight;

	countLight++;

	// Get light intensity value
	readLDR(&currentLight);

	// Update light intensity value every 1 second
	if(countLight % 1000 == 0)
	{
		g_light = currentLight;
		g_light = (uint32_t)KalmanFilter(&luxCurEst, (float)g_light, MEASURE_NOISE_INIT, PROCESS_NOISE_INIT);
	}

	// Update light intensity value to PC every 5 seconds
	if (countLight >= PERIOD_UPDATE_PC / 1000)
	{
		g_light = currentLight;
		g_light = (uint32_t)KalmanFilter(&luxCurEst, (float)g_light, MEASURE_NOISE_INIT, PROCESS_NOISE_INIT);
		emberAfCorePrintln("Light: %"PRIu32" Lux", g_light);

		countLight = 0;
	}

	// Update light intensity value immediately when a great change occurs
	if (((currentLight > g_light) && (currentLight - g_light >= CHANGE_VALUE_LIGHT)) ||
		((currentLight < g_light) && (g_light - currentLight >= CHANGE_VALUE_LIGHT)))
	{
		g_light = currentLight;
		g_light = (uint32_t)KalmanFilter(&luxCurEst, (float)g_light, MEASURE_NOISE_INIT, PROCESS_NOISE_INIT);
		emberAfCorePrintln("Light value changes greatly: %"PRIu32" Lux", g_light);
	}
}

/*
 * @func:		getADCPollingEventHandler
 *
 * @brief:		Poilling read ADC value
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void getADCPollingEventHandler (void)
{
	emberEventControlSetInactive(getADCPollingEventControl);

	UpdateValueLight();

	IADC_clearInt(IADC0, IADC_IF_SCANFIFODVL);

	emberEventControlSetDelayMS(getADCPollingEventControl, 1000);
}

/* END_FILE */
