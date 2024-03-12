#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_json.h"
#include "dev_pkg.h"

#include "log.h"
#include "hexutils.h"
#include "mosquitto_api.h"
#include "gateway.h"
#include "cJSON.h"

#if 0
typedef unsigned char   	UINT8;
typedef unsigned int  		UINT32;

typedef unsigned long long  UINT64;
#define SAFE_FREE(PTR)	if(PTR){free(PTR);PTR=NULL;}
#endif

#define MQTT_TOPIC_TYPE_CONNECT_STR			"connect"
#define MQTT_TOPIC_TYPE_DISCONNECT_STR     	"disconnect"
#define MQTT_TOPIC_TYPE_REQUEST_STR	       	"request"
#define MQTT_TOPIC_TYPE_RESPONSE_STR	   	"response"
#define MQTT_TOPIC_TYPE_ATTRIBUTE_STR      	"attribute"
#define MQTT_TOPIC_TYPE_REPORT_STR		   	"report"
#define MQTT_TOPIC_TYPE_RPC_STR			   	"rpc"
#define MQTT_TOPIC_TYPE_RESULT_STR		   	"result"

#define RPC_METHOD_MQTT_STR			"changeMQTTBroker"
#define RPC_METHOD_UPGRADE_STR		"upgradeBoxGateway"
#define RPC_METHOD_DEV_INFO_STR		"getDeviceInfo"

#define MQTT_TOPIC_TYPE_MAX_LEN		32
#define RPC_METHOD_MAX_LEN			32


typedef enum
{
	MQTT_TOPIC_TYPE_CONNECT 			= 1,
	MQTT_TOPIC_TYPE_DISCONNECT,
	MQTT_TOPIC_TYPE_REQUEST,	
	MQTT_TOPIC_TYPE_RESPONSE,	
	MQTT_TOPIC_TYPE_ATTRIBUTE,
	MQTT_TOPIC_TYPE_REPORT,		
	MQTT_TOPIC_TYPE_RPC,			
	MQTT_TOPIC_TYPE_RESULT,
	MQTT_TOPIC_TYPE_DEFAULT
}MQTT_TOPIC_TYPE_E;

typedef enum
{
	RPC_METHOD_TYPE_MQTT 			= 1,
	RPC_METHOD_TYPE_UPGRADE,
	RPC_METHOD_TYPE_DEV_INFO,	
	RPC_METHOD_TYPE_DEFAULT
}RPC_METHOD_E;

typedef struct _MQTT_TOPIC_INFO
{
	MQTT_TOPIC_TYPE_E 	topic_type;
	RPC_METHOD_E  		rpc_method;
	UINT64				session_id;
}MQTT_TOPIC_INFO, *P_MQTT_TOPIC_INFO;



static UINT8 *MQTT_TOPIC_TYPE_STR(MQTT_TOPIC_TYPE_E type)
{
	UINT8 *tmp = NULL;
	
	switch(type)
	{
		case MQTT_TOPIC_TYPE_CONNECT:
			tmp = (UINT8 *)MQTT_TOPIC_TYPE_CONNECT_STR;
			break;
		case MQTT_TOPIC_TYPE_DISCONNECT:
			tmp = (UINT8 *)MQTT_TOPIC_TYPE_DISCONNECT_STR;
			break;
		case MQTT_TOPIC_TYPE_REQUEST:
			tmp = (UINT8 *)MQTT_TOPIC_TYPE_REQUEST_STR;
			break;
		case MQTT_TOPIC_TYPE_RESPONSE:
			tmp = (UINT8 *)MQTT_TOPIC_TYPE_RESPONSE_STR;
			break;
		case MQTT_TOPIC_TYPE_ATTRIBUTE:
			tmp = (UINT8 *)MQTT_TOPIC_TYPE_ATTRIBUTE_STR;
			break;
		case MQTT_TOPIC_TYPE_REPORT:
			tmp = (UINT8 *)MQTT_TOPIC_TYPE_REPORT_STR;
			break;	
		case MQTT_TOPIC_TYPE_RPC:
			tmp = (UINT8 *)MQTT_TOPIC_TYPE_RPC_STR;
			break;	
		case MQTT_TOPIC_TYPE_RESULT:
			tmp = (UINT8 *)MQTT_TOPIC_TYPE_RESULT_STR;
			break;
	}
	return tmp;
}

static MQTT_TOPIC_TYPE_E MQTT_TOPIC_TYPE_INT(UINT8 *type)
{
	if(strstr(type, MQTT_TOPIC_TYPE_CONNECT_STR))
	{
		return MQTT_TOPIC_TYPE_CONNECT;
	}
	else if(strstr(type, MQTT_TOPIC_TYPE_DISCONNECT_STR))
	{
		return MQTT_TOPIC_TYPE_DISCONNECT;
	}
	else if(strstr(type, MQTT_TOPIC_TYPE_REQUEST_STR))
	{
		return MQTT_TOPIC_TYPE_REQUEST;
	}
	else if(strstr(type, MQTT_TOPIC_TYPE_RESPONSE_STR))
	{
		return MQTT_TOPIC_TYPE_RESPONSE;
	}
	else if(strstr(type, MQTT_TOPIC_TYPE_ATTRIBUTE_STR))
	{
		return MQTT_TOPIC_TYPE_ATTRIBUTE;
	}
	else if(strstr(type, MQTT_TOPIC_TYPE_REPORT_STR))
	{
		return MQTT_TOPIC_TYPE_REPORT;
	}
	else if(strstr(type, MQTT_TOPIC_TYPE_RPC_STR))
	{
		return MQTT_TOPIC_TYPE_RPC;
	}
	else if(strstr(type, MQTT_TOPIC_TYPE_RESULT_STR))
	{
		return MQTT_TOPIC_TYPE_RESULT;
	}
	else
	{
		return MQTT_TOPIC_TYPE_DEFAULT;
	}
}

static UINT8 *RPC_METHOD_STR(RPC_METHOD_E type)
{
	UINT8 *tmp = NULL;
	
	switch(type)
	{
		case RPC_METHOD_TYPE_MQTT :
			tmp = (UINT8 *)RPC_METHOD_MQTT_STR;
			break;
		case RPC_METHOD_TYPE_UPGRADE:
			tmp = (UINT8 *)RPC_METHOD_UPGRADE_STR;
			break;
		case RPC_METHOD_TYPE_DEV_INFO:
			tmp = (UINT8 *)RPC_METHOD_DEV_INFO_STR;
			break;	
		case RPC_METHOD_TYPE_DEFAULT:
			break;
	}
	return tmp;
}

static RPC_METHOD_E RPC_METHOD_INT(UINT8 *type)
{
	if(strstr(type, RPC_METHOD_MQTT_STR))
	{
		return RPC_METHOD_TYPE_MQTT;
	}
	else if(strstr(type, RPC_METHOD_UPGRADE_STR))
	{
		return RPC_METHOD_TYPE_UPGRADE;
	}
	else if(strstr(type, RPC_METHOD_DEV_INFO_STR))
	{
		return RPC_METHOD_TYPE_DEV_INFO;
	}
	else
	{
		return RPC_METHOD_TYPE_DEFAULT;
	}
}

static UINT64 mqtt_session_get(UINT8 *topic)
{
	UINT8 *tmp = NULL;
	UINT64 id = 0;


	tmp = strrchr(topic, '/');
	if(tmp == NULL)
	{
		return 0;
	}
	
	sscanf(tmp, "/%llu", &id);
	
	return id;
}

static MQTT_TOPIC_TYPE_E mqtt_topic_type_get(UINT8 *topic) 
{
	UINT8 tmp[MQTT_TOPIC_TYPE_MAX_LEN];
	MQTT_TOPIC_TYPE_E topic_type = MQTT_TOPIC_TYPE_DEFAULT;
	
	if(topic == NULL)
	{
		return MQTT_TOPIC_TYPE_DEFAULT;
	}

	memset(tmp, 0, sizeof(tmp));
  
  	sscanf(topic,"%*[^/]/%[^/]",tmp);
	printf("tmp:%s\r\n", tmp);
	if(strlen(tmp))
	{
		topic_type = MQTT_TOPIC_TYPE_INT(tmp);
	}

	return topic_type;

	#if 0
	printf("type_str1:%s\r\n", type_str);

	memset(type_str, 0, sizeof(type_str));
	sscanf(topic,"%*[^/]/%*[^/]/%[^/]",type_str);
	printf("type_str2:%s\r\n", type_str);

	memset(type_str, 0, sizeof(type_str));
	sscanf(topic,"%*[^/]/%*[^/]/%*[^/]/%[^/]",type_str);
	printf("type_str3:%s\r\n", type_str);
	//printf("sub_topic:%s\r\n", sub_topic);
	#endif
	
}

