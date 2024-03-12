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

#define SENDER_SEND_TOPIC	"pre/sender/respone/%s"
#define SENDER_LISTEN_TOPIC	"pre/proxy/request/#"

PRE_TOPIC_PARAM *sender_para = NULL;

//从指定文件读取shared_key
static UINT32 sharedKey_Get(char *file ,PRE_SYMMETRIC_KEY *key)
{
	int ret=0;
	
	ret = ConfigGet("key", "m", file,  (char *)key->m);
	if(ret || strlen(key->m) == 0)
	{
		printf("read m from %s error \r\n", file);
		return -1;
	}
	ret = ConfigGet("key", "mGT", file,  (char *)key->mGT);
	if(ret || strlen(key->mGT) == 0)
	{
		printf("read mGT from %s error \r\n", file);
		return -1;
	}
	return 0;
}

//将user_key存到指定文件
static UINT32 sharedKey_Set(char *file ,PRE_SYMMETRIC_KEY *key)
{
	int ret=0;

	ret = ConfigPut("key", "m",  (char *)key->m, file);
	if(ret)
	{
		printf("set m to %s error \r\n", file);
		return -1;
	}
	ret = ConfigPut("key", "mGT",  (char *)key->mGT, file);
	if(ret)
	{
		printf("set mGT to %s error \r\n", file);
		return -1;
	}
	return 0;
}

static UINT32 PreSymmetricKey_Get(UINT8 *seed, char *file ,PRE_SYMMETRIC_KEY *key)
{
	UINT32 ret = 0;
	
#if 0
	ret = PreSymmetricKeyGenerate(seed, &data_key);
#else
	if(access(file, F_OK)==0)
	{
		printf("获取对称密钥\r\n");
		ret = sharedKey_Get(file, key);
	}
	else
	{
		printf("生成对称密钥\r\n");
		ret = PreSymmetricKeyGenerate(seed, key);
		if(ret)
		{
			printf("PreSymmetricKeyGenerate error\r\n");
			return -1;
		}
		ret = sharedKey_Set(file, key);
	}	
#endif
	if(ret)
	{
		printf("PreSymmetricKey_Get error\r\n");
		return -1;
	}

	return 0;
}

static UINT32 Symmmeric_Enc(PRE_SYMMETRIC_KEY *data_key)
{
	//add1:使用对称密钥对数据进行加密
	UINT8 *user_data = "pre_demo:test_pre_function";
	UINT8 user_key[32] = {0};
	char *enc_out = NULL;
	int err_code = 0;
	int ret=0;

	printf("使用对称密钥对数据进行加密\r\n");
	printf("user_data :%s\r\n", user_data);
	StrToHex(user_key, data_key->m, sizeof(user_key));
	enc_out =DataEncryption(user_data,strlen(user_data), ENC_TYPE_AES_CBC_PKCS5, user_key, sizeof(user_key), &err_code);
	if(enc_out == NULL)
	{
		printf("Encryption fail, err_code:%d\r\n", err_code);
		return err_code;
	}
	printf("Encryption:%s\r\n\r\n", enc_out);	
	
	//将密文数据存储到文件
	ret = ConfigPut("cipher", "data",  (char *)enc_out, "./cipher");
	if(ret)
	{
		printf("set cipher_data to ./cipher error \r\n");
		return -1;
	}
	SAFE_FREE(enc_out);
	return 0;
}

