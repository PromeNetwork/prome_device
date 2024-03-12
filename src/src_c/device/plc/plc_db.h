

#ifndef __PLC_DB_H__
#define __PLC_DB_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "csqlite3.h"
#include "plc.h"

typedef enum
{
	PLC_COL_INDEX_TIMESTAMP = 0,
	PLC_COL_INDEX_PUBKEY,	
	PLC_COL_INDEX_DEV_ID,	
	PLC_COL_INDEX_DATA,	
	PLC_COL_INDEX_SIGN,		
	PLC_COL_INDEX_POEHASH,	
}PLC_COL_INDEX;


UINT32 PlcDataToDb(DEV_DRIVER *dev, PLC_DATA *data, 
						  UINT8 *PoeHash, UINT32 len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PLC_DB_H__ */