static RPC_METHOD_E mqtt_rpc_method_get(UINT8 *topic) 
{
	UINT8 tmp[RPC_METHOD_MAX_LEN];
	RPC_METHOD_E method = RPC_METHOD_TYPE_DEFAULT;

	if(topic == NULL)
	{
		return RPC_METHOD_TYPE_DEFAULT;
	}

	memset(tmp, 0, sizeof(tmp));

	//devices/rpc/request/aaa/upgradeBoxGateway/123
  	sscanf(topic,"%*[^/]/%*[^/]/%*[^/]/%*[^/]/%[^/]",tmp);
	printf("tmp:%s\r\n", tmp);
	if(strlen(tmp))
	{
		method = RPC_METHOD_INT(tmp);
	}

	return method;
	
}



static UINT32 mqtt_receiver_topic_parse(UINT8 *topic, MQTT_TOPIC_INFO *topic_info)
{
	if(topic == NULL || topic_info == NULL)
	{
		return 1;
	}

	topic_info->topic_type = mqtt_topic_type_get(topic);
	switch(topic_info->topic_type)
	{
		case MQTT_TOPIC_TYPE_CONNECT:
			break;
		case MQTT_TOPIC_TYPE_DISCONNECT:
			break;
		case MQTT_TOPIC_TYPE_REQUEST:
			break;
		case MQTT_TOPIC_TYPE_RESPONSE:
			break;
		case MQTT_TOPIC_TYPE_ATTRIBUTE:
			break;
		case MQTT_TOPIC_TYPE_REPORT:
			break;	
		case MQTT_TOPIC_TYPE_RPC:
			topic_info->rpc_method = mqtt_rpc_method_get(topic);
			topic_info->session_id = mqtt_session_get(topic);
			break;	
		case MQTT_TOPIC_TYPE_RESULT:
			break;
	}

	return 0;
}

/*****************************************************************************
 * @Function	: dev_cfg_receiver_cb
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 设备配置订阅回调函数
 * @param[in]  	: void *data       配置数据
               	  UINT32 data_len  数据长度
               	  char *topic      MQTT topic
               	  void *para       设备句柄
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: ATTRIBUTE

*****************************************************************************/
static int mqtt_attribute_process(void *handle,
								  void *data, 
								  UINT32 data_len, 
								  MQTT_TOPIC_INFO *topic_info)
{
	DEV_DRIVER *dev = NULL;

	dev = (DEV_DRIVER *)handle;

	return dev->dev_cfg_cb(dev, data, data_len);
}


static INT32 mqtt_rpc_process(void *handle,
							  void *data, 
							  UINT32 data_len, 
							  MQTT_TOPIC_INFO *topic_info)
{
	DEV_DRIVER *dev = NULL;
	INT32 ret = 0;
	INT32 need_restart = 0;

	dev = (DEV_DRIVER *)handle;
	switch(topic_info->rpc_method)
	{
		case RPC_METHOD_TYPE_MQTT :
			if(dev->dev_mqtt_cfg_cb)
			{
				need_restart = dev->dev_mqtt_cfg_cb(handle, data, data_len, topic_info->session_id);
			}
			break;
		case RPC_METHOD_TYPE_UPGRADE:
			GatewayUpgradeCfgCb(handle, data, data_len, topic_info->session_id);
			break;
		case RPC_METHOD_TYPE_DEV_INFO:
			break;	
		case RPC_METHOD_TYPE_DEFAULT:
			break;
	}

	#if 0
	ret = DevicePkgRpcResp(handle, data, data_len, topic_info->session_id, 
						   PKG_TYPE_DEV_RPC_MQTT_RESP);
	if(ret)
	{
		LOG_PRINT(WX_LOG_ERROR, "Device %s parse mqtt config fail.", dev->device_id);
	}
	#endif

	if(need_restart)
	{
		DevicePowerOff(handle);
	}
}

