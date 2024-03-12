

#ifndef __PRE_COMM_H__
#define __PRE_COMM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "mosquitto_api.h"


#define PRE_ROLE_SENDER_STR 	"sender"
#define PRE_ROLE_PROXY_STR 		"proxy"
#define PRE_ROLE_RECEIVER_STR 	"receiver"

#define PRE_METHOD_REQUEST_STR	"request"
#define PRE_METHOD_RESPONSE_STR	"respone"

#define PRE_ROLE_TYPE_MAX_LEN	32
#define PRE_METHOD_MAX_LEN		32


typedef enum
{
	PRE_ROLE_SENDER,	
	PRE_ROLE_PROXY,	
	PRE_ROLE_RECEIVER,	
	PRE_ROLE_DEFAULT,	
}PRE_ROLE_TYPE_E;

typedef enum
{
	PRE_METHOD_REQUEST,	
	PRE_METHOD_RESPONSE,	
	PRE_METHOD_DEFAULT,
}PRE_METHOD_TYPE_E;


typedef struct _PRE_TOPIC_INFO
{
	PRE_ROLE_TYPE_E 	role;
	PRE_METHOD_TYPE_E	method;
	UINT64				session_id;
}PRE_TOPIC_INFO, *P_PRE_TOPIC_INFO;

typedef struct _PRE_TOPIC_PARAM
{
	void *key;
	PRE_ROLE_TYPE_E 	role;
	MQTT_CONNECT_PARAM 	mqtt;
}PRE_TOPIC_PARAM, *P_PRE_TOPIC_PARAM;


void PreSessionidCreate(char *sessionid);
UINT32 PreTopicParse(UINT8 *topic, PRE_TOPIC_INFO *topic_info);
PRE_METHOD_TYPE_E PRE_METHOD_TYPE_INT(UINT8 *type);
UINT8 *PRE_METHOD_TYPE_STR(PRE_METHOD_TYPE_E type);
PRE_ROLE_TYPE_E PRE_ROLE_TYPE_INT(UINT8 *type);
UINT8 *PRE_ROLE_TYPE_STR(PRE_ROLE_TYPE_E type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PRE_COMM_H__ */
