#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "tempsensor_db.h"
#include "hexutils.h"
#include "csqlite3.h"


#define TEMPSENSOR_TABLE_NAME 	"tempsensor_table"

#define TEMPSENSOR_DB_DEV_ID		"DeviceId"
#define TEMPSENSOR_DB_NODE_ID	"NodeId"
#define TEMPSENSOR_DB_HUM		"Temperature"
#define TEMPSENSOR_DB_SIGN		"Sign"
#define TEMPSENSOR_DB_POEHASH	"PoEHash"
#define TEMPSENSOR_DB_TIMESTAMP	"TimeStamp"
#define TEMPSENSOR_DB_PUBKEY		"PubKey"


sql_key tempsensor_column[] = 
{
	{DB_TYPE_INT,	TEMPSENSOR_DB_TIMESTAMP,	NULL},
	//{DB_TYPE_TEXT,	TEMPSENSOR_DB_PUBKEY,		NULL},
	{DB_TYPE_TEXT, 	TEMPSENSOR_DB_DEV_ID,		NULL},
	{DB_TYPE_INT,	TEMPSENSOR_DB_NODE_ID,	NULL},
	{DB_TYPE_REAL,	TEMPSENSOR_DB_HUM,		NULL},
	//{DB_TYPE_TEXT,	TEMPSENSOR_DB_SIGN,		NULL},
	//{DB_TYPE_TEXT,	TEMPSENSOR_DB_POEHASH,	NULL},
};

#define TEMPSENSOR_COL_TIMESTAMP		&tempsensor_column[TEMPSENSOR_COL_INDEX_TIMESTAMP]
//#define TEMPSENSOR_COL_PUBKEY		&tempsensor_column[TEMPSENSOR_COL_INDEX_PUBKEY]
#define TEMPSENSOR_COL_DEV_ID		&tempsensor_column[TEMPSENSOR_COL_INDEX_DEV_ID]
#define TEMPSENSOR_COL_NODE_ID		&tempsensor_column[TEMPSENSOR_COL_INDEX_NODE_ID]
#define TEMPSENSOR_COL_HUM			&tempsensor_column[TEMPSENSOR_COL_INDEX_HUM]
//#define TEMPSENSOR_COL_SIGN			&tempsensor_column[TEMPSENSOR_COL_INDEX_SIGN]
//#define TEMPSENSOR_COL_POEHASH		&tempsensor_column[TEMPSENSOR_COL_INDEX_POEHASH]

#define TEMPSENSOR_COL_ALL			tempsensor_column


int tempsensor_table_create(sqlite3 *db)
{
	int ret = 0;

	ret = sql_create_table(db, TEMPSENSOR_TABLE_NAME, TEMPSENSOR_COL_ALL, 
						   sizeof(TEMPSENSOR_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

int tempsensor_table_add(DEV_DRIVER *dev, sqlite3 *db, TEMPSENSOR_DATA *data)
{
	int ret = 0;
	sql_key *column = NULL;

	char devid_str[DEV_ID_MAX_LEN];
	char nodeid_str[DEV_ID_MAX_LEN];
	char hum_str[DEV_ID_MAX_LEN];
	TIMESTAMP  	timestamp;
	char timestamp_str[TIMESTAMP_STR_MAX_LEN];
	//char hash_str[HASH_STR_MAX_LEN];

	//column = TEMPSENSOR_COL_PUBKEY;
	//column->key_value = dev->dev_pubkey;

	column = TEMPSENSOR_COL_DEV_ID;
	memset(devid_str, 0, DEV_ID_MAX_LEN);
	sprintf(devid_str, "%s", dev->device_id);
	column->key_value = devid_str;

	column = TEMPSENSOR_COL_NODE_ID;
	memset(nodeid_str, 0, DEV_ID_MAX_LEN);
	sprintf(nodeid_str, "%d", data->NodeId);
	column->key_value = nodeid_str;

	column = TEMPSENSOR_COL_HUM;
	memset(hum_str, 0, DEV_ID_MAX_LEN);
	sprintf(hum_str, "%.2f", data->temp);
	column->key_value = hum_str;

	column = TEMPSENSOR_COL_TIMESTAMP;
	memset(timestamp_str, 0, TIMESTAMP_STR_MAX_LEN);
	timestamp = GetTimestampInt();
	TIMESTAMP_TO_STR(timestamp, timestamp_str);
	column->key_value = timestamp_str;

	//column = TEMPSENSOR_COL_SIGN;
	//column->key_value = data->SignedMsg;

	//if(len && (PoeHash != NULL))
	//{
	//	column = TEMPSENSOR_COL_POEHASH;
		//memset(hash_str, 0, HASH_STR_MAX_LEN);
		//HexToStr(hash_str, PoeHash, len);
	//	column->key_value = PoeHash;
	//}

	ret = sql_insert_column_data(db, TEMPSENSOR_TABLE_NAME, TEMPSENSOR_COL_ALL, 
								 sizeof(TEMPSENSOR_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

UINT32 TempsensorTableSearchByTime(DEV_DRIVER *dev, 
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

	range = TEMPSENSOR_COL_TIMESTAMP;
	memset(starttime, 0, sizeof(starttime));
	memset(stoptime, 0, sizeof(stoptime));

	TIMESTAMP_TO_STR(start, starttime);
	TIMESTAMP_TO_STR(stop, stoptime);

	column = TEMPSENSOR_COL_DEV_ID;
	column->key_value = dev->device_id;
	ret = sql_search_range(db_handle, TEMPSENSOR_TABLE_NAME, range, starttime, 
							stoptime, column, 1, callback, para);

	sql_database_deinit(db_file, db_handle);	

	return ret;
}


UINT32 TempsensorDataToDb(DEV_DRIVER *dev, TEMPSENSOR_DATA *data)
{
	int ret = 0;
	sqlite3 *db_handle = NULL;
	UINT8 *db_file = dev->db_file;

	ret = sql_database_init(db_file, &db_handle);
	if(ret)
	{
		return 1;
	}
	
	ret = tempsensor_table_create(db_handle);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	ret = tempsensor_table_add(dev, db_handle, data);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	sql_database_deinit(db_file, db_handle);
	
	return 0;
}


