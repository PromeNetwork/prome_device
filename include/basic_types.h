/***********************************************************************************
 * @file   			: basic_types.h

 * @date   			: 2020-9-15
 * @Description   	: This document mainly defines the basic data type, data
                     structure, macro and data enumeration in the SDK

 * @other   		: 
 * @note History 	: 
***********************************************************************************/

#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <inttypes.h>
#include <pthread.h> 
#include <stdlib.h>


//#include "log.h"

//#define DEBUG

/*Macro definition*/
#ifndef FALSE
#define FALSE  0
#endif

#ifndef TRUE
#define TRUE   1
#endif

#ifdef	DEBUG
#define DEBUG_PRINT(fmt, args...) printf(fmt, ##args);
#else
#define DEBUG_PRINT(fmt, args...)
#endif	/* DEBUG */

#define DEBUG_PRINT_LINE()	printf("function:%s, line:%d\r\n", __FUNCTION__, __LINE__)
//#define DEBUG_PRINT_LINE()
#define   PromeBC_LOCK(lock)		{\
								if((lock) != NULL)\
								{\
									pthread_mutex_lock(lock);\
								}\
							}

#define   PromeBC_UNLOCK(lock)	{\
								if((lock) != NULL)\
								{\
									pthread_mutex_unlock(lock);\
								}\
							}


#ifndef UINT8
typedef unsigned char   	UINT8;
#endif

#ifndef UINT16
typedef unsigned short  	UINT16;
#endif

#ifndef UINT32
typedef unsigned int  		UINT32;
#endif

#ifndef UINT64
typedef unsigned long long int	UINT64;
#endif

#ifndef INT8
typedef signed char 		INT8;
#endif

#ifndef INT16
typedef signed short  		INT16;
#endif

#ifndef INT32
typedef signed int  		INT32;
#endif

#ifndef INT64
typedef int64_t          	INT64;
#endif

#ifndef TIMESTAMP
typedef unsigned long long  TIMESTAMP;
#endif

#ifndef TIMESTAMP_TO_STR
#define TIMESTAMP_TO_STR(timestamp, str) sprintf((char *)str, "%llu", timestamp)
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(PTR)	if(PTR){free(PTR);PTR=NULL;}
#endif

#define xstr(s) str(s)
#define str(s) #s

#if defined(CFG_DIR)
#define ROOT_DIR xstr(CFG_DIR)
#else
#define ROOT_DIR	"."
#endif
#define AES_KEY_DEFAULT	" _aes"


#if defined(DEV_AES_KEY)
#define AES_KEY_DEV xstr(DEV_AES_KEY)
#else
#define AES_KEY_DEV	AES_KEY_DEFAULT
#endif

#if defined(MQTT_TLS_AUTH)
#define MQTT_TLS_AUTH_DIR xstr(MQTT_TLS_AUTH)
#else
#define MQTT_TLS_AUTH_DIR	NULL
#endif


#define PUB_KEY_MAX_LEN			65
#define PRI_KEY_MAX_LEN			32
#define SIGN_MAX_LEN			65
#define HASH_MAX_LEN			32
#define ACCESS_KEY_LEN			64
#define FILE_NAME_MAX_LEN		128
#define ALG_NAME_MAX_LEN		32


#define TOKEN_LEN			64
#define DOMAIN_LEN			64


#define PUB_KEY_STR_MAX_LEN		(PUB_KEY_MAX_LEN * 2 + 1)
#define PRI_KEY_STR_MAX_LEN		(PRI_KEY_MAX_LEN * 2 + 1)
#define SIGN_STR_MAX_LEN		(SIGN_MAX_LEN * 2 + 1)
#define HASH_STR_MAX_LEN		(HASH_MAX_LEN * 2 + 1)

#define TIMESTAMP_STR_MAX_LEN	32
#define URL_MAX_LEN	256

#define RETURN_DESC_LEN	256

#define MESSAGE_MAX_LENGTH				(1024*4)
#define DEV_ID_MAX_LEN			32

#define DB_FILE_MAX_SIZE				(1024*1024*10) //10M

