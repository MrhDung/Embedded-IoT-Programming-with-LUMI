/*
 * nwk-user.c
 *
 *  Created on: Jul 30, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/App/Network/nwk-user.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
EmberEventControl joinNetworkEventControl;

uint32_t g_timeFindAndJoin = 0;
pNetworkHandle networkHandlerCallback = NULL;

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		NETWORK_Init
 *
 * @brief:		The function initializes the network
 *
 * @params:		callback
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_Init (pNetworkHandle callback)
{
	networkHandlerCallback = callback;
}

/*
 * @func:		NETWORK_FindAndJoin
 *
 * @brief:		The function performs a network scan 2 seconds
 * 				after the network scan function is called
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_FindAndJoin (void)
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}

/*
 * @func:		NETWORK_StopFindAndJoin
 *
 * @brief:		The function stops the network scan process
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_StopFindAndJoin (void)
{
	if(emberAfNetworkState() == EMBER_JOINED_NETWORK)
	{
		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}

/*
 * @func:		joinNetworkEventHandler
 *
 * @brief:		The function joins the network when no network is present
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void joinNetworkEventHandler (void)
{
	emberEventControlSetInactive(joinNetworkEventControl);

	EmberStatus status = emberAfNetworkState();

	if(status == EMBER_NO_NETWORK)
	{
		emberAfPluginNetworkSteeringStart();
		g_timeFindAndJoin++;
		emberEventControlSetDelayMS(joinNetworkEventControl, 5000);
	}
	else if (status == EMBER_JOINED_NETWORK)
	{
		emberAfPluginNetworkSteeringStop();
		g_timeFindAndJoin = 0;
	}
}

/*
 * @func:		emberAfStackStatusCallback
 *
 * @brief:		The function updates the status of the Zigbee network infrastructure
 *
 * @params:		status - Return status
 *
 * @retVal:		true / false
 *
 * @note:		None
 */
boolean emberAfStackStatusCallback (EmberStatus status)
{
	emberAfCorePrintln("nwk-emberAfStackStatusCallback\n");

	if (status == EMBER_NETWORK_UP)
	{
		if (g_timeFindAndJoin > 0)		// Join success
		{
			NETWORK_StopFindAndJoin();

			if(networkHandlerCallback != NULL)
			{
				networkHandlerCallback(NETWORK_JOIN_SUCCESS);
			}
		}
		else	// Has parent already and connected to parent success
		{
			if(networkHandlerCallback != NULL)
			{
				networkHandlerCallback(NETWORK_HAS_PARENT);
			}
		}
	}
	else if (status == EMBER_NETWORK_DOWN)
	{
		EmberNetworkStatus nwkCurrentStatus = emberAfNetworkState();

		if(nwkCurrentStatus == EMBER_NO_NETWORK)
		{
			if(networkHandlerCallback != NULL)
			{
				networkHandlerCallback(NETWORK_OUT_NETWORK);
			}
		}
		else if (nwkCurrentStatus == EMBER_JOINED_NETWORK_NO_PARENT)
		{
			if(networkHandlerCallback != NULL)
			{
			    networkHandlerCallback(NETWORK_LOST_PARENT);
			}
		}
	}

	return false;
}

/* END FILE */
