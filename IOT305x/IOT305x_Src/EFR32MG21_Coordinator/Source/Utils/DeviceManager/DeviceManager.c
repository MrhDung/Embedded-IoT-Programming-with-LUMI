/*
 * DeviceManager.c
 *
 *  Created on: Oct 30, 2024
 *      Author: Mr.hDung
 */

/******************************************************************************/
/*                                INCLUDEs                                    */
/******************************************************************************/
#include "Source/Utils/DeviceManager/DeviceManager.h"

/******************************************************************************/
/*                                 DEFINEs                                    */
/******************************************************************************/


/******************************************************************************/
/*                            STRUCTs AND ENUMs                               */
/******************************************************************************/


/******************************************************************************/
/*                       EVENTs AND GLOBAL VARIABLEs                          */
/******************************************************************************/
static uint8_t g_index = 0;
deviceInfo_t g_deviceList[DEVICE_NUMBER_MAX];

/******************************************************************************/
/*                           FUNCTIONs  PROTOTYPE                             */
/******************************************************************************/


/******************************************************************************/
/*                               FUNCTIONs                              	  */
/******************************************************************************/
/*
 * @func:		DeviceManager_AddDeviceToList
 *
 * @brief:		The function adds a device to the list
 *
 * @params[1]:	nodeId
 * @params[2]:	modelId
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_AddDeviceToList (EmberNodeId nodeId, char *modelId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	// Check if the array is full (If the array is full, no new devices can be added)
	if(g_index >= DEVICE_NUMBER_MAX)
	{
		return;
	}

	// Check if the device already exists in the list
	uint8_t indexDevice = DeviceManager_CheckDeviceFromList(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		return;		// The device already exists, not adding
	}

	// Create a new device
	deviceInfo_t newDevice;

	newDevice.nodeID = nodeId;
	newDevice.connectionStatus = ONLINE;

	strncpy(newDevice.modelID, modelId, sizeof(newDevice.modelID) - 1);
	newDevice.modelID[sizeof(newDevice.modelID) - 1] = '\0';

	// Add the new device to the list
	g_deviceList[g_index] = newDevice;
	g_index++;

	halCommonSetToken(TOKEN_DEVICE_INFO, g_deviceList);
}

/*
 * @func:		DeviceManager_RemoveDeviceFromList
 *
 * @brief:		The function removes a device from the list
 *
 * @params:		nodeId
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_RemoveDeviceFromList (EmberNodeId nodeId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	uint8_t indexDevice = DeviceManager_CheckDeviceFromList(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
        for(uint8_t i = indexDevice; i < g_index - 1; i++)
        {
        	g_deviceList[i] = g_deviceList[i + 1];
        }

        g_index--;
	}

	halCommonSetToken(TOKEN_DEVICE_INFO, g_deviceList);
}

/*
 * @func:		DeviceManager_RemoveDeviceFromNetwork
 *
 * @brief:		The function removes a device from the network
 *
 * @params:		nodeId
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_RemoveDeviceFromNetwork (EmberNodeId nodeId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	emberSendZigbeeLeave(nodeId, EMBER_ZIGBEE_LEAVE_WITHOUT_REJOIN);
	DeviceManager_RemoveDeviceFromList(nodeId);

	halCommonSetToken(TOKEN_DEVICE_INFO, g_deviceList);
}

/*
 * @func:		DeviceManager_CheckDeviceFromList
 *
 * @brief:		The function checks whether the device is in the list
 *
 * @params:		nodeId
 *
 * @retVal:		If the device already exists, return its position in the list
 * 				If the device does not exist in the list, return DEVICE_INDEX_INVALID
 *
 * @note:		None
 */
uint8_t DeviceManager_CheckDeviceFromList (EmberNodeId nodeId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	for(uint8_t i = 0; i < g_index; i++)
	{
		if(nodeId == g_deviceList[i].nodeID)
		{
			return i;
		}
	}

	return DEVICE_INDEX_INVALID;
}

/*
 * @func:		DeviceManager_GetDeviceIndexByModelId
 *
 * @brief:		The function retrieves a device from the list based on the model ID
 *
 * @params:		modelId
 *
 * @retVal:		If the device already exists, return its position in the list
 * 				If the device does not exist in the list, return DEVICE_INDEX_INVALID
 *
 * @note:		None
 */
uint8_t DeviceManager_GetDeviceIndexByModelId (const char *modelId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	for(uint8_t i = 0; i < g_index; i++)
	{
		if(strncmp(g_deviceList[i].modelID, modelId, strlen(modelId)) == 0)
		{
			return i;
		}
	}

	return DEVICE_INDEX_INVALID;
}

/*
 * @func:		DeviceManager_GetNodeID
 *
 * @brief:		The function retrieves the nodeID of the device
 *
 * @params:		modelId
 *
 * @retVal:		nodeID
 *
 * @note:		None
 */
EmberNodeId DeviceManager_GetNodeID (char *modelId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	for(uint8_t i = 0; i < g_index; i++)
	{
		if(strncmp(g_deviceList[i].modelID, modelId, strlen(modelId)) == 0)
		{
			return g_deviceList[i].nodeID;
		}
	}

	return DEVICE_NODEID_INVALID;
}

