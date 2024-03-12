#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>

#include "basic_types.h"
#include "pre_api.h"
#include "pre_json.h"
#include "pre_comm.h"
#include "cJSON.h"
#include "algorithm_api.h"
#include "hexutils.h"

#include "mosquitto_api.h"

#define PROXY_SEND_TOPIC_TO_SEND		"pre/proxy/request/%s"
#define PROXY_SEND_TOPIC_TO_RECV		"pre/proxy/respone/%s"
#define PROXY_LISTEN_TOPIC_FROM_RECV	"pre/receiver/request/#"
#define PROXY_LISTEN_TOPIC_FROM_SEND	"pre/sender/respone/#"

PRE_TOPIC_PARAM *proxy_para = NULL;

static int pre_proxy_process_send(	void *handle,
									void *data, 
									UINT32 data_len, 
									PRE_TOPIC_INFO *topic_info)
{
	PRE_SENDER_KEY		sender_key;	
	PRE_RECEIVER_KEY	receiver_key;	
	PRE_REENCRYPT_KEY 	re_key;
	PRE_CIPHER_TEXT 	level1_cipher;
	PRE_CIPHER_TEXT 	reencrypt_cipher;
	PRE_RECEIVER_KEY	*proxy_key = NULL;
	data_package package;
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	UINT32 ret = 0;
	UINT8 topic[256];
	UINT8 session[16];

	proxy_key = (PRE_RECEIVER_KEY *)proxy_para->key;

	tmp = PreRoleGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	
	if(PRE_ROLE_TYPE_INT(tmp) != PRE_ROLE_SENDER)
	{
		SAFE_FREE(tmp);
		return 0;
	}
	SAFE_FREE(tmp);

	memset(&receiver_key, 0, sizeof(receiver_key));
	tmp = PreRecvPublicKeyGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(receiver_key.public_key2, "%s", tmp);
	SAFE_FREE(tmp);

	memset(&sender_key, 0, sizeof(sender_key));
	tmp = PreSendPublicKeyGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(sender_key.public_key1, "%s", tmp);
	SAFE_FREE(tmp);

	memset(&level1_cipher, 0, sizeof(level1_cipher));
	tmp = PreCipherText1Get((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(level1_cipher.cipher_text1, "%s", tmp);
	SAFE_FREE(tmp);
	
	tmp = PreCipherText2Get((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(level1_cipher.cipher_text2, "%s", tmp);
	SAFE_FREE(tmp);

	memset(&re_key, 0, sizeof(re_key));
	tmp = PreReencryptKeyGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(re_key.reencrypt_key, "%s", tmp);
	SAFE_FREE(tmp);

	//9.Proxy利用重加密密钥与数据提供者的公钥1一起重加密第一重数据密文，生成第二重数据密文
	printf("9.Proxy利用重加密密钥与数据提供者的公钥1一起重加密第一重数据密文，生成第二重数据密文\r\n");
	memset(&reencrypt_cipher, 0, sizeof(reencrypt_cipher));
	PreReEncryptCipherGenerate(&reencrypt_cipher, &re_key, &level1_cipher, &sender_key);
	printf("reencrypt cipher1= %s\r\n", reencrypt_cipher.cipher_text1);
	printf("reencrypt cipher2= %s\r\n\r\n", reencrypt_cipher.cipher_text2);

	memset(&package, 0, sizeof(package));
	PackageInit(&package);

	PreRecvPublicKeyAdd(&package, receiver_key.public_key2);
	PreSendPublicKeyAdd(&package, sender_key.public_key1);
	PreProxyPublicKeyAdd(&package, proxy_key->public_key2);
	PreRoleAdd(&package, PRE_ROLE_PROXY_STR);
	PreCipherText1Add(&package, reencrypt_cipher.cipher_text1);
	PreCipherText2Add(&package, reencrypt_cipher.cipher_text2);

	memset(session, 0, sizeof(session));
	PreSessionidCreate(session);
	memset(topic, 0, sizeof(topic));
	sprintf(topic, PROXY_SEND_TOPIC_TO_RECV, session);
	//10.Proxy将生成的第二重数据密文，打包发送给数据接收者
	printf("10.Proxy将生成的第二重数据密文，打包发送给数据接收者，报文如下，报文内容包括：\r\n");
	printf("（1）数据接收者公钥2\r\n");
	printf("（2）数据发送者公钥1\r\n");
	printf("（3）Proxy公钥2\r\n");
	printf("（4）第二重数据密文数据TEXT1\r\n");
	printf("（5）第二重数据密文数据TEXT2\r\n");
	printf("（6）Proxy角色\r\n");
	PrePackageFormattPrint(package.data);
	ret = MqttPublish(topic, package.data, package.data_len, proxy_para->mqtt.mosq);
	if(ret)
	{
		return ret;
	}

	return 0;
}

static int pre_proxy_process_recv(	void *handle,
									void *data, 
									UINT32 data_len, 
									PRE_TOPIC_INFO *topic_info)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	data_package package;
	PRE_RECEIVER_KEY	*proxy_key = NULL;	
	UINT32 ret = 0;
	UINT8 topic[256];
	UINT8 session[16];

	proxy_key = (PRE_RECEIVER_KEY *)proxy_para->key;

	memset(&package, 0, sizeof(package));
	PackageInit(&package);

	DEBUG_PRINT_LINE();
	tmp = PreRoleGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}

	DEBUG_PRINT_LINE();
	if(PRE_ROLE_TYPE_INT(tmp) != PRE_ROLE_RECEIVER)
	{
		SAFE_FREE(tmp);
		return 0;
	}
	SAFE_FREE(tmp);

	DEBUG_PRINT_LINE();
	tmp = PreRecvPublicKeyGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	PreRecvPublicKeyAdd(&package, tmp);
	SAFE_FREE(tmp);

	#if 1
	tmp = PreSendPublicKeyGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	PreSendPublicKeyAdd(&package, tmp);
	SAFE_FREE(tmp);
	#endif

	DEBUG_PRINT_LINE();
	PreRoleAdd(&package, PRE_ROLE_PROXY_STR);
	PreProxyPublicKeyAdd(&package, proxy_key->public_key2);

	memset(session, 0, sizeof(session));
	PreSessionidCreate(session);
	memset(topic, 0, sizeof(topic));
	sprintf(topic, PROXY_SEND_TOPIC_TO_SEND, session);
	DEBUG_PRINT_LINE();
	printf("3.Proxy将数据接收者的请求转发给数据提供者，报文如下，报文内容包括：\r\n");
	printf("（1）数据接收者公钥2\r\n");
	printf("（2）数据发送者公钥2\r\n");
	printf("（3）Proxy公钥2\r\n");
	printf("（4）Proxy角色\r\n");
	PrePackageFormattPrint(package.data);
	ret = MqttPublish(topic, package.data, package.data_len, proxy_para->mqtt.mosq);
	if(ret)
	{
		return ret;
	}
	return 0;
	DEBUG_PRINT_LINE();
}


static INT32 pre_proxy_cb(void *data, UINT32 data_len, char *topic, void *para)
{
	PRE_TOPIC_INFO topic_info;
	PRE_TOPIC_PARAM *topic_para = NULL;
	UINT32 ret = 0;

	if(para == NULL)
	{
		return 1;
	}

	topic_para = (PRE_TOPIC_PARAM *)para;

	memset(&topic_info, 0, sizeof(topic_info));
	DEBUG_PRINT_LINE();
	ret = PreTopicParse(topic, &topic_info);
	DEBUG_PRINT_LINE();
	if(ret)
	{
		DEBUG_PRINT_LINE();
		return 1;
	}

	switch(topic_info.role)
	{
		case PRE_ROLE_SENDER:
			//9.Proxy收到数据提供者响应报文，报文如下：
			printf("9.Proxy收到数据提供者响应报文，报文如下：\r\n");
			PrePackageFormattPrint((UINT8 *)data);
			DEBUG_PRINT_LINE();
			pre_proxy_process_send(para, data, data_len, &topic_info);
			DEBUG_PRINT_LINE();
			break;
		case PRE_ROLE_RECEIVER:
			printf("2.Proxy收到数据接收者发起的申请，报文如下：\r\n");
			PrePackageFormattPrint((UINT8 *)data);
			DEBUG_PRINT_LINE();
			pre_proxy_process_recv(para, data, data_len, &topic_info);
			DEBUG_PRINT_LINE();
			break;
	}


	return 0;
}


static void *pre_proxy_listen()
{
	UINT32 ret = 0;

	do 
	{
		ret = MqttServerConnect(&proxy_para->mqtt);
		sleep(3);
	}while(ret != 0);


	return NULL;
}


void PreProxyThreadStart()
{
	pthread_attr_t attr; 
	pthread_t 	tid;


	pthread_attr_init( &attr ); 
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	if (!pthread_create(&tid, &attr, pre_proxy_listen, NULL))
    {
        //LOG_PRINT(  Prome_LOG_INFO, "Create proxy listen thread OK!");
    }
    else
    {
        //LOG_PRINT(  Prome_LOG_ERROR, "Create proxy listen thread fail!");
    }
	pthread_attr_destroy(&attr);
	sleep(1);
}


void *PreProxyInit(void *para)
{
	UINT8 *topic_list = NULL;
	cJSON *pArr = NULL;
	PRE_USER_KEY	*proxy_key = NULL;
	MQTT_CONNECT_PARAM 	*mqtt = NULL;
	UINT32 ret = 0;

	mqtt = (MQTT_CONNECT_PARAM *)para;

	pArr = cJSON_CreateArray();
	if(pArr == NULL)
	{
		return NULL;
	}

	cJSON_AddItemToArray(pArr, cJSON_CreateString(PROXY_LISTEN_TOPIC_FROM_RECV));
	cJSON_AddItemToArray(pArr, cJSON_CreateString(PROXY_LISTEN_TOPIC_FROM_SEND));
	
	topic_list = cJSON_PrintUnformatted(pArr);
	if(topic_list == NULL)
	{
		cJSON_Delete(pArr);
		return NULL;
	}
	printf("topic_list:%s\r\n", topic_list);
	cJSON_Delete(pArr);

	proxy_para = malloc(sizeof(PRE_TOPIC_PARAM));
	if(proxy_para == NULL)
	{
		return NULL;
	}

	proxy_key = malloc(sizeof(PRE_USER_KEY));
	if(proxy_key == NULL)
	{
		SAFE_FREE(proxy_para);
		return NULL;
	}

	//3.生成Proxy的公私钥
#if 0
	printf("生成Proxy的公私钥\r\n");
	memset(proxy_key, 0, sizeof(PRE_USER_KEY));
	PreUserKeyGenerate(proxy_key);
#else
	memset(proxy_key, 0, sizeof(PRE_USER_KEY));
	if(access("./proxy_key", F_OK)==0)
	{
		printf("获取Proxy的公私钥\r\n");
		PreUserKey_Get("./proxy_key", proxy_key);
	}
	else
	{
		printf("生成Proxy的公私钥\r\n");
		PreUserKeyGenerate(proxy_key);
		PreUserKey_Set("./proxy_key", proxy_key);
	}	
#endif	
	printf("proxy secret key1:%s\r\n", proxy_key->secret_key1);
	printf("proxy secret key2:%s\r\n", proxy_key->secret_key2);
	printf("proxy public key1:%s\r\n", proxy_key->public_key1);
	printf("proxy public key2:%s\r\n\r\n", proxy_key->public_key2);
	
	memset(proxy_para, 0, sizeof(PRE_TOPIC_PARAM));

	sprintf(proxy_para->mqtt.host, "%s", mqtt->host);
	sprintf(proxy_para->mqtt.username, "%s", mqtt->username);
	sprintf(proxy_para->mqtt.password, "%s", mqtt->password);
	proxy_para->mqtt.port = mqtt->port;
	proxy_para->mqtt.sub_topics = topic_list;
	proxy_para->mqtt.msg_cb = pre_proxy_cb;
	proxy_para->mqtt.userdata = (void *)proxy_para;
	proxy_para->key = (void *)proxy_key;
	proxy_para->role = PRE_ROLE_PROXY;

	do 
	{
		ret = MqttServerConnect(&proxy_para->mqtt);
		sleep(3);
	}while(ret != 0);
	SAFE_FREE(topic_list);

	//PreProxyThreadStart();
	return NULL;
}

#if 0
int main(int argc, char **argv)
{
	
	PreProxyInit();
	PreProxySendReq();
	
	return 0;
}

#endif
