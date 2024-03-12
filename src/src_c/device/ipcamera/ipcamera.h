

#ifndef __IPCAMERA_H__
#define __IPCAMERA_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_cfg.h"
#include "thread_api.h"

 
#define DEV_DESC_IPCAMERA		"ipcamera"
#define IPCAMERA_BASIC_DIR		ROOT_DIR"/data/"DEV_DESC_IPCAMERA
#define IPCAMERA_DEV_DIR		IPCAMERA_BASIC_DIR"/dev"
#define IPCAMERA_PIC_DIR		IPCAMERA_BASIC_DIR"/pic"
#define IPCAMERA_VIDEO_DIR		IPCAMERA_BASIC_DIR"/video"


#define KEY_RTSP_URL					"rtsp_url"
 
 typedef struct _IPCAMERA_DATA
 {
	UINT8			Pic[FILE_NAME_MAX_LEN];
	UINT32			PicId;
	UINT8			PicHash[HASH_STR_MAX_LEN];
	UINT32			PicHashLen;
 }IPCAMERA_DATA ,*P_IPCAMERA_DATA; 

 typedef struct _IPCAMERA_CFG
 {
	UINT8			PicServer[URL_MAX_LEN];
	UINT32			interval;
 }IPCAMERA_CFG ,*P_IPCAMERA_CFG;


UINT32 IpcameraCfgInit(void *handle);
UINT32 IpcameraCfgRecvCb(void *handle, void *data, UINT32 data_len);
UINT32 IpcameraDataPkg(void *handle, void *data, data_package *pkg);
UINT32 IpcameraDataSave(void *handle, 
						void *data, 
        				UINT8 *PoeHash, 
        				UINT32 len);
UINT32 IpcameraDataUnPkg(void *handle, UINT8 *pkg, void *data);
UINT32 IpcameraFileToPlatform(DEV_DRIVER *dev, 
							  UINT8 *picname_in, 
        					  UINT8 *picname_out, INT32 *pic_id);
INT32 IpcameraMqttCfgRecvCb(void *handle, void *data, UINT32 data_len, UINT64 request_id);
UINT32 IpcameraOnline(void *handle);
void IpcameraTaskClean();
UINT32 IpcameraTaskStart(DEVICE_INFO *dev);
UINT32 IpcameraTaskStop(DEVICE_INFO *dev);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __IPCAMERA_H__ */
