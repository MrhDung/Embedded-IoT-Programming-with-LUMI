/*
 * nwk-user.h
 *
 *  Created on: Jul 30, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_APP_NETWORK_NWK_USER_H_
#define SOURCE_APP_NETWORK_NWK_USER_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	NETWORK_CREATING,
	NETWORK_OPENING,
	NETWORK_CLOSING,
	NETWORK_JOIN_SUCCESS,
	NETWORK_JOIN_FAIL,
	NETWORK_HAS_PARENT,
	NETWORK_OUT_NETWORK,
	NETWORK_LOST_PARENT,
} NetworkState_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
typedef void (*pNetworkHandle)(NetworkState_e);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void NETWORK_Init (pNetworkHandle callback);
void NETWORK_CreateNetwork (void);
void NETWORK_OpenNetwork (void);
void NETWORK_CloseNetwork (void);
void NETWORK_FindAndJoin (void);
void NETWORK_StopFindAndJoin (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_APP_NETWORK_NWK_USER_H_ */

/* END FILE */
