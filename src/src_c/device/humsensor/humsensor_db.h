#ifndef __HUMSENSOR_DB_H__
#define __HUMSENSOR_DB_H__

#include "humsensor.h"

#include "csqlite3.h"

typedef enum
{
	HUMSENSOR_COL_INDEX_TIMESTAMP = 0,
	//HUMSENSOR_COL_INDEX_PUBKEY,	
	HUMSENSOR_COL_INDEX_DEV_ID,	
	HUMSENSOR_COL_INDEX_NODE_ID,	
	HUMSENSOR_COL_INDEX_HUM,		
	//HUMSENSOR_COL_INDEX_SIGN,		
	//HUMSENSOR_COL_INDEX_POEHASH,	
}HUMSENSOR_COL_INDEX;


UINT32 HumsensorDataToDb(DEV_DRIVER *dev, HUMSENSOR_DATA *data);
UINT32 HumsensorTableSearchByTime(DEV_DRIVER *dev, 
								  TIMESTAMP start, 
								  TIMESTAMP stop, 
								  sql_callback callback, 
								  void *para);

#endif
