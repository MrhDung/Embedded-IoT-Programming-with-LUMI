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
uint32_t g_timeFindAndJoin = 0;
pNetworkHandle g_networkHandlerCallback = NULL;

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
	g_networkHandlerCallback = callback;
}

/*
 * @func:		NETWORK_CreateNetwork
 *
 * @brief:		The function creates a network with random parameters
 *
 * @params:		None
 *
 * @retVal:		True/False
 *
 * @note:		None
 */
bool NETWORK_CreateNetwork (void)
{
	if(emberAfPluginNetworkCreatorStart(true) == EMBER_INVALID_CALL)
	{
		return false;
	}

	if (g_networkHandlerCallback != NULL)
	{
		g_networkHandlerCallback(NETWORK_CREATING);
	}

	return true;
}

/*
 * @func:		NETWORK_CreateNetworkForm
 *
 * @brief:		The function creates a network with user-configured parameters
 *
 * @params[1]:	panId
 * @params[2]:	radioTxPower
 * @params[3]:	channel
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_CreateNetworkForm(EmberPanId panId, uint8_t radioTxPower, uint8_t channel)
{
	emberAfPluginNetworkCreatorNetworkForm(1, panId, radioTxPower, channel);

  	if (g_networkHandlerCallback != NULL)
  	{
  		g_networkHandlerCallback(NETWORK_CREATING);
  	}
}

/*
 * @func:		NETWORK_OpenNetwork
 *
 * @brief:		The function opens the network
 *
 * @params:		None
 *
 * @retVal:		True - Network creation successful
 * 				False - Network creation failed
 *
 * @note:		None
 */
bool NETWORK_OpenNetwork (void)
{
	if(emberAfPluginNetworkCreatorSecurityOpenNetwork() != EMBER_SUCCESS)
	{
		return false;
	}

	if (g_networkHandlerCallback != NULL)
	{
		g_networkHandlerCallback(NETWORK_OPENING);
	}

	return true;
}

/*
 * @func:		NETWORK_CloseNetwork
 *
 * @brief:		The function closes the network
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_CloseNetwork (void)
{
	emberAfPluginNetworkCreatorSecurityCloseNetwork();

	if (g_networkHandlerCallback != NULL)
	{
		g_networkHandlerCallback(NETWORK_CLOSING);
	}
}

/*
 * @func:		NETWORK_DeleteNetwork
 *
 * @brief:		The function delete the network
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void NETWORK_DeleteNetwork (void)
{
	emberLeaveNetwork();

	if (g_networkHandlerCallback != NULL)
	{
		g_networkHandlerCallback(NETWORK_NO_NETWORK);
	}
}

/* END FILE */
