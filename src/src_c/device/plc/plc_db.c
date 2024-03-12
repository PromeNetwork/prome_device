#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "plc_db.h"
#include "hexutils.h"
#include "csqlite3.h"


#define PLC_TABLE_NAME 		"plc_table"

#define PLC_DB_DEV_ID		"DeviceId"
#define PLC_DB_DATA			"Data"
#define PLC_DB_SIGN			"Sign"
#define PLC_DB_POEHASH		"PoEHash"
#define PLC_DB_TIMESTAMP	"TimeStamp"
#define PLC_DB_PUBKEY		"PubKey"


sql_key plc_column[] = 
{
	{DB_TYPE_INT,	PLC_DB_TIMESTAMP,	NULL},
	{DB_TYPE_TEXT,	PLC_DB_PUBKEY,		NULL},
	{DB_TYPE_TEXT, 	PLC_DB_DEV_ID,		NULL},
	{DB_TYPE_REAL,	PLC_DB_DATA,		NULL},
	{DB_TYPE_TEXT,	PLC_DB_SIGN,		NULL},
	{DB_TYPE_TEXT,	PLC_DB_POEHASH,		NULL},
};

#define PLC_COL_TIMESTAMP	&plc_column[PLC_COL_INDEX_TIMESTAMP]
#define PLC_COL_PUBKEY		&plc_column[PLC_COL_INDEX_PUBKEY]
#define PLC_COL_DEV_ID		&plc_column[PLC_COL_INDEX_DEV_ID]
#define PLC_COL_DATA		&plc_column[PLC_COL_INDEX_DATA]
#define PLC_COL_SIGN		&plc_column[PLC_COL_INDEX_SIGN]
#define PLC_COL_POEHASH		&plc_column[PLC_COL_INDEX_POEHASH]

#define PLC_COL_ALL			plc_column

static int plc_table_create(sqlite3 *db)
{
	int ret = 0;

	ret = sql_create_table(db, PLC_TABLE_NAME, PLC_COL_ALL, 
						   sizeof(PLC_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

static int plc_table_add(DEV_DRIVER *dev, sqlite3 *db, 
								 PLC_DATA *data, 
						  		 UINT8 *PoeHash, UINT32 len)
{
	int ret = 0;
	sql_key *column = NULL;

	char devid_str[DEV_ID_MAX_LEN];
	char data_str[DEV_ID_MAX_LEN];
	char timestamp_str[TIMESTAMP_STR_MAX_LEN];
	//char hash_str[HASH_STR_MAX_LEN];

	column = PLC_COL_PUBKEY;
	column->key_value = dev->dev_pubkey;

	column = PLC_COL_DEV_ID;
	memset(devid_str, 0, DEV_ID_MAX_LEN);
	sprintf(devid_str, "%s", dev->device_id);
	column->key_value = devid_str;

	column = PLC_COL_DATA;
	memset(data_str, 0, DEV_ID_MAX_LEN);
	sprintf(data_str, "%f", data->PlcData);
	column->key_value = data_str;

	#if 0
	column = PLC_COL_TIMESTAMP;
	memset(timestamp_str, 0, TIMESTAMP_STR_MAX_LEN);
	TIMESTAMP_TO_STR(data->TimeStamp, timestamp_str);
	column->key_value = timestamp_str;

	column = PLC_COL_SIGN;
	column->key_value = data->SignedMsg;

	if(len && (PoeHash != NULL))
	{
		column = PLC_COL_POEHASH;
		//memset(hash_str, 0, HASH_STR_MAX_LEN);
		//HexToStr(hash_str, PoeHash, len);
		column->key_value = PoeHash;
	}
	#endif

	ret = sql_insert_column_data(db, PLC_TABLE_NAME, PLC_COL_ALL, 
								 sizeof(PLC_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

UINT32 PlcDataToDb(DEV_DRIVER *dev, PLC_DATA *data, UINT8 *PoeHash, UINT32 len)
{
	int ret = 0;
	sqlite3 *db_handle = NULL;
	UINT8 *db_file = dev->db_file;

	ret = sql_database_init(db_file, &db_handle);
	if(ret)
	{
		return 1;
	}

	ret = plc_table_create(db_handle);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	ret = plc_table_add(dev, db_handle, data, PoeHash, len);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	sql_database_deinit(db_file, db_handle);
	return 0;
}


