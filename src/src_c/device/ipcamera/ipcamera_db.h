

#ifndef __IPCAMERA_DB_H__
#define __IPCAMERA_DB_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "csqlite3.h"
#include "ipcamera.h"

typedef enum
{
	IPCAMERA_COL_INDEX_TIMESTAMP = 0,
	IPCAMERA_COL_INDEX_PUBKEY,	
	IPCAMERA_COL_INDEX_DEV_ID,	
	IPCAMERA_COL_INDEX_PIC,	
	IPCAMERA_COL_INDEX_PIC_ID,		
	IPCAMERA_COL_INDEX_PIC_HASH,		
	IPCAMERA_COL_INDEX_SIGN,		
	IPCAMERA_COL_INDEX_POEHASH,	
}IPCAMERA_COL_INDEX;


UINT32 IpcameraDataToDb(DEV_DRIVER *dev, IPCAMERA_DATA *data, 
						  UINT8 *PoeHash, UINT32 len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __IPCAMERA_DB_H__ */
