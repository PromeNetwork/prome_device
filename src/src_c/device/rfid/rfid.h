

#ifndef __RFID_H__
#define __RFID_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_cfg.h"
#include "thread_api.h"

 
#define DEV_DESC_RFID		"rfid"
#define RFID_BASIC_DIR		ROOT_DIR"/data/"DEV_DESC_RFID
 
 typedef struct _RFID_DATA
 {
	/*����Ϊ�豸���ݽṹ˽���ֶ�*/
	UINT8	Rfid[DEV_ID_MAX_LEN];
	UINT32	RfidLen;
	UINT32	AntennaID;
	double	RSSI;

 }RFID_DATA ,*P_RFID_DATA; 


UINT32 RfidCfgInit(void *handle);
UINT32 RfidCfgRecvCb(void *handle, void *data, UINT32 data_len);
UINT32 RfidDataPkg(void *handle, void *data, data_package *pkg);
UINT32 RfidDataSave(void *handle, void *data, UINT8 *PoeHash, UINT32 len);
UINT32 RfidDataSendJava(UINT8 *deviceid, void *data);
UINT32 RfidDataUnPkg(void *handle, UINT8 *pkg, void *data);
INT32  RfidMqttCfgRecvCb(void *handle, void *data, UINT32 data_len, UINT64 request_id);
UINT32 RfidOnline(void *handle);
UINT32 RfidOnlineJava(UINT8 *deviceid);
void RfidTaskClean();
UINT32 RfidTaskStart(DEVICE_INFO *dev);
UINT32 RfidTaskStop(DEVICE_INFO *dev);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __RFID_H__ */
