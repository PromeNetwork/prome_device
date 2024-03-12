#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <sqlite3.h>

#include "csqlite3.h"
#include "log.h"

static sqlite3 * database = NULL;


#define DB_TYPE_INT_STR			"INTEGER"
#define DB_TYPE_REAL_STR		"REAL"
#define DB_TYPE_TEXT_STR		"TEXT"
#define DB_TYPE_BLOB_STR		"BLOB"
#define DB_TYPE_NULL_STR		"NULL"

#define TABLE_COUNT								"select count(*) from sqlite_master where type='table' and name='%s'"
//#define QUERY_CREATE_TABLE					"create table %s(%s %s primary key NOT NULL)"
#define CREATE_TABLE_AUTO_PRIMARY_KEY			"create table %s ( ID INTEGER PRIMARY KEY AUTOINCREMENT, %s"
#define CREATE_TABLE							"create table %s %s"
#define CREATE_TABLE_PRIMARY_KEY				"(%s %s primary key NOT NULL"
#define TABLE_ADD_COLUMN						"ALTER TABLE %s ADD COLUMN %s %s"
#define TABLE_INSERT							"INSERT INTO %s %s VALUES %s"


#define TABLE_INSERT_PRIMARY_KEY_INT			"INSERT INTO %s (%s) VALUES (%s)"
#define TABLE_INSERT_PRIMARY_KEY_TEXT			"INSERT INTO %s (%s) VALUES ('%s')"

#define TABLE_UPDATE_PRIMARY_KEY_INT_KEY_INT 	"UPDATE %s SET %s = %s WHERE %s = %s"
#define TABLE_UPDATE_PRIMARY_KEY_INT_KEY_TEXT 	"UPDATE %s SET %s = '%s' WHERE %s = %s"
#define TABLE_UPDATE_PRIMARY_KEY_TEXT_KEY_INT 	"UPDATE %s SET %s = %s WHERE %s = '%s'"
#define TABLE_UPDATE_PRIMARY_KEY_TEXT_KEY_TEXT 	"UPDATE %s SET %s = '%s' WHERE %s = '%s'"

#define TABLE_SELECT_PRIMARY_KEY_COUNT_INT		"select count(*) from %s WHERE %s = %s"
#define TABLE_SELECT_PRIMARY_KEY_COUNT_TEXT		"select count(*) from %s WHERE %s = '%s'"
#define TABLE_SELECT_PRIMARY_KEY_INT			"select * from %s WHERE %s = %s"
#define TABLE_SELECT_PRIMARY_KEY_TEXT			"select * from %s WHERE %s = '%s'"

#define TABLE_SELECT_RANGE_INT					"select * from %s WHERE %s between %s and %s"
#define TABLE_SELECT_RANGE_TEXT					"select * from %s WHERE %s between '%s' and '%s'"

//select * from plc_table where TimeStamp < 1610680286222 and DeviceId=41731 order by TimeStamp desc limit 0,1;
#define TABLE_SELECT_LAST_RECORD_INT			"select * from %s WHERE %s < %s"
#define TABLE_SELECT_LAST_RECORD_TEXT			"select * from %s WHERE %s < '%s'"

//#define TABLE_TRAVERSAL  						"select * from %s" 
#define TABLE_TRAVERSAL_BY_DESC					"select * from %s order by %s desc"
#define TABLE_TRAVERSAL_BY_ASC					"select * from %s order by %s asc"



char *key_type_str[]={DB_TYPE_NULL_STR, 
					DB_TYPE_INT_STR, 
					DB_TYPE_REAL_STR, 
					DB_TYPE_TEXT_STR,
					DB_TYPE_BLOB_STR};

