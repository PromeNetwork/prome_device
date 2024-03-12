#ifndef __CSQLITE3_H__
#define __CSQLITE3_H__

#include <sqlite3.h>


#define DB_TYPE_NULL		0	//值是一个 NULL 值。
#define DB_TYPE_INT			1	//值是一个带符号的整数，根据值的大小存储在 1、2、3、4、6 或 8 字节中。
#define DB_TYPE_REAL		2	//值是一个浮点值，存储为 8 字节的 IEEE 浮点数字。
#define DB_TYPE_TEXT		3	//值是一个文本字符串，使用数据库编码（UTF-8、UTF-16BE 或 UTF-16LE）存储。
#define DB_TYPE_BLOB		4	//值是一个 blob 数据，完全根据它的输入存储。

#define ORDER_BY_DESC		0
#define ORDER_BY_ASC		1

#define DB_CMD_LEN			2048
#define DB_KEY_LEN			1024
#define DB_VALUE_LEN		1024

typedef struct _sql_key_t
{
	int key_type;
	char key_str[32];
	char *key_value;
}sql_key;

typedef int (*sql_callback)(void *para, int argc, char **argv, char **azColName);

/********************************************************************************
 * Function	    : sql_database_init		                       	               	*
 * Input	    : char *db_name - 数据库名字                  						*
 * Output	    : 无		                   										*
 * Return	    : 成功返回0，失败返回错误码                   								*
 * Comments	    : 创建数据库				                           					*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_database_init(char *db_name, sqlite3 **ppDb);
int sql_database_deinit(char *db_name, sqlite3 *db);


/********************************************************************************
 * Function	    : sql_create_table		                       	               	*
 * Input	    : char *table_name - 数据库表名字                  					*
 *		        : sql_key *column -  数据库表列相关信息			                   		*
 *				: 注意：column[0]表示为主键												*
 *		        : int col_num -      数据库表列数              						*
 * Output	    : 无		                   										*
 * Return	    : 成功返回0，失败返回错误码                   								*
 * Comments	    : 创建数据库表				                           				*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_create_table(sqlite3 *db, char *table_name, sql_key *column, int col_num);
int sql_create_table_auto_key(sqlite3 *db, char *table_name, sql_key *column, int col_num);

/********************************************************************************
 * Function	    : sql_create_table_just_primary_key	           	               	*
 * Input	    : char *table_name - 数据库表名字                  					*
 *		        : sql_key *pri_key - 数据库表主键信息		               				*
 * Output	    : 无		                   										*
 * Return	    : 成功返回0，失败返回错误码                   								*
 * Comments	    : 创建一个只含主键的数据库表				                           			*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_create_table_just_primary_key(sqlite3 *db, char *table_name, sql_key *pri_key);

/********************************************************************************
 * Function	    : sql_add_column_to_table                      	             	*
 * Input	    : char *table_name - 数据库表名字                  					*
 *		        : sql_key *column -  待增加的列信息		               				*
 * Output	    : 无		                   										*
 * Return	    : 成功返回0，失败返回错误码                   								*
 * Comments	    : 向	数据库表	table_name中增加一列		                           		*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_add_column_to_table(sqlite3 *db, char *table_name, sql_key *column);

/********************************************************************************
 * Function	    : sql_insert_column_data                       	               	*
 * Input	    : char *table_name - 数据库表名字                  					*
 *		        : sql_key *pri_key - 数据库表主键信息(含数据)	               				*
 *		        : sql_key *column -  数据库表列相关信息(含数据)		                   		*
 *		        : int col_num -      数据库表列数              						*
 * Output	    : 无		                   										*
 * Return	    : 成功返回0，失败返回错误码                   								*
 * Comments	    : 向	数据库表	table_name中插入一列数据	                           		*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_insert_column_data(sqlite3 *db, char *table_name, sql_key *column, int col_num);

/********************************************************************************
 * Function	    : sql_insert_column_data_just_primary_key      	               	*
 * Input	    : char *table_name - 数据库表名字                  					*
 *		        : sql_key *pri_key - 数据库表主键信息(含数据)	               				*
 * Output	    : 无		                   										*
 * Return	    : 成功返回0，失败返回错误码                   								*
 * Comments	    : 向	数据库表	table_name中插入一列数据，此列数据只含主键的相关数据           			*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_insert_column_data_just_primary_key(sqlite3 *db, char *table_name, sql_key *pri_key);

/********************************************************************************
 * Function	    : sql_update_key_data					      	               	*
 * Input	    : char *table_name - 数据库表名字                  					*
 *		        : sql_key *pri_key - 数据库表主键信息(含数据)	               				*
 *		        : sql_key *key - 数据库表主键信息(含数据)		               				*
 * Output	    : 无		                   										*
 * Return	    : 成功返回0，失败返回错误码                   								*
 * Comments	    : 更新表table_name中某一列数据						           			*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_update_key_data(sqlite3 *db, char *table_name, sql_key *pri_key, sql_key *key);

/********************************************************************************
 * Function	    : sql_search_key						      	               	*
 * Input	    : char *table_name - 数据库表名字                  					*
 *		        : sql_key *pri_key - 数据库表主键信息(含数据)	             				*
 *		        : sql_callback *callback - 回调函数		               				*
 *		        : void *para - callback函数参数，带回查找结果             					*
 * Output	    : 无		                   										*
 * Return	    : 存在返回1，不存在返回0                   								*
 * Comments	    : 查找表table_name中	是否存在	pri_key信息		           			*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_search_key(sqlite3 *db, char *table_name, sql_key *pri_key, sql_callback callback, void *para);

/********************************************************************************
 * Function	    : sql_search_key						      	               	*
 * Input	    : char *table_name - 数据库表名字                  					*
 *		        : sql_key *pri_key - 数据库表主键信息(含数据)	               				*
 * Output	    : 无		                   										*
 * Return	    : 存在返回1，不存在返回0                   								*
 * Comments	    : 查找表table_name中	是否存在	pri_key信息		           			*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_check_key_exist(sqlite3 *db, char *table_name, sql_key *column, int col_num);

/********************************************************************************
 * Function	    : sql_traversal_table					      	               	*
 * Input	    : char *table_name -      数据库表名字                  				*
 *		        : sql_key *pri_key - 数据库表主键信息(含数据)	               				*
 *		        : sql_callback callback - 返回信息处理回调函数            				*
 * Output	    : 无		                   										*
 * Return	    : 成功返回0，失败返回错误码                   								*
 * Comments	    : 遍历表table_name并按照主键降序排序，返回信息由callback函数处理	           			*
 * First Make	: 2019-11-30 xqchen							                   	*
 ********************************************************************************/ 
int sql_traversal_table(sqlite3 *db, char *table_name, sql_key *pri_key, sql_callback callback, void *para, int order);

int sql_search_range(sqlite3 *db, char *table_name, sql_key *range, char *start, char *end, 
					 sql_key *column, int col_num,
					 sql_callback callback, void *para);

int sql_search_last_record(sqlite3 *db, char *table_name, sql_key *range, 
						   char *start, sql_key *column, int col_num,
						   sql_callback callback, void *para);

#endif

