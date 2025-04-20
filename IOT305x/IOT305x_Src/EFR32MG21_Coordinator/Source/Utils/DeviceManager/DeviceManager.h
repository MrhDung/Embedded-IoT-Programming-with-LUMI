/*
 * DeviceManager.h
 *
 *  Created on: Oct 30, 2024
 *      Author: Mr.hDung
 */

#ifndef SOURCE_UTILS_DEVICEMANAGER_DEVICEMANAGER_H_
#define SOURCE_UTILS_DEVICEMANAGER_DEVICEMANAGER_H_

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "app/framework/include/af.h"
#include "Source/Utils/CoordinatorTokens.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/
#define DEVICE_NUMBER_MAX			20
#define DEVICE_INDEX_INVALID		0xFF
#define DEVICE_NODEID_INVALID		0xFF


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/
typedef enum
{
	LED_OFF,
	LED_ON
} ledState_e;

/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/


/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/
void DeviceManager_AddDeviceToList (EmberNodeId nodeId, char *modelId);
void DeviceManager_RemoveDeviceFromList (EmberNodeId nodeId);
void DeviceManager_RemoveDeviceFromNetwork (EmberNodeId nodeId);
uint8_t DeviceManager_CheckDeviceFromList (EmberNodeId nodeId);
uint8_t DeviceManager_GetDeviceIndexByModelId (const char *modelId);
EmberNodeId DeviceManager_GetNodeID (char *modelId);
char* DeviceManager_GetModelID (EmberNodeId nodeId);
deviceInfo_t* DeviceManager_GetDeviceList (void);
uint8_t DeviceManager_GetDeviceCountToList (void);
bool DeviceManager_SetDeviceOnline (EmberNodeId nodeId);
bool DeviceManager_SetDeviceOffline (EmberNodeId nodeId);
bool DeviceManager_SetStateLedOn (EmberNodeId nodeId);
bool DeviceManager_SetStateLedOff (EmberNodeId nodeId);
void DeviceManager_UpdateLastTime (EmberNodeId nodeId, uint32_t time);
void DeviceManager_PrintDeviceList (void);

/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/


#endif /* SOURCE_UTILS_DEVICEMANAGER_DEVICEMANAGER_H_ */

/* END FILE */
