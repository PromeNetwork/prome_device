

#ifndef __GATEWAY_H__
#define __GATEWAY_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "upgrade.h"


#define DEV_DESC_GATEWAY		"boxgateway"
#define DEV_GATEWAY_BASIC_DIR	ROOT_DIR"/data/"DEV_DESC_GATEWAY

typedef struct _GATEWAY_CONFIG
{
	UPGRADE_CFG upgrade;
	DEVICE_INFO *dev_list;
}GATEWAY_CONFIG, *P_GATEWAY_CONFIG;



UINT32 GatewayCfgInit(void *handle);
UINT32 GatewayCfgRecvCb(void *handle, void *data, UINT32 data_len);
UINT32 GatewayDevlistStart(void *handle);
UINT32 GatewayDevlistStop(void *handle);
UINT32 GatewayOnline(void *handle);
//void GatewayPowerOff(void *handle);
UINT32 GatewayThreadStart(void *handle);
UINT32 GatewayThreadStop(void *handle);
void GatewayTimeRequestRestart(void *handle);
void GatewayTimeRequestStart(void *handle);
void GatewayTimeRequestStop(void *handle);
void GatewayUpgradeThreadRestart(void *handle);
void GatewayUpgradeThreadStart(void *handle);
void GatewayUpgradeThreadStop(void *handle);
void GatewayLogSendStop(void *handle);
UINT32 GatewayDataPkg(void *handle, void *data, data_package *pkg);
INT32 GatewayMqttCfgRecvCb(void *handle, 
						 void *data, 
						 UINT32 data_len, 
						 UINT64 request_id);
UINT32 GatewayUpgradeCfgCb(void *handle, void *data, UINT32 len, UINT64 request_id);
UINT32 GatewayOffline(void *handle);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __GATEWAY_H__ */
