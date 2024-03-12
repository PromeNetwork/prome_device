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
#include "config.h"

#include "mosquitto_api.h"

#define RECEIVER_SEND_TOPIC		"pre/receiver/request/%s"
#define RECEIVER_LISTEN_TOPIC	"pre/proxy/respone/#"

PRE_TOPIC_PARAM *receiver_para = NULL;

static UINT32 Symmmeric_Dec(PRE_SYMMETRIC_KEY *data_key)
{

	UINT8 user_key[32] = {0};
	UINT8 *dec_out = NULL;
	int err_code = 0;
	UINT32 len = 0;
	UINT8 cipher[128] ={0};
	int ret=0;
	
	ret = ConfigGet("cipher", "data", "./cipher",  (char *)cipher);
	if(ret || strlen(cipher) == 0)
	{
		printf("read cipher_data from ./cipher error \r\n");
		return -1;
	}
	//13.用共享密钥解密密文数据，得要原始数据
	printf("Encryption of user_data:%s\r\n", cipher);
	memset(user_key, 0 , sizeof(user_key));
	StrToHex(user_key, data_key->m, sizeof(user_key));
	dec_out =DataDecryption(cipher,strlen(cipher), ENC_TYPE_AES_CBC_PKCS5, user_key, sizeof(user_key) ,&err_code, &len);
	if(dec_out == NULL)
	{
		printf("Decryption fail, err_code:%d\r\n", err_code);
		return err_code;
	}
	printf("Decryption of user_data:%s\r\n", dec_out);
	SAFE_FREE(dec_out);
		
	return 0;
}


