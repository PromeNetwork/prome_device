#if 0
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "ipcamera_db.h"
#include "hexutils.h"
#include "csqlite3.h"


#define IPCAMERA_TABLE_NAME 		"ipcamera_table"

#define IPCAMERA_DB_DEV_ID		"DeviceId"
#define IPCAMERA_DB_PIC			"Pic"
#define IPCAMERA_DB_PIC_ID		"PicId"
#define IPCAMERA_DB_PIC_HASH	"PicHash"
#define IPCAMERA_DB_SIGN			"Sign"
#define IPCAMERA_DB_POEHASH		"PoEHash"
#define IPCAMERA_DB_TIMESTAMP	"TimeStamp"
#define IPCAMERA_DB_PUBKEY		"PubKey"


sql_key ipcamera_column[] = 
{
	{DB_TYPE_INT,	IPCAMERA_DB_TIMESTAMP,	NULL},
	{DB_TYPE_TEXT,	IPCAMERA_DB_PUBKEY,		NULL},
	{DB_TYPE_TEXT, 	IPCAMERA_DB_DEV_ID,		NULL},
	{DB_TYPE_TEXT,	IPCAMERA_DB_PIC,		NULL},
	{DB_TYPE_INT,	IPCAMERA_DB_PIC_ID,		NULL},
	{DB_TYPE_TEXT,	IPCAMERA_DB_PIC_HASH,	NULL},
	{DB_TYPE_TEXT,	IPCAMERA_DB_SIGN,		NULL},
	{DB_TYPE_TEXT,	IPCAMERA_DB_POEHASH,	NULL},
};

#define IPCAMERA_COL_TIMESTAMP	&ipcamera_column[IPCAMERA_COL_INDEX_TIMESTAMP]
#define IPCAMERA_COL_PUBKEY		&ipcamera_column[IPCAMERA_COL_INDEX_PUBKEY]
#define IPCAMERA_COL_DEV_ID		&ipcamera_column[IPCAMERA_COL_INDEX_DEV_ID]
#define IPCAMERA_COL_PIC		&ipcamera_column[IPCAMERA_COL_INDEX_PIC]
#define IPCAMERA_COL_PIC_ID		&ipcamera_column[IPCAMERA_COL_INDEX_PIC_ID]
#define IPCAMERA_COL_PIC_HASH	&ipcamera_column[IPCAMERA_COL_INDEX_PIC_HASH]
#define IPCAMERA_COL_SIGN		&ipcamera_column[IPCAMERA_COL_INDEX_SIGN]
#define IPCAMERA_COL_POEHASH	&ipcamera_column[IPCAMERA_COL_INDEX_POEHASH]

#define IPCAMERA_COL_ALL		ipcamera_column

static int ipcamera_table_create(sqlite3 *db)
{
	int ret = 0;

	ret = sql_create_table(db, IPCAMERA_TABLE_NAME, IPCAMERA_COL_ALL, 
						   sizeof(IPCAMERA_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

static int ipcamera_table_add(DEV_DRIVER *dev, sqlite3 *db, 
								 IPCAMERA_DATA *data, 
						  		 UINT8 *PoeHash, UINT32 len)
{
	int ret = 0;
	sql_key *column = NULL;

	char devid_str[DEV_ID_MAX_LEN];
	char pic_id_str[DEV_ID_MAX_LEN];
	//char pic_hash_str[HASH_STR_MAX_LEN];
	char timestamp_str[TIMESTAMP_STR_MAX_LEN];
	//char hash_str[HASH_STR_MAX_LEN];

	column = IPCAMERA_COL_PUBKEY;
	column->key_value = dev->dev_pubkey;

	column = IPCAMERA_COL_DEV_ID;
	memset(devid_str, 0, DEV_ID_MAX_LEN);
	sprintf(devid_str, "%s", dev->device_id);
	column->key_value = devid_str;

	column = IPCAMERA_COL_PIC;
	column->key_value = data->Pic;

	column = IPCAMERA_COL_PIC_ID;
	memset(pic_id_str, 0, DEV_ID_MAX_LEN);
	sprintf(pic_id_str, "%d", data->PicId);
	column->key_value = pic_id_str;

	column = IPCAMERA_COL_PIC_HASH;
	//memset(pic_hash_str, 0, HASH_STR_MAX_LEN);
	//HexToStr(pic_hash_str, data->PicHash, data->PicHashLen);
	column->key_value = data->PicHash;

	column = IPCAMERA_COL_TIMESTAMP;
	memset(timestamp_str, 0, TIMESTAMP_STR_MAX_LEN);
	TIMESTAMP_TO_STR(data->TimeStamp, timestamp_str);
	column->key_value = timestamp_str;

	column = IPCAMERA_COL_SIGN;
	column->key_value = data->SignedMsg;

	if(len && (PoeHash != NULL))
	{
		column = IPCAMERA_COL_POEHASH;
		//memset(hash_str, 0, HASH_STR_MAX_LEN);
		//HexToStr(hash_str, PoeHash, len);
		column->key_value = PoeHash;
	}

	ret = sql_insert_column_data(db, IPCAMERA_TABLE_NAME, IPCAMERA_COL_ALL, 
								 sizeof(IPCAMERA_COL_ALL)/sizeof(sql_key));
	if(ret)
	{
		return ret;
	}
	return 0;
}

UINT32 IpcameraDataToDb(DEV_DRIVER *dev, IPCAMERA_DATA *data, UINT8 *PoeHash, UINT32 len)
{
	int ret = 0;
	sqlite3 *db_handle = NULL;
	UINT8 *db_file = dev->db_file;

	ret = sql_database_init(db_file, &db_handle);
	if(ret)
	{
		return 1;
	}

	ret = ipcamera_table_create(db_handle);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	ret = ipcamera_table_add(dev, db_handle, data, PoeHash, len);
	if(ret)
	{
		sql_database_deinit(db_file, db_handle);
		return ret;
	}

	sql_database_deinit(db_file, db_handle);

	return 0;
}
#endif