int sql_exec(sqlite3 *db, sql_callback callback, 
			 void *para, const char *fmt, ...)
{
	int ret;
	char *errMsg = NULL;
	char *sql;
	va_list ap;
	
	if(db == NULL) return -1;
	
	va_start(ap, fmt);

	sql = sqlite3_vmprintf(fmt, ap);

	#if 1
	do
	{
		ret = sqlite3_exec(db, sql, callback, para, &errMsg);
		if( ret != SQLITE_OK )
		{
			LOG_PRINT(   Prome_LOG_ERROR, "SQL ERROR %d [%s] %s ", ret, errMsg, sql);
			if( strstr(errMsg, "database is locked") )
			{
				sleep(1);
				if (errMsg)
				{
					sqlite3_free(errMsg);
				}
				continue;
			}
			if (errMsg)
			{
				sqlite3_free(errMsg);
			}
			break;
		}
	}while(ret != SQLITE_OK);

	#else
	ret = sqlite3_exec(db, sql, callback, para, &errMsg);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(   Prome_LOG_ERROR, "SQL ERROR %d [%s] %s ", ret, errMsg, sql);
		if (errMsg)
		{
			sqlite3_free(errMsg);
		}
	}
	#endif
	//printf("SQL: %s\n", sql);
	
	
	sqlite3_free(sql);
	va_end(ap);
	
	return ret;
}


int sql_get_int_field(sqlite3 *db, const char *fmt, ...)
{
	va_list		ap;
	int		counter, result;
	char		*sql;
	int		ret;
	sqlite3_stmt	*stmt;
	
	if(db == NULL) return -1;
	
	va_start(ap, fmt);

	sql = sqlite3_vmprintf(fmt, ap);

 	//printf("sql: %s\n", sql);

	switch (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL))
	{
		case SQLITE_OK:
			break;
		default:
			LOG_PRINT(  Prome_LOG_ERROR, "prepare failed: %s\n%s\n",
					  sqlite3_errmsg(db), sql);
			sqlite3_free(sql);
			va_end(ap);
			return -1;
	}
	sqlite3_free(sql);

	for (counter = 0;
	     ((result = sqlite3_step(stmt)) == SQLITE_BUSY || result == SQLITE_LOCKED) && counter < 2;
	     counter++) {
		 /* While SQLITE_BUSY has a built in timeout,
		    SQLITE_LOCKED does not, so sleep */
		 if (result == SQLITE_LOCKED)
		 	sleep(1);
	}

	switch (result)
	{
		case SQLITE_DONE:
			/* no rows returned */
			ret = 0;
			break;
		case SQLITE_ROW:
			if (sqlite3_column_type(stmt, 0) == SQLITE_NULL)
			{
				ret = 0;
				break;
			}
			ret = sqlite3_column_int(stmt, 0);
			//printf("sql ret = %d \n", ret);
			break;
		default:
			LOG_PRINT(  Prome_LOG_ERROR, "%s: step failed: %s\n", __func__,
					  sqlite3_errmsg(db));
			ret = -1;
			break;
 	}
	//printf("counter = %d \n", counter);
	
	sqlite3_finalize(stmt);
	va_end(ap);
	
	return ret;
}



int sql_database_init(char *db_name, sqlite3 **ppDb)
{
	if( sqlite3_open(db_name, ppDb) != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Failed to open sqlite database!\n");
		return -1;
	}

	return 0;
}

int sql_database_deinit(char *db_name, sqlite3 *db)
{
	if( sqlite3_close(db) != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Failed to close sqlite database!\n");
		return -1;
	}

	return 0;
}



int sql_create_table(sqlite3 *db, char *table_name, 
					 sql_key *column, int col_num)
{
	int count = 0;
	int ret = 0;
	char db_cmd[DB_CMD_LEN];
	char sub_cmd[DB_CMD_LEN];
	char *zErrMsg = 0;
	int i = 0;
	sql_key *key = NULL;
	sql_key *pri_key = NULL;

	

	memset(db_cmd, 0, sizeof(db_cmd));
	sprintf(db_cmd, TABLE_COUNT, table_name);

	//printf("function:%s, line:%d\r\n", __FUNCTION__, __LINE__);
	count = sql_get_int_field(db, db_cmd);
	if(count == 0)
	{
		//printf("function:%s, line:%d\r\n", __FUNCTION__, __LINE__);
		memset(db_cmd, 0, sizeof(db_cmd));
		memset(sub_cmd, 0, sizeof(sub_cmd));
		pri_key = &column[0];
		sprintf(sub_cmd, CREATE_TABLE_PRIMARY_KEY, pri_key->key_str, 
				key_type_str[pri_key->key_type]);
		for(i = 1; i< col_num; i++)
		{
			key = &column[i];
			sprintf(sub_cmd, "%s, %s %s", sub_cmd, key->key_str, 
					key_type_str[key->key_type]);
		}
		sprintf(sub_cmd, "%s)", sub_cmd);

		sprintf(db_cmd, CREATE_TABLE, table_name, sub_cmd);
		//printf("-----db_cmd:%s\r\n", db_cmd);
		ret = sql_exec(db, NULL, NULL, db_cmd);
		if( ret != SQLITE_OK )
		{
			LOG_PRINT(  Prome_LOG_ERROR, "Error creating table %s!\n", table_name);
			return -1;
		}
	}
	
	return 0;
}

int sql_create_table_auto_key(sqlite3 *db, char *table_name, 
							  sql_key *column, int col_num)
{
	int count = 0;
	int ret = 0;
	char db_cmd[DB_CMD_LEN];
	char sub_cmd[DB_CMD_LEN];
	char *zErrMsg = 0;
	int i = 0;
	sql_key *key = NULL;
	sql_key *pri_key = NULL;

	

	memset(db_cmd, 0, sizeof(db_cmd));
	sprintf(db_cmd, TABLE_COUNT, table_name);

	//printf("function:%s, line:%d\r\n", __FUNCTION__, __LINE__);
	count = sql_get_int_field(db, db_cmd);
	if(count == 0)
	{
		//printf("function:%s, line:%d\r\n", __FUNCTION__, __LINE__);
		memset(db_cmd, 0, sizeof(db_cmd));
		memset(sub_cmd, 0, sizeof(sub_cmd));
		for(i = 0; i< col_num; i++)
		{
			key = &column[i];
			if(strlen(sub_cmd))
			{
				sprintf(sub_cmd, "%s, %s %s", sub_cmd, key->key_str, 
						key_type_str[key->key_type]);
			}
			else
			{
				sprintf(sub_cmd, "%s %s", key->key_str, 
						key_type_str[key->key_type]);
			}
		}
		sprintf(sub_cmd, "%s)", sub_cmd);

		sprintf(db_cmd, CREATE_TABLE_AUTO_PRIMARY_KEY, table_name, sub_cmd);
		//printf("-----db_cmd:%s\r\n", db_cmd);
		ret = sql_exec(db, NULL, NULL, db_cmd);
		if( ret != SQLITE_OK )
		{
			LOG_PRINT(  Prome_LOG_ERROR, "Error creating table %s!\n", table_name);
			return -1;
		}
	}
	
	return 0;
}


int sql_create_table_just_primary_key(sqlite3 *db, char *table_name, 
									  sql_key *pri_key)
{
	int count = 0;
	int ret = 0;
	char db_cmd[DB_CMD_LEN];
	char sub_cmd[DB_CMD_LEN];
	char *zErrMsg = 0;
	int i = 0;

	memset(db_cmd, 0, sizeof(db_cmd));
	memset(sub_cmd, 0, sizeof(sub_cmd));

	sprintf(sub_cmd, CREATE_TABLE_PRIMARY_KEY, pri_key->key_str, 
			key_type_str[pri_key->key_type]);
	sprintf(sub_cmd, "%s)", sub_cmd);
	sprintf(db_cmd, CREATE_TABLE, table_name, sub_cmd);
	ret = sql_exec(db, NULL, NULL, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error creating table %s!\n", table_name);
		return -1;
	}
	
	return 0;
}