static int pre_receiver_process(void *handle,
								void *data, 
								UINT32 data_len, 
								PRE_TOPIC_INFO *topic_info)
{
	PRE_CIPHER_TEXT 	reencrypt_cipher;
	PRE_SYMMETRIC_KEY 	msg;
	PRE_TOPIC_PARAM		*para = NULL;
	PRE_RECEIVER_KEY	*receiver_key = NULL;	
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	para = (PRE_TOPIC_PARAM	*)handle;
	receiver_key = (PRE_RECEIVER_KEY *)para->key;

	tmp = PreRoleGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	
	if(PRE_ROLE_TYPE_INT(tmp) != PRE_ROLE_PROXY)
	{
		SAFE_FREE(tmp);
		return 0;
	}
	SAFE_FREE(tmp);

	memset(&reencrypt_cipher, 0, sizeof(reencrypt_cipher));
	tmp = PreCipherText1Get((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(reencrypt_cipher.cipher_text1, "%s", tmp);
	SAFE_FREE(tmp);

	
	tmp = PreCipherText2Get((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(reencrypt_cipher.cipher_text2, "%s", tmp);
	SAFE_FREE(tmp);

	//12.数据接收者收到proxy的响应报文
	printf("12.数据接收者利用自己的私钥解密第二重数据密文，得到共享密钥\r\n");
	memset(&msg, 0, sizeof(msg));
	PreDecryption(&msg, &reencrypt_cipher, receiver_key);
	printf("msg m:%s\r\n", msg.m);
	printf("msg mGT:%s\r\n\r\n", msg.mGT);

	//Symmmeric_Dec(&msg);
	
	return 0;
}


static INT32 pre_receiver_cb(void *data, UINT32 data_len, char *topic, void *para)
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

	//11.数据接收者收到proxy的响应报文
	printf("11.数据接收者收到proxy的响应报文，报文如下：\r\n");
	PrePackageFormattPrint((UINT8 *)data);
	pre_receiver_process(para, data, data_len, &topic_info);

	return 0;
}


static void *pre_receiver_listen(void *ptr)
{
	UINT32 ret = 0;

	do 
	{
		ret = MqttServerConnect(&receiver_para->mqtt);
		sleep(3);
	}while(ret != 0);


	return NULL;
}


void PreReceiverThreadStart()
{
	pthread_attr_t attr; 
	pthread_t 	tid;

	pthread_attr_init( &attr ); 
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	if (!pthread_create(&tid, &attr, pre_receiver_listen, NULL))
    {
        //LOG_PRINT(  Prome_LOG_INFO, "Create receiver listen thread OK!");
    }
    else
    {
        //LOG_PRINT(  Prome_LOG_ERROR, "Create receiver listen thread fail!");
    }
	pthread_attr_destroy(&attr);
	sleep(1);
}

void PreReceiverSendReq(UINT8 *sender_pbkey)
{
	data_package package;
	UINT32 ret = 0;
	UINT8 topic[256];
	UINT8 session[16];
	UINT8 *tmpkey="0000000000000000";
	PRE_RECEIVER_KEY	*receiver_key = NULL;	

	receiver_key = (PRE_RECEIVER_KEY *)receiver_para->key;
	memset(&package, 0, sizeof(package));
	PackageInit(&package);

	PreRecvPublicKeyAdd(&package, receiver_key->public_key2);
	if(sender_pbkey == NULL)
		PreSendPublicKeyAdd(&package, tmpkey);//senderkey 暂时为0
	else
		PreSendPublicKeyAdd(&package, sender_pbkey);
	PreProxyPublicKeyAdd(&package, tmpkey);//proxykey 暂时为0
	PreRoleAdd(&package, PRE_ROLE_RECEIVER_STR);

	memset(session, 0, sizeof(session));
	PreSessionidCreate(session);
	memset(topic, 0, sizeof(topic));
	sprintf(topic, RECEIVER_SEND_TOPIC, session);
	printf("-3-mosq:%p\r\n", receiver_para->mqtt.mosq);
	printf("1.数据接收者向Proxy发起申请，报文如下，报文内容包括：\r\n");
	printf("（1）数据接收者公钥2\r\n");
	printf("（2）数据发送者公钥2\r\n");
	printf("（3）数据代理公钥2\r\n");
	printf("（4）数据接收者角色\r\n");
	PrePackageFormattPrint(package.data);
	ret = MqttPublish(topic, package.data, package.data_len, receiver_para->mqtt.mosq);
	if(ret)
	{
		return ;
	}
	return ;
}

void *PreReceiverInit(void *para)
{
	UINT8 *topic_list = NULL;
	cJSON *pArr = NULL;
	PRE_RECEIVER_KEY	*receiver_key = NULL;
	MQTT_CONNECT_PARAM 	*mqtt = NULL;
	UINT32 ret = 0;

	mqtt = (MQTT_CONNECT_PARAM *)para;

	pArr = cJSON_CreateArray();
	if(pArr == NULL)
	{
		return NULL;
	}

	cJSON_AddItemToArray(pArr, cJSON_CreateString(RECEIVER_LISTEN_TOPIC));
	
	topic_list = cJSON_PrintUnformatted(pArr);
	if(topic_list == NULL)
	{
		cJSON_Delete(pArr);
		return NULL;
	}
	printf("topic_list:%s\r\n", topic_list);
	cJSON_Delete(pArr);

	receiver_para = malloc(sizeof(PRE_TOPIC_PARAM));
	if(receiver_para == NULL)
	{
		return NULL;
	}

	receiver_key = malloc(sizeof(PRE_RECEIVER_KEY));
	if(receiver_key == NULL)
	{
		SAFE_FREE(receiver_para);
		return NULL;
	}
	//生成接收者(Bob)的公私钥
#if 0
	printf("生成接收者(Bob)的公私钥\r\n");
	memset(receiver_key, 0, sizeof(PRE_RECEIVER_KEY));
	PreUserKeyGenerate(receiver_key);
#else
	memset(receiver_key, 0, sizeof(PRE_USER_KEY));
	if(access("./receiver_key", F_OK)==0)
	{
		printf("获取接收者(Bob)的公私钥\r\n");
		PreUserKey_Get("./receiver_key", receiver_key);
	}
	else
	{
		printf("生成接收者(Bob)的公私钥\r\n");
		PreUserKeyGenerate(receiver_key);
		PreUserKey_Set("./receiver_key", receiver_key);
	}	
#endif	

	printf("receiver secret key1:%s\r\n", receiver_key->secret_key1);
	printf("receiver secret key2:%s\r\n", receiver_key->secret_key2);
	printf("receiver public key1:%s\r\n", receiver_key->public_key1);
	printf("receiver public key2:%s\r\n\r\n", receiver_key->public_key2);
	
	memset(receiver_para, 0, sizeof(PRE_TOPIC_PARAM));

	sprintf(receiver_para->mqtt.host, "%s", mqtt->host);
	sprintf(receiver_para->mqtt.username, "%s", mqtt->username);
	sprintf(receiver_para->mqtt.password, "%s", mqtt->password);
	receiver_para->mqtt.port = mqtt->port;
	receiver_para->mqtt.sub_topics = topic_list;
	receiver_para->mqtt.msg_cb = pre_receiver_cb;
	receiver_para->mqtt.userdata = (void *)receiver_para;
	receiver_para->key = (void *)receiver_key;
	receiver_para->role = PRE_ROLE_RECEIVER;

	do 
	{
		ret = MqttServerConnect(&receiver_para->mqtt);
		sleep(3);
	}while(ret != 0);
	SAFE_FREE(topic_list);

	//PreReceiverThreadStart();
	return NULL;
}

#if 0
int main(int argc, char **argv)
{
	
	PreReceiverInit();
	PreReceiverSendReq();
	
	return 0;
}

#endif
