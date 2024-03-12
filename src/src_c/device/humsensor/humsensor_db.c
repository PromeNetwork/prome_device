#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "humsensor_db.h"
#include "hexutils.h"
#include "csqlite3.h"


#define HUMSENSOR_TABLE_NAME 	"humsensor_table"

#define HUMSENSOR_DB_DEV_ID		"DeviceId"
#define HUMSENSOR_DB_NODE_ID	"NodeId"
#define HUMSENSOR_DB_HUM		"Humidtity"
#define HUMSENSOR_DB_SIGN		"Sign"
#define HUMSENSOR_DB_POEHASH	"PoEHash"
#define HUMSENSOR_DB_TIMESTAMP	"TimeStamp"
#define HUMSENSOR_DB_PUBKEY		"PubKey"


sql_key humsensor_column[] = 
{
	{DB_TYPE_INT,	HUMSENSOR_DB_TIMESTAMP,	NULL},
	//{DB_TYPE_TEXT,	HUMSENSOR_DB_PUBKEY,		NULL},
	{DB_TYPE_TEXT, 	HUMSENSOR_DB_DEV_ID,		NULL},
	{DB_TYPE_INT,	HUMSENSOR_DB_NODE_ID,	NULL},
	{DB_TYPE_REAL,	HUMSENSOR_DB_HUM,		NULL},
	//{DB_TYPE_TEXT,	HUMSENSOR_DB_SIGN,		NULL},
	//{DB_TYPE_TEXT,	HUMSENSOR_DB_POEHASH,	NULL},
};

#define HUMSENSOR_COL_TIMESTAMP		&humsensor_column[HUMSENSOR_COL_INDEX_TIMESTAMP]
//#define HUMSENSOR_COL_PUBKEY		&humsensor_column[HUMSENSOR_COL_INDEX_PUBKEY]
#define HUMSENSOR_COL_DEV_ID		&humsensor_column[HUMSENSOR_COL_INDEX_DEV_ID]
#define HUMSENSOR_COL_NODE_ID		&humsensor_column[HUMSENSOR_COL_INDEX_NODE_ID]
#define HUMSENSOR_COL_HUM			&humsensor_column[HUMSENSOR_COL_INDEX_HUM]
//#define HUMSENSOR_COL_SIGN			&humsensor_column[HUMSENSOR_COL_INDEX_SIGN]
//#define HUMSENSOR_COL_POEHASH		&humsensor_column[HUMSENSOR_COL_INDEX_POEHASH]

#define HUMSENSOR_COL_ALL			humsensor_column


int humsensor_table_create(sqlite3 *db)
{
	int ret = 0;

	ret = sql_create_table(db, HUMSENSOR_TABLE_NAME, HUMSENSOR_COL_ALL, 
						   sizeof(HUMSENSOR_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

int humsensor_table_add(DEV_DRIVER *dev, sqlite3 *db, HUMSENSOR_DATA *data)
{
	int ret = 0;
	sql_key *column = NULL;

	char devid_str[DEV_ID_MAX_LEN];
	char nodeid_str[DEV_ID_MAX_LEN];
	char hum_str[DEV_ID_MAX_LEN];
	TIMESTAMP  	timestamp;
	char timestamp_str[TIMESTAMP_STR_MAX_LEN];
	//char hash_str[HASH_STR_MAX_LEN];

	//column = HUMSENSOR_COL_PUBKEY;
	//column->key_value = dev->dev_pubkey;

	column = HUMSENSOR_COL_DEV_ID;
	memset(devid_str, 0, DEV_ID_MAX_LEN);
	sprintf(devid_str, "%s", dev->device_id);
	column->key_value = devid_str;

	column = HUMSENSOR_COL_NODE_ID;
	memset(nodeid_str, 0, DEV_ID_MAX_LEN);
	sprintf(nodeid_str, "%d", data->NodeId);
	column->key_value = nodeid_str;

	column = HUMSENSOR_COL_HUM;
	memset(hum_str, 0, DEV_ID_MAX_LEN);
	sprintf(hum_str, "%.2f", data->Hum);
	column->key_value = hum_str;

	column = HUMSENSOR_COL_TIMESTAMP;
	memset(timestamp_str, 0, TIMESTAMP_STR_MAX_LEN);
	timestamp = GetTimestampInt();
	TIMESTAMP_TO_STR(timestamp, timestamp_str);
	column->key_value = timestamp_str;

	//column = HUMSENSOR_COL_SIGN;
	//column->key_value = data->SignedMsg;

	//if(len && (PoeHash != NULL))
	//{
	//	column = HUMSENSOR_COL_POEHASH;
		//memset(hash_str, 0, HASH_STR_MAX_LEN);
		//HexToStr(hash_str, PoeHash, len);
	//	column->key_value = PoeHash;
	//}

	ret = sql_insert_column_data(db, HUMSENSOR_TABLE_NAME, HUMSENSOR_COL_ALL, 
								 sizeof(HUMSENSOR_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

UINT32 HumsensorTableSearchByTime(DEV_DRIVER *dev, 
								  TIMESTAMP start, 
								  TIMESTAMP stop, 
								  sql_callback callback, 
								  void *para)
{
	sql_key *range = NULL;
	sql_key *column = NULL;
	char starttime[TIMESTAMP_STR_MAX_LEN];
	char stoptime[TIMESTAMP_STR_MAX_LEN];
	UINT32 ret = 0;
	sqlite3 *db_handle = NULL;
	UINT8 *db_file = dev->db_file;

	ret = sql_database_init(db_file, &db_handle);
	if(ret)
	{
		return 1;
	}

	range = HUMSENSOR_COL_TIMESTAMP;
	memset(starttime, 0, sizeof(starttime));
	memset(stoptime, 0, sizeof(stoptime));

	TIMESTAMP_TO_STR(start, starttime);
	TIMESTAMP_TO_STR(stop, stoptime);

	column = HUMSENSOR_COL_DEV_ID;
	column->key_value = dev->device_id;
	ret = sql_search_range(db_handle, HUMSENSOR_TABLE_NAME, range, starttime, 
							stoptime, column, 1, callback, para);

	sql_database_deinit(db_file, db_handle);	

	return ret;
}


UINT32 HumsensorDataToDb(DEV_DRIVER *dev, HUMSENSOR_DATA *data)
{
	int ret = 0;
	sqlite3 *db_handle = NULL;
	UINT8 *db_file = dev->db_file;

	ret = sql_database_init(db_file, &db_handle);
	if(ret)
	{
		return 1;
	}
	
	ret = humsensor_table_create(db_handle);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	ret = humsensor_table_add(dev, db_handle, data);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	sql_database_deinit(db_file, db_handle);
	
	return 0;
}