/*
 * @func:		DeviceManager_GetModelID
 *
 * @brief:		The function retrieves the model ID of the device
 *
 * @params:		nodeId
 *
 * @retVal:		modelID
 *
 * @note:		None
 */
char* DeviceManager_GetModelID (EmberNodeId nodeId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	for(uint8_t i = 0; i < g_index; i++)
	{
		if(nodeId == g_deviceList[i].nodeID)
		{
			return g_deviceList[i].modelID;
		}
	}

	return NULL;
}

/*
 * @func:		DeviceManager_GetDeviceList
 *
 * @brief:		The function retrieves device information from the list
 *
 * @params:		None
 *
 * @retVal:		g_deviceList
 *
 * @note:		None
 */
deviceInfo_t* DeviceManager_GetDeviceList (void)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);
	return g_deviceList;
}

/*
 * @func:		DeviceManager_GetDeviceCountToList
 *
 * @brief:		The function retrieves the number of devices stored in the list
 *
 * @params:		None
 *
 * @retVal:		g_index
 *
 * @note:		None
 */
uint8_t DeviceManager_GetDeviceCountToList (void)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);
	return g_index;
}

/*
 * @func:		DeviceManager_SetDeviceOnline
 *
 * @brief:		The function sets the device's Online status
 *
 * @params:		nodeId
 *
 * @retVal:		True if set online success (is currently in the list), False otherwise
 *
 * @note:		None
 */
bool DeviceManager_SetDeviceOnline (EmberNodeId nodeId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	uint8_t indexDevice = DeviceManager_CheckDeviceFromList(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		g_deviceList[indexDevice].connectionStatus = ONLINE;
		return true;
	}

	halCommonSetToken(TOKEN_DEVICE_INFO, g_deviceList);

	return false;
}

/*
 * @func:		DeviceManager_SetDeviceOffline
 *
 * @brief:		The function sets the device's Offline status
 *
 * @params:		nodeId
 *
 * @retVal:		True if set offline success (is currently in the list), False otherwise
 *
 * @note:		None
 */
bool DeviceManager_SetDeviceOffline (EmberNodeId nodeId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	uint8_t indexDevice = DeviceManager_CheckDeviceFromList(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		g_deviceList[indexDevice].connectionStatus = OFFLINE;
		return true;
	}

	halCommonSetToken(TOKEN_DEVICE_INFO, g_deviceList);

	return false;
}

/*
 * @func:		DeviceManager_SetStateLedOn
 *
 * @brief:		The function sets the LED on status
 *
 * @params:		nodeId
 *
 * @retVal:		True if set On Led success (is currently in the list), False otherwise
 *
 * @note:		None
 */
bool DeviceManager_SetStateLedOn (EmberNodeId nodeId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	uint8_t indexDevice = DeviceManager_CheckDeviceFromList(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		g_deviceList[indexDevice].deviceState = LED_ON;
		return true;
	}

	halCommonSetToken(TOKEN_DEVICE_INFO, g_deviceList);

	return false;
}

/*
 * @func:		DeviceManager_SetStateLedOff
 *
 * @brief:		The function sets the LED off status
 *
 * @params:		nodeId
 *
 * @retVal:		True if set off Led success (is currently in the list), False otherwise
 *
 * @note:		None
 */
bool DeviceManager_SetStateLedOff (EmberNodeId nodeId)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	uint8_t indexDevice = DeviceManager_CheckDeviceFromList(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		g_deviceList[indexDevice].deviceState = LED_OFF;
		return true;
	}

	halCommonSetToken(TOKEN_DEVICE_INFO, g_deviceList);

	return false;
}

/*
 * @func:		DeviceManager_UpdateLastTime
 *
 * @brief:		The function updates the latest timestamp upon receiving a message
 *
 * @params[1]:	nodeId
 * @params[2]:	time
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_UpdateLastTime (EmberNodeId nodeId, uint32_t time)
{
	halCommonGetToken(g_deviceList, TOKEN_DEVICE_INFO);

	uint8_t indexDevice = DeviceManager_CheckDeviceFromList(nodeId);

	if(indexDevice != DEVICE_INDEX_INVALID)
	{
		g_deviceList[indexDevice].lastTime = time;
	}

	halCommonSetToken(TOKEN_DEVICE_INFO, g_deviceList);
}

/*
 * @func:		DeviceManager_PrintDeviceList
 *
 * @brief:		The function prints the list of devices
 *
 * @params:		None
 *
 * @retVal:		None
 *
 * @note:		None
 */
void DeviceManager_PrintDeviceList (void)
{
	for(uint8_t i = 0; i < g_index; i++)
	{
		char* modelid = g_deviceList[i].modelID;
		EmberNodeId nodeid = g_deviceList[i].nodeID;
		emberAfCorePrintln("Device %d, node id: %x, modelId: %s", i, nodeid, modelid);
	}
}

/* END FILE */
