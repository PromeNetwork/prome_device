

#ifndef __RFID_DB_H__
#define __RFID_DB_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "csqlite3.h"
#include "rfid.h"

typedef enum
{
	RFID_COL_INDEX_TIMESTAMP = 0,
	RFID_COL_INDEX_PUBKEY,	
	RFID_COL_INDEX_DEV_ID,	
	RFID_COL_INDEX_RFID,
	RFID_COL_INDEX_ATID,
	RFID_COL_INDEX_RSSI,
	RFID_COL_INDEX_SIGN,		
	RFID_COL_INDEX_POEHASH,	
}RFID_COL_INDEX;


UINT32 RfidDataToDb(DEV_DRIVER *dev, RFID_DATA *data, 
					UINT8 *PoeHash, UINT32 len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __RFID_DB_H__ */
