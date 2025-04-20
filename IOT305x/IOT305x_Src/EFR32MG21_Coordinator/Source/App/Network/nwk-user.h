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
#include <stdbool.h>
#include "app/framework/include/af.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	NETWORK_NO_NETWORK,
	NETWORK_CREATING,
	NETWORK_OPENING,
	NETWORK_CLOSING
} networkState_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
typedef void (*pNetworkHandle)(networkState_e);

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void NETWORK_Init (pNetworkHandle callback);
bool NETWORK_CreateNetwork (void);
void NETWORK_CreateNetworkForm(EmberPanId panId, uint8_t radioTxPower, uint8_t channel);
bool NETWORK_OpenNetwork (void);
void NETWORK_CloseNetwork (void);
void NETWORK_DeleteNetwork (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_APP_NETWORK_NWK_USER_H_ */

/* END FILE */