/*Error code define*/
typedef enum
{
	/*The following error code is the client local error code.				*/
	ERR_SUCCESS					= 0,	 /*success							*/
	ERR_FILE_OPEN_FAILED		= 1001,	 /*file open fail					*/
	ERR_FILE_READ_FAILED		= 1002,	 /*file read error					*/
	ERR_FILE_IS_NULL			= 1003,	 /*file size is 0					*/
	ERR_HASH_METHOD_NOT_SUPPORT	= 2001,	 /*Hash algorithm not supported		*/
	ERR_HASH_FAIL				= 2002,	 /*Hash calculation failed			*/
	ERR_SIGN_FAIL				= 2003,	 /*Signature calculation failed		*/
	ERR_SIGN_METHOD_NOT_SUPPORT	= 2004,	 /*sign algorithm not supported		*/
	ERR_MALLOC_FAIL				= 3001,  /*malloc fail, maybe not 			*/
										 /*enough heap memory				*/
	ERR_BASE64_ENCODE_FAIL		= 4001,	 /*base64 encode fail				*/
	ERR_BASE64_DECODE_FAIL		= 4002,	 /*base64 decode fail				*/
	ERR_DATA_ASSEMBLE_FAIL		= 5001,  /*data assemble fail				*/
	ERR_POE_TYPE_NOT_SUPPORT	= 6001,	 /*POE type not supported			*/
	ERR_KEY_GENERATE_FAIL		= 7001,
	ERR_ENCRYPT_FAIL			= 8001,	 /*Encrypt calculation failed		*/
	ERR_DECRYPT_FAIL			= 8002,	 /*Decrypt calculation failed		*/
	ERR_ENCRYPT_METHOD_NOT_SUPPORT= 8003,/*Encrypt algorithm not supported	*/

	/*The following error code is consistent with the server				*/
	ERR_REQUEST_SUCCESS			= 17000, /*Request success/response ok		*/
	ERR_PARAMETER_CHECK_FAILED 	= 12010, /*Parameter check failed, maybe	*/ 
										 /*parameter is null or content 	*/
										 /*length is too long				*/
	ERR_FILE_UPLOAD_FAILED		= 18010, /*File upload failed				*/
	ERR_FILE_DOWNLOAD_FAILED	= 18020, /*File download failed				*/
	ERR_REQUEST_TIMEOUT			= 20001, /*request timeout, The timestamp 	*/
										 /*is 2 minute longer				*/
	ERR_INCORRECT_ACCESSKEYID  	= 20003, /*incorrect accessKeyId, Identity 	*/
										 /*check failed						*/
	ERR_ACCESS_CLOSED			= 20005, /*access closed, The uplink  		*/
										 /*function has been suspended, 	*/
										 /*please contact customer service	*/
	ERR_VERIFY_SIGN_FAILED		= 20006, /*verify sign failed				*/
	ERR_CONTENT_STORAGE_FAILED 	= 20007, /*content storage failed, please 	*/
										 /*try again						*/
	ERR_POE_HASH_INCORRECT		= 20009, /*poe hash incorrect				*/
	ERR_FILE_SIZE_EXCEEDED		= 20010, /*file size exceeded, file size no */
										 /*larger than 30 MB				*/
	ERR_POE_TYPE_MISMATCH		= 20011, /*poe type mismatch, check your poe*/
										 /*type if match sdk method			*/
	ERR_POE_NOT_AVAILABLE		= 20012, /*poe not available, try later or 	*/
										 /*contact admin					*/
}ERR_CODE;

/*poe save result data structure*/
typedef struct _DATA_ONCHAIN_RESPONE
{
	ERR_CODE	SaveRetCode;				/*Return code of Poe save	*/
	UINT8		SavePoeHash[HASH_MAX_LEN];	/*The Hash value of Poe save*/
											/*we can use this hash value*/ 
											/*for Poe query				*/
	UINT32		HashLen;
	UINT8		SaveRetDesc[RETURN_DESC_LEN];	/*Return code description	*/
}DATA_ONCHAIN_RESPONE ,*P_DATA_ONCHAIN_RESPONE; 


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif


