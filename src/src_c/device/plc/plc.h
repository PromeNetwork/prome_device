

#ifndef __PLC_H__
#define __PLC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "dev_comm.h"
 
#define DEV_DESC_PLC		"plc"
#define PLC_BASIC_DIR		ROOT_DIR"/data/"DEV_DESC_PLC
#define PLC_DEV_DIR			ROOT_DIR"/data/"DEV_DESC_PLC"/dev"
#define PLC_DATABASE 		ROOT_DIR"/data/"DEV_DESC_PLC"/"DEV_DESC_PLC".db"

#define SECTION_ADDR_STR	"plc_addr"
#define KEY_SLAVE_ID		"slave_id"
#define KEY_SLAVE_PORT		"slave_port"
#define KEY_SLAVE_INTERVAL	"slave_interval"
#define KEY_PLC_ADDR		SECTION_ADDR_STR

 
 typedef struct _PLC_DATA
 {
	double		PlcData;
	UINT8 		host[DOMAIN_LEN];
	UINT32 		port;
	UINT32		slave_id;
	UINT8 		protocol[DOMAIN_LEN];
 }PLC_DATA ,*P_PLC_DATA; 


 UINT32 PlcCfgAlloc(void *handle);
 UINT32 PlcCfgInit(void *handle);
 UINT32 PlcCfgRecvCb(void *handle, void *data, UINT32 data_len);
 UINT32 PlcDataPkg(void *handle, void *data, data_package *pkg);
 UINT32 PlcDataSave(void *handle, void *data, UINT8 *PoeHash, UINT32 len);
 UINT32 PlcDataUnPkg(void *handle, UINT8 *pkg, void *data);
 UINT32 PlcOnline(void *handle);
 UINT32 PlcTaskStart(DEVICE_INFO *dev);
 UINT32 PlcTaskStop(DEVICE_INFO *dev);
 INT32 PlcMqttCfgRecvCb(void *handle, void *data, UINT32 data_len, UINT64 request_id);
 void PlcTaskClean();



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PLC_H__ */
