#ifndef __TEMPSENSOR_DB_H__
#define __TEMPSENSOR_DB_H__

#include "tempsensor.h"

#include "csqlite3.h"

typedef enum
{
	TEMPSENSOR_COL_INDEX_TIMESTAMP = 0,
	//TEMPSENSOR_COL_INDEX_PUBKEY,	
	TEMPSENSOR_COL_INDEX_DEV_ID,	
	TEMPSENSOR_COL_INDEX_NODE_ID,	
	TEMPSENSOR_COL_INDEX_HUM,		
	//TEMPSENSOR_COL_INDEX_SIGN,		
	//TEMPSENSOR_COL_INDEX_POEHASH,	
}TEMPSENSOR_COL_INDEX;


UINT32 TempsensorDataToDb(DEV_DRIVER *dev, TEMPSENSOR_DATA *data);
UINT32 TempsensorTableSearchByTime(DEV_DRIVER *dev, 
								  TIMESTAMP start, 
								  TIMESTAMP stop, 
								  sql_callback callback, 
								  void *para);

#endif
