#if 0
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "rfid_db.h"
#include "hexutils.h"
#include "csqlite3.h"


#define RFID_TABLE_NAME 	"rfid_table"

#define RFID_DB_DEV_ID		"DeviceId"
#define RFID_DB_RFID		"Rfid"
#define RFID_DB_ATID		"AntennaId"
#define RFID_DB_RSSI		"Rssi"
#define RFID_DB_SIGN		"Sign"
#define RFID_DB_POEHASH		"PoEHash"
#define RFID_DB_TIMESTAMP	"TimeStamp"
#define RFID_DB_PUBKEY		"PubKey"


sql_key rfid_column[] = 
{
	{DB_TYPE_INT,	RFID_DB_TIMESTAMP,	NULL},
	{DB_TYPE_TEXT,	RFID_DB_PUBKEY,		NULL},
	{DB_TYPE_TEXT, 	RFID_DB_DEV_ID,		NULL},
	{DB_TYPE_TEXT,	RFID_DB_RFID,		NULL},
	{DB_TYPE_INT,	RFID_DB_ATID,		NULL},
	{DB_TYPE_REAL,	RFID_DB_RSSI,		NULL},
	{DB_TYPE_TEXT,	RFID_DB_SIGN,		NULL},
	{DB_TYPE_TEXT,	RFID_DB_POEHASH,	NULL},
};

#define RFID_COL_TIMESTAMP	&rfid_column[RFID_COL_INDEX_TIMESTAMP]
#define RFID_COL_PUBKEY		&rfid_column[RFID_COL_INDEX_PUBKEY]
#define RFID_COL_DEV_ID		&rfid_column[RFID_COL_INDEX_DEV_ID]
#define RFID_COL_RFID		&rfid_column[RFID_COL_INDEX_RFID]
#define RFID_COL_ATID		&rfid_column[RFID_COL_INDEX_ATID]
#define RFID_COL_RSSI		&rfid_column[RFID_COL_INDEX_RSSI]
#define RFID_COL_SIGN		&rfid_column[RFID_COL_INDEX_SIGN]
#define RFID_COL_POEHASH	&rfid_column[RFID_COL_INDEX_POEHASH]

#define RFID_COL_ALL		rfid_column

static int rfid_table_create(sqlite3 *db)
{
	int ret = 0;

	ret = sql_create_table(db, RFID_TABLE_NAME, RFID_COL_ALL, 
						   sizeof(RFID_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

static int rfid_table_add(DEV_DRIVER *dev, sqlite3 *db, 
								 RFID_DATA *data, 
						  		 UINT8 *PoeHash, UINT32 len)
{
	int ret = 0;
	sql_key *column = NULL;

	char devid_str[DEV_ID_MAX_LEN];
	char at_str[DEV_ID_MAX_LEN];
	char rssi_str[DEV_ID_MAX_LEN];
	char timestamp_str[TIMESTAMP_STR_MAX_LEN];
	//char hash_str[HASH_STR_MAX_LEN];

	column = RFID_COL_PUBKEY;
	column->key_value = dev->dev_pubkey;

	column = RFID_COL_DEV_ID;
	memset(devid_str, 0, DEV_ID_MAX_LEN);
	sprintf(devid_str, "%s", dev->device_id);
	column->key_value = devid_str;

	column = RFID_COL_RFID;
	column->key_value = data->Rfid;

	column = RFID_COL_ATID;
	memset(at_str, 0, DEV_ID_MAX_LEN);
	sprintf(at_str, "%d", data->AntennaID);
	column->key_value = at_str;

	column = RFID_COL_RSSI;
	memset(rssi_str, 0, DEV_ID_MAX_LEN);
	sprintf(rssi_str, "%.2f", data->RSSI);
	column->key_value = rssi_str;

	column = RFID_COL_TIMESTAMP;
	memset(timestamp_str, 0, TIMESTAMP_STR_MAX_LEN);
	TIMESTAMP_TO_STR(data->TimeStamp, timestamp_str);
	column->key_value = timestamp_str;

	column = RFID_COL_SIGN;
	column->key_value = data->SignedMsg;

	if(len && (PoeHash != NULL))
	{
		column = RFID_COL_POEHASH;
		//memset(hash_str, 0, HASH_STR_MAX_LEN);
		//HexToStr(hash_str, PoeHash, len);
		column->key_value = PoeHash;
	}

	ret = sql_insert_column_data(db, RFID_TABLE_NAME, RFID_COL_ALL, 
								 sizeof(RFID_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

UINT32 RfidDataToDb(DEV_DRIVER *dev, RFID_DATA *data, UINT8 *PoeHash, UINT32 len)
{
	int ret = 0;
	sqlite3 *db_handle = NULL;
	UINT8 *db_file = dev->db_file;

	ret = sql_database_init(db_file, &db_handle);
	if(ret)
	{
		return 1;
	}

	ret = rfid_table_create(db_handle);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	ret = rfid_table_add(dev, db_handle, data, PoeHash, len);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	sql_database_deinit(db_file, db_handle);

	return 0;
}
#endif