int sql_add_column_to_table(sqlite3 *db, char *table_name, sql_key *column)
{
	char db_cmd[DB_CMD_LEN];
	int ret = 0;

	//check the events is or not exist
	memset(db_cmd, 0, sizeof(db_cmd));
	sprintf(db_cmd, TABLE_ADD_COLUMN, table_name, column->key_str, 
			key_type_str[column->key_type]);
	ret = sql_exec(db, NULL, NULL, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error add column %s to table %s !\n",
				  column->key_str, table_name);
		return -1;
	}

	return 0;
}

int sql_insert_column_data(sqlite3 *db, char *table_name, 
						   sql_key *column, int col_num)
{
	int count = 0;
	int ret = 0;
	char db_cmd[DB_CMD_LEN];
	char key_str[DB_KEY_LEN];
	char value_str[DB_VALUE_LEN];
	char *zErrMsg = 0;
	int i = 0;
	sql_key *key = NULL;
	sql_key *pri_key = NULL;

	memset(key_str, 0, sizeof(key_str));
	memset(value_str, 0, sizeof(value_str));
	pri_key = &column[0];
	sprintf(key_str, "(%s", pri_key->key_str);
	if(pri_key->key_type == DB_TYPE_INT || pri_key->key_type == DB_TYPE_REAL)
	{
		sprintf(value_str, "(%s", pri_key->key_value);
	}
	else
	{
		sprintf(value_str, "('%s'", pri_key->key_value);
	}

	for(i = 1; i< col_num; i++)
	{
		key = &column[i];
		sprintf(key_str, "%s,%s", key_str, key->key_str);
		if(key->key_type == DB_TYPE_INT || key->key_type == DB_TYPE_REAL)
		{
			sprintf(value_str, "%s,%s", value_str, key->key_value);
		}
		else
		{
			sprintf(value_str, "%s,'%s'", value_str, key->key_value);
		}
	}
	sprintf(key_str, "%s)", key_str);
	sprintf(value_str, "%s)", value_str);

	//printf("---------key_str:%s\r\n", key_str);
	//printf("---------value_str:%s\r\n", value_str);
	
	sprintf(db_cmd, TABLE_INSERT, table_name, key_str, value_str);
	ret = sql_exec(db, NULL, NULL, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error insert column to table %s!\n", table_name);
		return -1;
	}
	
	return 0;
}


int sql_insert_column_data_just_primary_key(sqlite3 *db, char *table_name, 
											sql_key *pri_key)
{
	char db_cmd[DB_CMD_LEN];
	int ret = 0;

	//check the events is or not exist
	memset(db_cmd, 0, sizeof(db_cmd));
	if(pri_key->key_type == DB_TYPE_INT || pri_key->key_type == DB_TYPE_REAL)
	{
		sprintf(db_cmd, TABLE_INSERT_PRIMARY_KEY_INT, table_name, 
				pri_key->key_str, pri_key->key_value);
	}
	else
	{
		sprintf(db_cmd, TABLE_INSERT_PRIMARY_KEY_TEXT, table_name, 
				pri_key->key_str, pri_key->key_value);
	}
	ret = sql_exec(db, NULL, NULL, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error insert  %s data to table %s !\n",
				  pri_key->key_str, table_name);
		return -1;
	}
	return 0;
}