static int pre_sender_process(void *handle,
								void *data, 
								UINT32 data_len, 
								PRE_TOPIC_INFO *topic_info)
{
	PRE_TOPIC_PARAM		*para = NULL;
	PRE_SENDER_KEY		*sender_key = NULL;	
	PRE_SYMMETRIC_KEY 	data_key;
	PRE_REENCRYPT_KEY 	re_key;
	PRE_RECEIVER_KEY	receiver_key;	
	PRE_CIPHER_TEXT 	level1_cipher;
	PRE_USER_KEY		proxy_key;
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	data_package package;
	UINT32 ret = 0;
	UINT8 topic[256];
	UINT8 session[16];

	para = (PRE_TOPIC_PARAM	*)handle;
	sender_key = (PRE_SENDER_KEY *)para->key;

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

	memset(&receiver_key, 0, sizeof(receiver_key));
	tmp = PreRecvPublicKeyGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(receiver_key.public_key2, "%s", tmp);
	SAFE_FREE(tmp);

	memset(&proxy_key, 0, sizeof(proxy_key));
	tmp = PreProxyPublicKeyGet((UINT8 *)data, &len);
	if(tmp == NULL)
	{
		return 0;
	}
	sprintf(proxy_key.public_key2, "%s", tmp);
	SAFE_FREE(tmp);

	

	//5.数据提供者生成用于实际数据加密使用的对称密钥m，为GT曲线上的一个点
	printf("5.数据提供者生成/读取用于实际数据加密使用的对称密钥m，为GT曲线上的一个点\r\n");
	memset(&data_key, 0, sizeof(data_key));
	PreSymmetricKey_Get(NULL,"./sym_key", &data_key);
	printf("data_key m:%s\r\n", data_key.m);
	printf("data_key mGT:%s\r\n\r\n", data_key.mGT);

	//6.数据提供者利用接收者(Bob)的公钥2与自己的私钥1，一起生成重加密密钥
	printf("6.数据提供者利用接收者(Bob)的公钥2与自己的私钥1，一起生成重加密密钥\r\n");
	memset(&re_key, 0, sizeof(re_key));
	PreReEncryptKeyGenerate(&re_key, &receiver_key, sender_key);
	printf("reencrypt key:%s\r\n\r\n", re_key.reencrypt_key);

	//7.数据提供者利用自己的公钥1加密数据，生成第一重数据密文
	printf("7.数据提供者利用自己的公钥1加密数据，生成第一重数据密文\r\n");
	memset(&level1_cipher, 0, sizeof(level1_cipher));
	PreLevel1CipherGenerate(&level1_cipher, sender_key, &data_key);
	printf("level1 cipher1= %s\r\n", level1_cipher.cipher_text1);
	printf("level1 cipher2= %s\r\n\r\n", level1_cipher.cipher_text2);

	memset(&package, 0, sizeof(package));
	PackageInit(&package);

	PreRecvPublicKeyAdd(&package, receiver_key.public_key2);
	PreSendPublicKeyAdd(&package, sender_key->public_key1);
	PreProxyPublicKeyAdd(&package, proxy_key.public_key2);
	PreRoleAdd(&package, PRE_ROLE_SENDER_STR);
	PreCipherText1Add(&package, level1_cipher.cipher_text1);
	PreCipherText2Add(&package, level1_cipher.cipher_text2);
	PreReencryptKeyAdd(&package, re_key.reencrypt_key);

	memset(session, 0, sizeof(session));
	PreSessionidCreate(session);
	memset(topic, 0, sizeof(topic));
	sprintf(topic, SENDER_SEND_TOPIC, session);
	//8.数据提供者将自己的公钥1、第一重数据密文数据、重加密密钥等打包发送给proxy，报文如下：
	printf("8.数据提供者将自己的公钥1、第一重数据密文数据、重加密密钥等打包发送给proxy，报文如下，报文内容包括：\r\n");
	printf("（1）数据接收者公钥2\r\n");
	printf("（2）数据发送者公钥1\r\n");
	printf("（3）Proxy公钥2\r\n");
	printf("（4）第一重数据密文数据TEXT1\r\n");
	printf("（5）第一重数据密文数据TEXT2\r\n");
	printf("（6）重加密密钥\r\n");
	printf("（7）数据发送者角色\r\n");
	PrePackageFormattPrint(package.data);
	ret = MqttPublish(topic, package.data, package.data_len, sender_para->mqtt.mosq);
	if(ret)
	{
		return ret;
	}
	
	return 0;
}


static INT32 pre_sender_cb(void *data, UINT32 data_len, char *topic, void *para)
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

	printf("4.数据提供者收到proxy转发的请求，报文如下：\r\n");
	PrePackageFormattPrint((UINT8 *)data);
	pre_sender_process(para, data, data_len, &topic_info);

	return 0;
}


static void *pre_sender_listen(void *ptr)
{
	UINT32 ret = 0;

	do 
	{
		ret = MqttServerConnect(&sender_para->mqtt);
		sleep(3);
	}while(ret != 0);


	return NULL;
}


