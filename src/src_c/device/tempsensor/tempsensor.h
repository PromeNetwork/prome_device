#ifndef __TEMPSENSOR_H__
#define __TEMPSENSOR_H__

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_cfg.h"
#include "thread_api.h"


#define DEV_DESC_TEMPSENSOR		"tempsensor"
#define TEMPSENSOR_BASIC_DIR		ROOT_DIR"/data/"DEV_DESC_TEMPSENSOR

typedef struct _TEMPSENSOR_DATA
{
	UINT32	NodeId;
	double 	temp;
}TEMPSENSOR_DATA ,*P_TEMPSENSOR_DATA; 

UINT32 TempsensorCfgInit(void *handle);
UINT32 TempsensorCfgRecvCb(void *handle, void *data, UINT32 data_len);
UINT32 TempSensorDataPkg(void *handle, void *data, data_package *pkg);
UINT32 TempSensorDataSave(void *handle, void *data);
int TempsensorDataSendJava(UINT8 *deviceid, void *data);
UINT32 TempSensorDataUnPkg(void *handle, UINT8 *pkg, void *data);
INT32  TempsensorMqttCfgRecvCb(void *handle, void *data, UINT32 data_len, UINT64 request_id);
UINT32 TempsensorOnline(void *handle);
int TempsensorOnlineJava(UINT8 *deviceid);
void TempsensorTaskClean();
UINT32 TempsensorTaskStart(DEVICE_INFO *dev);
UINT32 TempsensorTaskStop(DEVICE_INFO *dev);
UINT32 TempSensorRuleExec(void *handle);



#endif