int sql_update_key_data(sqlite3 *db, char *table_name, 
						sql_key *pri_key, sql_key *key)
{
	char db_cmd[DB_CMD_LEN];
	int ret = 0;

	//check the events is or not exist
	memset(db_cmd, 0, sizeof(db_cmd));
	if(pri_key->key_type == DB_TYPE_INT || pri_key->key_type == DB_TYPE_REAL)
	{
		if(key->key_type == DB_TYPE_INT || key->key_type == DB_TYPE_REAL)
		{
			sprintf(db_cmd, TABLE_UPDATE_PRIMARY_KEY_INT_KEY_INT, 
					table_name, key->key_str, key->key_value, pri_key->key_str, 
					pri_key->key_value);
		}
		else
		{
			sprintf(db_cmd, TABLE_UPDATE_PRIMARY_KEY_INT_KEY_TEXT, 
					table_name, key->key_str, key->key_value, pri_key->key_str, 
					pri_key->key_value);
		}
	}
	else
	{
		if(key->key_type == DB_TYPE_INT || key->key_type == DB_TYPE_REAL)
		{
			sprintf(db_cmd, TABLE_UPDATE_PRIMARY_KEY_TEXT_KEY_INT, 
					table_name, key->key_str, key->key_value, pri_key->key_str, 
					pri_key->key_value);
		}
		else
		{
			sprintf(db_cmd, TABLE_UPDATE_PRIMARY_KEY_TEXT_KEY_TEXT, 
					table_name, key->key_str, key->key_value, pri_key->key_str, 
					pri_key->key_value);
		}
	}
	//printf("db_cmd:%s\r\n", db_cmd);
	ret = sql_exec(db, NULL, NULL, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error insert  %s data to table %s !\n",
				  key->key_str, table_name);
		return -1;
	}
	return 0;
}

int sql_check_key_exist(sqlite3 *db, char *table_name, 
						sql_key *column, int col_num)
{
	char db_cmd[DB_CMD_LEN];
	char sub_cmd1[DB_CMD_LEN];
	char sub_cmd2[DB_CMD_LEN];
	int ret = 0;
	int i = 0;
	sql_key *key;

	//check the events is or not exist
	memset(db_cmd, 0, sizeof(db_cmd));

	if(col_num == 1)//单条件查找
	{
		if(column->key_type == DB_TYPE_INT || column->key_type == DB_TYPE_REAL)
		{
			sprintf(db_cmd, TABLE_SELECT_PRIMARY_KEY_COUNT_INT, 
					table_name, column->key_str, column->key_value);
		}
		else
		{
			sprintf(db_cmd, TABLE_SELECT_PRIMARY_KEY_COUNT_TEXT, 
					table_name, column->key_str, column->key_value);
		}
	}
	else if(col_num == 2)//多条件查找
	{
		key = &column[1];
		if(key->key_type == DB_TYPE_INT || key->key_type == DB_TYPE_REAL)
		{
			sprintf(sub_cmd1, TABLE_SELECT_PRIMARY_KEY_INT, 
					table_name, key->key_str, key->key_value);
		}
		else
		{
			sprintf(sub_cmd1, TABLE_SELECT_PRIMARY_KEY_INT, 
					table_name, key->key_str, key->key_value);
		}
		//printf("----sub_cmd1:%s\r\n", sub_cmd1);
		sprintf(sub_cmd2, "(%s)", sub_cmd1);
		//printf("####sub_cmd2:%s\r\n", sub_cmd2);

		key = &column[0];
		if(key->key_type == DB_TYPE_INT || key->key_type == DB_TYPE_REAL)
		{
			sprintf(db_cmd, TABLE_SELECT_PRIMARY_KEY_COUNT_INT, 
					sub_cmd2, key->key_str, key->key_value);
		}
		else
		{
			sprintf(db_cmd, TABLE_SELECT_PRIMARY_KEY_COUNT_TEXT, 
					sub_cmd2, key->key_str, key->key_value);
		}
	}
	//printf("db_cmd:%s\r\n", db_cmd);
	ret = sql_get_int_field(db, db_cmd);
	
	return ret;
}