void PreSenderThreadStart()
{
	pthread_attr_t attr; 
	pthread_t 	tid;

	pthread_attr_init( &attr ); 
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	if (!pthread_create(&tid, &attr, pre_sender_listen, NULL))
    {
        //LOG_PRINT(  Prome_LOG_INFO, "Create sender listen thread OK!");
    }
    else
    {
        //LOG_PRINT(  Prome_LOG_ERROR, "Create sender listen thread fail!");
    }
	pthread_attr_destroy(&attr);
	sleep(1);
}

void *Key_Update(void *para)
{
	PRE_SYMMETRIC_KEY 	data_key;
	int ret=0; 
	do{
		printf("\r\n=====数据提供者生成/更新用于实际数据加密使用的对称密钥m====\r\n");
		memset(&data_key, 0, sizeof(data_key));
		ret = PreSymmetricKeyGenerate(NULL, &data_key);
		ret |= sharedKey_Set("./sym_key", &data_key);
		printf("data_key m:%s\r\n", data_key.m);
		printf("data_key mGT:%s\r\n\r\n", data_key.mGT);

		//密钥更新，对测试数据重新加密
		//ret |= Symmmeric_Enc(&data_key);
	
		sleep(200);
	}while(!ret);
}

void *PreSenderInit(void *para)
{
	UINT8 *topic_list = NULL;
	cJSON *pArr = NULL;
	PRE_SENDER_KEY	*sender_key = NULL;
	MQTT_CONNECT_PARAM 	*mqtt = NULL;
	UINT32 ret = 0;

	mqtt = (MQTT_CONNECT_PARAM *)para;

	pArr = cJSON_CreateArray();
	if(pArr == NULL)
	{
		return NULL;
	}

	cJSON_AddItemToArray(pArr, cJSON_CreateString(SENDER_LISTEN_TOPIC));
	
	topic_list = cJSON_PrintUnformatted(pArr);
	if(topic_list == NULL)
	{
		cJSON_Delete(pArr);
		return NULL;
	}
	printf("topic_list:%s\r\n", topic_list);
	cJSON_Delete(pArr);

	sender_para = malloc(sizeof(PRE_TOPIC_PARAM));
	if(sender_para == NULL)
	{
		return NULL;
	}

	sender_key = malloc(sizeof(PRE_SENDER_KEY));
	if(sender_key == NULL)
	{
		SAFE_FREE(sender_para);
		return NULL;
	}
	
	//生成发送者Alice的公私钥
#if 0
	printf("生成发送者Alice的公私钥\r\n");
	memset(sender_key, 0, sizeof(sender_key));
	PreUserKeyGenerate(sender_key);
#else
	memset(sender_key, 0, sizeof(PRE_USER_KEY));
	if(access("./sender_key", F_OK)==0)
	{
		printf("获取Alice的公私钥\r\n");
		PreUserKey_Get("./sender_key", sender_key);
	}
	else
	{
		printf("生成Alice的公私钥\r\n");
		PreUserKeyGenerate(sender_key);
		PreUserKey_Set("./sender_key", sender_key);
	}	
#endif	

	printf("sender secret key1:%s\r\n", sender_key->secret_key1);
	printf("sender secret key2:%s\r\n", sender_key->secret_key2);
	printf("sender public key1:%s\r\n", sender_key->public_key1);
	printf("sender public key2:%s\r\n\r\n", sender_key->public_key2);
	
	memset(sender_para, 0, sizeof(PRE_TOPIC_PARAM));

	sprintf(sender_para->mqtt.host, "%s", mqtt->host);
	sprintf(sender_para->mqtt.username, "%s", mqtt->username);
	sprintf(sender_para->mqtt.password, "%s", mqtt->password);
	sender_para->mqtt.port = mqtt->port;
	sender_para->mqtt.sub_topics = topic_list;
	sender_para->mqtt.msg_cb = pre_sender_cb;
	sender_para->mqtt.userdata = (void *)sender_para;
	sender_para->key = (void *)sender_key;
	sender_para->role = PRE_ROLE_SENDER;

	do 
	{
		ret = MqttServerConnect(&sender_para->mqtt);
		sleep(3);
	}while(ret != 0);
	SAFE_FREE(topic_list);

	//PreSenderThreadStart();
	return NULL;
}

#if 0
int main(int argc, char **argv)
{
	
	PreSenderInit();
	PreSenderSendReq();
	
	return 0;
}

#endif
