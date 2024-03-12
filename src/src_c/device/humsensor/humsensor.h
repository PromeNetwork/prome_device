#ifndef __HUMSENSOR_H__
#define __HUMSENSOR_H__

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_cfg.h"
#include "thread_api.h"


#define DEV_DESC_HUMSENSOR		"humsensor"
#define HUMSENSOR_BASIC_DIR		ROOT_DIR"/data/"DEV_DESC_HUMSENSOR

typedef struct _HUMSENSOR_DATA
{
	UINT32	NodeId;
	double 	Hum;
}HUMSENSOR_DATA ,*P_HUMSENSOR_DATA; 

UINT32 HumsensorCfgInit(void *handle);
UINT32 HumsensorCfgRecvCb(void *handle, void *data, UINT32 data_len);
UINT32 HumSensorDataPkg(void *handle, void *data, data_package *pkg);
UINT32 HumSensorDataSave(void *handle, void *data);
int HumsensorDataSendJava(UINT8 *deviceid, void *data);
UINT32 HumSensorDataUnPkg(void *handle, UINT8 *pkg, void *data);
INT32 HumsensorMqttCfgRecvCb(void *handle, 
							 void *data, 
							 UINT32 data_len, 
							 UINT64 request_id);
UINT32 HumsensorOnline(void *handle);
int HumsensorOnlineJava(UINT8 *deviceid);
void HumsensorTaskClean();
UINT32 HumsensorTaskStart(DEVICE_INFO *dev);
UINT32 HumsensorTaskStop(DEVICE_INFO *dev);
UINT32 HumSensorRuleExec(void *handle);



#endif