int sql_search_key(sqlite3 *db, char *table_name, sql_key *pri_key, 
				   sql_callback callback, void *para)
{
	char db_cmd[DB_CMD_LEN];
	int ret = 0;

	//check the events is or not exist
	memset(db_cmd, 0, sizeof(db_cmd));
	if(pri_key->key_type == DB_TYPE_INT || pri_key->key_type == DB_TYPE_REAL)
	{
		sprintf(db_cmd, TABLE_SELECT_PRIMARY_KEY_INT, table_name, 
				pri_key->key_str, pri_key->key_value);
	}
	else
	{
		sprintf(db_cmd, TABLE_SELECT_PRIMARY_KEY_TEXT, table_name, 
				pri_key->key_str, pri_key->key_value);
	}

	//printf("db_cmd:%s\r\n", db_cmd);
	ret = sql_exec(db, callback, para, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error search table %s !\n", table_name);
		return -1;
	}
	
	return ret;
}

int sql_search_range(sqlite3 *db, char *table_name, sql_key *range, 
					 char *start, char *end, 
					 sql_key *column, int col_num,
					 sql_callback callback, void *para)
{
	char db_cmd[DB_CMD_LEN];
	int ret = 0, i = 0;
	sql_key *key = NULL;

	//check the events is or not exist
	memset(db_cmd, 0, sizeof(db_cmd));
	if(range->key_type == DB_TYPE_INT || range->key_type == DB_TYPE_REAL)
	{
		sprintf(db_cmd, TABLE_SELECT_RANGE_INT, table_name, 
				range->key_str, start, end);
	}
	else
	{
		sprintf(db_cmd, TABLE_SELECT_RANGE_TEXT, table_name, 
				range->key_str, start, end);
	}

	for(i = 0; i< col_num; i++)
	{
		key = &column[i];
		if(key->key_type == DB_TYPE_INT || key->key_type == DB_TYPE_REAL)
		{
			sprintf(db_cmd, "%s and %s = %s", db_cmd, key->key_str, 
					key->key_value);
		}
		else
		{
			sprintf(db_cmd, "%s and %s = '%s'", db_cmd, key->key_str, 
					key->key_value);
		}
	}

	//printf("db_cmd:%s\r\n", db_cmd);
	ret = sql_exec(db, callback, para, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error search table %s !\n", table_name);
		return -1;
	}
	
	return ret;
}

int sql_search_last_record(sqlite3 *db, char *table_name, sql_key *range, 
	 					   char *start, sql_key *column, int col_num,
	 					   sql_callback callback, void *para)
{
	char db_cmd[DB_CMD_LEN];
	int ret = 0, i = 0;
	sql_key *key = NULL;

	//check the events is or not exist
	memset(db_cmd, 0, sizeof(db_cmd));
	if(range->key_type == DB_TYPE_INT || range->key_type == DB_TYPE_REAL)
	{
		sprintf(db_cmd, TABLE_SELECT_LAST_RECORD_INT, table_name, 
		range->key_str, start);
	}
	else
	{
		sprintf(db_cmd, TABLE_SELECT_LAST_RECORD_TEXT, table_name, 
		range->key_str, start);
	}

	for(i = 0; i< col_num; i++)
	{
		key = &column[i];
		if(key->key_type == DB_TYPE_INT || key->key_type == DB_TYPE_REAL)
		{
			sprintf(db_cmd, "%s and %s = %s", db_cmd, key->key_str, 
			key->key_value);
		}
		else
		{
			sprintf(db_cmd, "%s and %s = '%s'", db_cmd, key->key_str, 
			key->key_value);
		}
	}

	sprintf(db_cmd, "%s order by %s desc limit 0,1", db_cmd, range->key_str);

	printf("db_cmd:%s\r\n", db_cmd);
	ret = sql_exec(db, callback, para, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error search table %s !\n", table_name);
		return -1;
	}

	return ret;
}


int sql_traversal_table(sqlite3 *db, char *table_name, sql_key *pri_key, 
						sql_callback callback, void *para, int order)
{
	char db_cmd[DB_CMD_LEN];
	int ret = 0;

	
	//check the events is or not exist
	memset(db_cmd, 0, sizeof(db_cmd));
	if(order == ORDER_BY_DESC)
		sprintf(db_cmd, TABLE_TRAVERSAL_BY_DESC, table_name, pri_key->key_str);
	else
		sprintf(db_cmd, TABLE_TRAVERSAL_BY_ASC, table_name, pri_key->key_str);
	
	ret = sql_exec(db, callback, para, db_cmd);
	if( ret != SQLITE_OK )
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Error traversal table %s !\n", table_name);
		return -1;
	}

	return 0;
}

