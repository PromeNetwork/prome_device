#include <string.h>
#include <stdlib.h>

#include "basic_types.h"
#include "pre_comm.h"
#include "hexutils.h"



UINT8 *PRE_ROLE_TYPE_STR(PRE_ROLE_TYPE_E type)
{
	UINT8 *tmp = NULL;
	
	switch(type)
	{
		case PRE_ROLE_SENDER:
			tmp = (UINT8 *)PRE_ROLE_SENDER_STR;
			break;
		case PRE_ROLE_PROXY:
			tmp = (UINT8 *)PRE_ROLE_PROXY_STR;
			break;
		case PRE_ROLE_RECEIVER:
			tmp = (UINT8 *)PRE_ROLE_RECEIVER_STR;
			break;	
	}
	return tmp;
}

PRE_ROLE_TYPE_E PRE_ROLE_TYPE_INT(UINT8 *type)
{
	if(strstr(type, PRE_ROLE_SENDER_STR))
	{
		return PRE_ROLE_SENDER;
	}
	else if(strstr(type, PRE_ROLE_PROXY_STR))
	{
		return PRE_ROLE_PROXY;
	}
	else if(strstr(type, PRE_ROLE_RECEIVER_STR))
	{
		return PRE_ROLE_RECEIVER;
	}
	else
	{
		return PRE_ROLE_DEFAULT;
	}
}

UINT8 *PRE_METHOD_TYPE_STR(PRE_METHOD_TYPE_E type)
{
	UINT8 *tmp = NULL;
	
	switch(type)
	{
		case PRE_METHOD_REQUEST :
			tmp = (UINT8 *)PRE_METHOD_REQUEST_STR;
			break;
		case PRE_METHOD_RESPONSE:
			tmp = (UINT8 *)PRE_METHOD_RESPONSE_STR;
			break;
	}
	return tmp;
}

PRE_METHOD_TYPE_E PRE_METHOD_TYPE_INT(UINT8 *type)
{
	if(strstr(type, PRE_METHOD_REQUEST_STR))
	{
		return PRE_METHOD_REQUEST;
	}
	else if(strstr(type, PRE_METHOD_RESPONSE_STR))
	{
		return PRE_METHOD_RESPONSE;
	}
	else
	{
		return PRE_METHOD_DEFAULT;
	}
}

static UINT64 pre_session_get(UINT8 *topic)
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

static PRE_ROLE_TYPE_E pre_role_type_get(UINT8 *topic) 
{
	UINT8 tmp[PRE_ROLE_TYPE_MAX_LEN];
	PRE_ROLE_TYPE_E topic_type = PRE_ROLE_DEFAULT;
	
	if(topic == NULL)
	{
		return PRE_ROLE_DEFAULT;
	}

	memset(tmp, 0, sizeof(tmp));
  
  	sscanf(topic,"%*[^/]/%[^/]",tmp);
	printf("tmp:%s\r\n", tmp);
	if(strlen(tmp))
	{
		topic_type = PRE_ROLE_TYPE_INT(tmp);
	}

	return topic_type;	
}

static PRE_METHOD_TYPE_E pre_method_type_get(UINT8 *topic) 
{
	UINT8 tmp[PRE_METHOD_MAX_LEN];
	PRE_METHOD_TYPE_E method = PRE_METHOD_DEFAULT;

	if(topic == NULL)
	{
		return PRE_METHOD_DEFAULT;
	}

	memset(tmp, 0, sizeof(tmp));

	//devices/rpc/request/aaa/upgradeBoxGateway/123
	//pre/receiver/request/
  	sscanf(topic,"%*[^/]/%*[^/]/%[^/]",tmp);
	printf("tmp:%s\r\n", tmp);
	if(strlen(tmp))
	{
		method = PRE_METHOD_TYPE_INT(tmp);
	}

	return method;
	
}

UINT32 PreTopicParse(UINT8 *topic, PRE_TOPIC_INFO *topic_info)
{
	if(topic == NULL || topic_info == NULL)
	{
		return 1;
	}

	DEBUG_PRINT_LINE();
	topic_info->role = pre_role_type_get(topic);
	topic_info->method = pre_method_type_get(topic);
	topic_info->session_id = pre_session_get(topic);
	DEBUG_PRINT_LINE();
	
	return 0;
}

void PreSessionidCreate(char *sessionid)
{
	TIMESTAMP stamp = 0;
	stamp = GetTimestampInt();
	TIMESTAMP_TO_STR(stamp, sessionid);
	//sprintf(sessionid, "%6d", rand());
}


