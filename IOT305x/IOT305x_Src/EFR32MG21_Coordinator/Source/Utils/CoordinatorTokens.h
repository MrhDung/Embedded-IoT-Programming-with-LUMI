/**
* Custom Application Tokens
*/
// Define token names
#define CREATOR_DEVICE_INFO    	(0x0001)
#define NVM3KEY_DEVICE_INFO 	(NVM3KEY_DOMAIN_ZIGBEE | 0xB000)

// Types for the tokens
#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#include "typedefs.h"

typedef enum
{
	OFFLINE,
	ONLINE
} connectionStatus_e;

typedef enum
{
	DEVICE_OFF,
	DEVICE_ON
} deviceState_e;

typedef struct
{
	EmberNodeId nodeID;
	connectionStatus_e connectionStatus;
	deviceState_e deviceState;
	uint32_t lastTime;
	char modelID[20];
} deviceInfo_t;

#endif // DEFINETYPES

#ifdef DEFINETOKENS
// Define the actual token storage information
DEFINE_BASIC_TOKEN (DEVICE_INFO, deviceInfo_t, {0xFF})
#endif // DEFINETOKENS