#if 0
int callback_test(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	
	for(i=0; i<argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}



int main(int argc, char* argv[])
{
	char *db_name = "device.db";
	char *table_name1 = "device_list";
	char *primary_key1 = "deviceId";
	int key_type = DB_TYPE_TEXT;
	sql_key column1[4];
	sql_key pri_key1;
	int ret = 0;

	char *table_name2 = "event_list";
	char *primary_key2 = "eventId";
	sql_key pri_key2;
	sql_key column2[4];
	
	sql_database_init(db_name);

	memset(&pri_key1, 0, sizeof(pri_key1));
	sprintf(pri_key1.key_str, "%s", primary_key1);
	pri_key1.key_type = DB_TYPE_TEXT;
	pri_key1.key_value = "12345";

	memset(&column1[0], 0, sizeof(column1[0]));
	sprintf(column1[0].key_str, "%s", "sn");
	column1[0].key_type = DB_TYPE_TEXT;
	column1[0].key_value = "0001";

	memset(&column1[1], 0, sizeof(column1[1]));
	sprintf(column1[1].key_str, "%s", "mac");
	column1[1].key_type = DB_TYPE_TEXT;
	column1[1].key_value = "11:22:33:44:55:66";

	memset(&column1[2], 0, sizeof(column1[2]));
	sprintf(column1[2].key_str, "%s", "volt");
	column1[2].key_type = DB_TYPE_INT;
	column1[2].key_value = "123";
	
	sql_create_table_just_primary_key(table_name1, &pri_key1);
	sql_add_column_to_table(table_name1, &column1[0]);
	sql_add_column_to_table(table_name1, &column1[1]);
	sql_add_column_to_table(table_name1, &column1[2]);

	
	sql_insert_column_data_just_primary_key(table_name1, &pri_key1);
	sql_update_key_data(table_name1, &pri_key1, &column1[0]);
	sql_update_key_data(table_name1, &pri_key1, &column1[1]);
	sql_update_key_data(table_name1, &pri_key1, &column1[2]);


	memset(&pri_key1, 0, sizeof(pri_key1));
	sprintf(pri_key1.key_str, "%s", primary_key1);
	pri_key1.key_type = DB_TYPE_TEXT;
	pri_key1.key_value = "1234";
	ret = sql_search_key(table_name1, &pri_key1);
	printf("ret:%d\r\n", ret);

	sql_traversal_table(table_name1, callback_test);


	memset(&pri_key2, 0, sizeof(pri_key2));
	sprintf(pri_key2.key_str, "%s", primary_key2);
	pri_key2.key_type = DB_TYPE_TEXT;
	pri_key2.key_value = "eventid1";

	memset(&column2[0], 0, sizeof(column2[0]));
	sprintf(column2[0].key_str, "%s", "deviceid");
	column2[0].key_type = DB_TYPE_TEXT;
	column2[0].key_value = "1234";

	memset(&column2[1], 0, sizeof(column2[1]));
	sprintf(column2[1].key_str, "%s", "eventtype");
	column2[1].key_type = DB_TYPE_TEXT;
	column2[1].key_value = "设备移动";

	memset(&column2[2], 0, sizeof(column2[2]));
	sprintf(column2[2].key_str, "%s", "time");
	column2[2].key_type = DB_TYPE_INT;
	column2[2].key_value = "234567";

	sql_create_table(table_name2, &pri_key2, column2, 3);
	sql_insert_column_data(table_name2, &pri_key2, column2, 3);
	sql_traversal_table(table_name2, callback_test);

	return 0;
}
#endif