/*****************************************************************************
 * @Function	: dev_cfg_receiver_cb
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 设备配置订阅回调函数
 * @param[in]  	: void *data       配置数据
               	  UINT32 data_len  数据长度
               	  char *topic      MQTT topic
               	  void *para       设备句柄
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static INT32 mqtt_receiver_cb(void *data, UINT32 data_len, char *topic, void *para)
{
	DEV_DRIVER *dev = NULL;
	MQTT_TOPIC_INFO topic_info;
	UINT32 ret = 0;

	if(para == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)para;

	memset(&topic_info, 0, sizeof(topic_info));
	ret = mqtt_receiver_topic_parse(topic, &topic_info);
	if(ret)
	{
		return 1;
	}

	switch(topic_info.topic_type)
	{
		case MQTT_TOPIC_TYPE_CONNECT:
			break;
		case MQTT_TOPIC_TYPE_DISCONNECT:
			break;
		case MQTT_TOPIC_TYPE_REQUEST:
			break;
		case MQTT_TOPIC_TYPE_RESPONSE:
			break;
		case MQTT_TOPIC_TYPE_ATTRIBUTE:
			mqtt_attribute_process(para, data, data_len, &topic_info);
			break;
		case MQTT_TOPIC_TYPE_REPORT:
			break;	
		case MQTT_TOPIC_TYPE_RPC:
			mqtt_rpc_process(para, data, data_len, &topic_info);
			break;	
		case MQTT_TOPIC_TYPE_RESULT:
			break;
	}
	

	return 0;
}

/*****************************************************************************
 * @Function	: dev_cfg_receiver
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 设备配置订阅函数
 * @param[in]  	: void *ptr  设备句柄
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static void *device_mqtt_connect(void *ptr)
{
	UINT32 ret = 0;
	UINT8 topic[256];
	UINT8 *topic_list = NULL;
	DEV_DRIVER *dev = NULL;
	cJSON *pArr = NULL;
	MQTT_CONNECT_PARAM 	*tmp = NULL;

	if(ptr == NULL)
	{
		return NULL;
	}

	dev = (DEV_DRIVER *)ptr;

	pArr = cJSON_CreateArray();
	if(pArr == NULL)
	{
		return NULL;
	}

	memset(topic, 0, sizeof(topic));
	snprintf(topic, sizeof(topic), "devices/rpc/request/%s/#", dev->dev_pubkey);
	cJSON_AddItemToArray(pArr, cJSON_CreateString(topic));

	memset(topic, 0, sizeof(topic));
	snprintf(topic, sizeof(topic), "devices/attribute/%s/#", dev->dev_pubkey);
	cJSON_AddItemToArray(pArr, cJSON_CreateString(topic));
	
	topic_list = cJSON_PrintUnformatted(pArr);
	if(topic_list == NULL)
	{
		cJSON_Delete(pArr);
		return NULL;
	}
	cJSON_Delete(pArr);

	tmp = &(dev->mqtt_cfg.mqtt);
	tmp->sub_topics = topic_list;
	tmp->msg_cb = mqtt_receiver_cb;
	tmp->userdata = (void *)dev;

	do 
	{
		ret = MqttServerConnect(tmp);
		sleep(3);
	}while(ret != 0);

	SAFE_FREE(topic_list);

	return NULL;
}

void DeviceMqttConnect(void *handle)
{
	pthread_attr_t attr; 
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	pthread_attr_init( &attr ); 
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	if (!pthread_create(&dev->mqtt_cfg.mqtt_tid, 
						&attr, 
						device_mqtt_connect, 
						(void *)dev))
    {
        LOG_PRINT(WX_LOG_INFO, "Create %s mqtt connect thread OK!", 
							dev->device_id);
    }
    else
    {
        LOG_PRINT(WX_LOG_ERROR, "Create %s mqtt connect thread fail!", 
							 dev->device_id);
    }
	pthread_attr_destroy(&attr);
	sleep(1);
}

void DeviceMqttDisConnect(void *handle)
{
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return;
	}

	dev = (DEV_DRIVER *)handle;

	MqttServerDisConnect(&(dev->mqtt_cfg.mqtt));
	if(dev->mqtt_cfg.mqtt_tid != 0)
	{
		pthread_cancel(dev->mqtt_cfg.mqtt_tid);
		sleep(1);
	}
}

#if 0

int main(int argc, char **argv)
{
	MQTT_TOPIC_INFO topic_info;

	memset(&topic_info, 0, sizeof(topic_info));
	mqtt_receiver_topic_parse(argv[1], &topic_info);
	printf("topic_info->topic_type:%d\r\n", topic_info.topic_type);
	printf("topic_info->rpc_method:%d\r\n", topic_info.rpc_method);
	printf("topic_info->session_id:%llu\r\n", topic_info.session_id);

	return 0;
}
#endif


