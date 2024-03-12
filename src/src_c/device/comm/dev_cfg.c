#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "basic_types.h"
#include "cJSON.h"
#include "hexutils.h"
#include "dev_json.h"
#include "dev_cfg.h"
#include "algorithm_api.h"
#include "dev_comm.h"
#include "log.h"
#include "upgrade.h"
#include "mosquitto_api.h"

UINT8 *CFG_TYPE_STR(CFG_TYPE_E type)
{
	UINT8 *tmp = NULL;

	switch(type)
	{
		case CFG_TYPE_MQTT:
			tmp = (UINT8 *)CFG_TYPE_MQTT_STR;
			break;
		case CFG_TYPE_BP_CT:
			tmp = (UINT8 *)CFG_TYPE_BP_CT_STR;
			break;
		case CFG_TYPE_BP_CFG:
			tmp = (UINT8 *)CFG_TYPE_BP_CFG_STR;
			break;
		case CFG_TYPE_UPGRADE:
			tmp = (UINT8 *)CFG_TYPE_UPGRADE_STR;
			break;
		case CFG_TYPE_CHAIN:
			tmp = (UINT8 *)CFG_TYPE_CHAIN_STR;
			break;
		case CFG_TYPE_RULE:
			tmp = (UINT8 *)CFG_TYPE_RULE_STR;
			break;
		case CFG_TYPE_ADD_DEV:
			tmp = (UINT8 *)CFG_TYPE_ADD_DEV_STR;
			break;
		case CFG_TYPE_FILE:
			tmp = (UINT8 *)CFG_TYPE_FILE_STR;
			break;
		default:
			tmp = (UINT8 *)"UNKNOWN";
			break;
	}
	return tmp;
}

CFG_TYPE_E CFG_TYPE_INT(UINT8 *type)
{
	if(strstr(type, CFG_TYPE_MQTT_STR))
	{
		return CFG_TYPE_MQTT;
	}
	else if(strstr(type, CFG_TYPE_BP_CT_STR))
	{
		return CFG_TYPE_BP_CT;
	}
	else if(strstr(type, CFG_TYPE_BP_CFG_STR))
	{
		return CFG_TYPE_BP_CFG;
	}
	else if(strstr(type, CFG_TYPE_UPGRADE_STR))
	{
		return CFG_TYPE_UPGRADE;
	}
	else if(strstr(type, CFG_TYPE_CHAIN_STR))
	{
		return CFG_TYPE_CHAIN;
	}
	else if(strstr(type, CFG_TYPE_RULE_STR))
	{
		return CFG_TYPE_RULE;
	}
	else if(strstr(type, CFG_TYPE_ADD_DEV_STR))
	{
		return CFG_TYPE_ADD_DEV;
	}
	else if(strstr(type, CFG_TYPE_FILE_STR))
	{
		return CFG_TYPE_FILE;
	}
	else
	{
		return CFG_TYPE_DEFALUT;
	}
}

UINT8 *BP_TYPE_STR(BP_TYPE_E type)
{
	UINT8 *tmp = NULL;
	
	switch(type)
	{
		case BP_LOCAL_LIB:
			tmp = (UINT8 *)BP_LOCAL_LIB_STR;
			break;
		case BP_LOCAL_SERVER:
			tmp = (UINT8 *)BP_LOCAL_SERVER_STR;
			break;
		case BP_REMOTE_SERVER:
			tmp = (UINT8 *)BP_REMOTE_SERVER_STR;
			break;
		default:
			tmp = (UINT8 *)"UNKNOWN";
			break;
	}
	return tmp;
}

BP_TYPE_E BP_TYPE_INT(UINT8 *type)
{
	if(strstr(type, BP_LOCAL_LIB_STR))
	{
		return BP_LOCAL_LIB;
	}
	else if(strstr(type, BP_LOCAL_SERVER_STR))
	{
		return BP_LOCAL_SERVER;
	}
	else if(strstr(type, BP_REMOTE_SERVER_STR))
	{
		return BP_REMOTE_SERVER;
	}
	else
	{
		return BP_NONE;
	}
}


static UINT32 device_mqtt_compare(MQTT_CONNECT_PARAM *mqtt1, 
								  MQTT_CONNECT_PARAM *mqtt2)
{
	if(mqtt1->port != mqtt2->port)
	{
		return 1;
	}

	if(mqtt1->qos != mqtt2->qos)
	{
		return 1;
	}

	if(strcmp(mqtt1->host, mqtt2->host))
	{
		return 1;
	}

	if(strcmp(mqtt1->username, mqtt2->username))
	{
		return 1;
	}

	if(strcmp(mqtt1->password, mqtt2->password))
	{
		return 1;
	}
	return 0;
}

static UINT32 device_mqtt_check(MQTT_CONNECT_PARAM *mqtt)
{
	UINT32 ret = 0;
	
	ret = MqttServerConnect(mqtt);
	if(ret)
	{
		MqttServerDisConnect(mqtt);
		return 1;
	}

	MqttServerDisConnect(mqtt);
	return 0;
	
}

INT32 DeviceMqttCfgParse(void *handle, UINT8 *package, UINT8 need_check)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	MQTT_CFG *mqtt_cfg = NULL;
	MQTT_CONNECT_PARAM mqtt;
	INT32 ret = 0;
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return -1;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&mqtt, 0, sizeof(mqtt));
	tmp = JsonHostGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(mqtt.host, tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		return -1;
	}

	tmp = JsonPortGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy((UINT8 *)&(mqtt.port), tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		return -1;
	}

	tmp = JsonUserGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(mqtt.username, tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		return -1;
	}

	tmp = JsonPwdGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(mqtt.password, tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		return -1;
	}

	mqtt_cfg = &(dev->mqtt_cfg);
	
	ret = device_mqtt_compare(&mqtt_cfg->mqtt, &mqtt);
	if(ret)
	{
		if(need_check)
		{
			ret = device_mqtt_check(&mqtt);
			if(ret)
			{
				LOG_PRINT(WX_LOG_ERROR, 
						  "Device %s the mqtt broker we receive cannot be connected.", 
						  dev->device_id);
				return -1;
			}
		}

		mqtt_cfg->mqtt.port = mqtt.port;
		mqtt_cfg->mqtt.qos = mqtt.qos;
		memcpy(mqtt_cfg->mqtt.host, mqtt.host, sizeof(mqtt.host));
		memcpy(mqtt_cfg->mqtt.username, mqtt.username, sizeof(mqtt.username));
		memcpy(mqtt_cfg->mqtt.password, mqtt.password, sizeof(mqtt.password));
		return 1;
	}

	return 0;
}




/*****************************************************************************
 * @Function	: cfg_parse_mqtt
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 解析MQTT相关配置
 * @param[in]  	: DEV_DRIVER *dev  设备句柄
               	  UINT8 *package   配置信息字串
 * @param[out]  : None
 * @return  	: 	-1:解析失败；
 *					 0:解析成功，但配置相同未改变
 *					 1:解析成功，配置不同
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
INT32 DeviceMqttCfgInit(void *handle, UINT8 *package)
{
	cJSON *root = NULL;
	cJSON *array = NULL;
	cJSON *Item = NULL;
	UINT8 *sub_pkg = NULL;
	INT32 ret = 0;

	root = cJSON_Parse(package);
	if(root == NULL)
	{
		return -1;
	}

	/*查找是否存在CfgList字段*/
	array = cJSON_GetObjectItem(root, KEY_STR_CFG_MQTT);
	if(array == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	
	sub_pkg = cJSON_PrintUnformatted(array);
	if(sub_pkg == NULL)
	{
		cJSON_Delete(root); 
		return -1;
	}
	
	ret = DeviceMqttCfgParse(handle, sub_pkg, 0);
	if(ret < 0)
	{
		SAFE_FREE(sub_pkg);
		cJSON_Delete(root); 
		return -1;
	}

	SAFE_FREE(sub_pkg);
	cJSON_Delete(root); 
	return 0;
}

UINT32 DeviceMqttCfgSave(void *handle, UINT8 *file)
{
	UINT8 *filebuff = NULL;
	cJSON *array = NULL;
	cJSON *root = NULL;
	cJSON *Item_mqtt = NULL;
	UINT8 *json = NULL;
	MQTT_CFG *mqtt_cfg = NULL;

	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;
	mqtt_cfg = &(dev->mqtt_cfg);
	
	filebuff = ReadFileAll(file);
	if(filebuff == NULL)
	{
		return 1;
	}

	root = cJSON_Parse(filebuff);
	if(root == NULL)
	{
		return 1;
	}

	if(strlen(mqtt_cfg->mqtt.host) == 0)
	{
		return 1;
	}
	
	Item_mqtt = cJSON_CreateObject();
	cJSON_AddStringToObject(Item_mqtt, KEY_STR_HOST, mqtt_cfg->mqtt.host);
	cJSON_AddNumberToObject(Item_mqtt, KEY_STR_PORT, mqtt_cfg->mqtt.port);
	cJSON_AddStringToObject(Item_mqtt, KEY_STR_USER, mqtt_cfg->mqtt.username);
	cJSON_AddStringToObject(Item_mqtt, KEY_STR_PWD, mqtt_cfg->mqtt.password);

	array = cJSON_GetObjectItem(root, KEY_STR_CFG_MQTT);
	if(array)
	{
		cJSON_DeleteItemFromObject(root, KEY_STR_CFG_MQTT);
	}
	
	cJSON_AddItemToObject(root, KEY_STR_CFG_MQTT, Item_mqtt);

	json = cJSON_PrintUnformatted(root);
	if(json == NULL)
	{
		SAFE_FREE(filebuff);
		cJSON_Delete(root); 
		return 1;
	}
	DeviceCfgSaveFormatted(file, json);
	SAFE_FREE(filebuff);
	SAFE_FREE(json);
	cJSON_Delete(root); 
	return 0;
}

static UINT32 device_chain_compare(CHAIN_CFG *chain1, CHAIN_CFG *chain2)
{
	if(chain1->data_onchain != chain2->data_onchain)
	{
		return 1;
	}

	//if(chain1->data_onchain == ONCHAIN_NO)
	//{
	//	return 0;
	//}

	if(chain1->type != chain2->type)
	{
		return 1;
	}

	if(chain1->type == CHAIN_SAMURAI)
	{
		if(strcmp(chain1->access_id, chain2->access_id))
		{
			return 1;
		}

		if(strcmp(chain1->access_key, chain2->access_key))
		{
			return 1;
		}
	}
	else
	{
		if(strcmp(chain1->token, chain2->token))
		{
			return 1;
		}
	}

	if(strcmp(chain1->domain, chain2->domain))
	{
		return 1;
	}

	return 0;
}


static INT32 device_chain_cfg_parse(DEV_DRIVER *dev, UINT8 *package)
{
	UINT32 ret = 0;
	UINT32 err_code = 0;
	UINT32 dec_len = 0;
	UINT8 *dec = NULL;
	CHAIN_CFG chain;
	CHAIN_CFG *chain_cfg = NULL;
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	memset(&chain, 0, sizeof(CHAIN_CFG));
	len = 0;
	tmp = JsonOnChainGet((void *)dev, package, &len);
	if(tmp)
	{
		if(strstr(tmp, "yes"))
			chain.data_onchain = ONCHAIN_YES;
		else
			chain.data_onchain = ONCHAIN_NO;
		SAFE_FREE(tmp);
	}
	else
	{
		return -1;
	}

	//if(chain.data_onchain == ONCHAIN_YES)
	//{
		len = 0;
		tmp = JsonRawTypeGet((void *)dev, package, &len);
		if(tmp)
		{
			chain.type = CHAIN_TYPE_INT(tmp);
			SAFE_FREE(tmp);
		}
		else
		{
			return -1;
		}

		if(chain.type == CHAIN_SAMURAI)
		{
			len = 0;
			tmp = JsonAccessIdGet((void *)dev, package, &len);
			if(tmp)
			{
				memcpy(chain.access_id, tmp, len);
				SAFE_FREE(tmp);
			}
			else
			{
				return -1;
			}

			len = 0;
			tmp = JsonAccessKeyGet((void *)dev, package, &len);
			if(tmp)
			{
				dec = DataDecryption(tmp, len, ENC_TYPE_AES_ECB_PKCS5,	
									 AES_KEY_DEV, strlen(AES_KEY_DEV), &err_code, 
									 &dec_len);
				if(dec == NULL)
				{
					return -1;
				}

				memcpy(chain.access_key, dec, dec_len);
				SAFE_FREE(dec);
				SAFE_FREE(tmp);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			len = 0;
			tmp = JsonTokenGet((void *)dev, package, &len);
			if(tmp)
			{
				memcpy(chain.token, tmp, len);
				SAFE_FREE(tmp);
			}
			else
			{
				return -1;
			}
		}

		len = 0;
		tmp = JsonDomainGet((void *)dev, package, &len);
		if(tmp)
		{
			memcpy(chain.domain, tmp, len);
			SAFE_FREE(tmp);
		}
		else
		{
			return -1;
		}
	//}

	chain.flag = 1;

	chain_cfg = &(dev->chain_cfg);
	
	ret = device_chain_compare(chain_cfg, &chain);
	if(ret)
	{
		memset(chain_cfg, 0, sizeof(dev->chain_cfg));
		memcpy(chain_cfg, &chain, sizeof(dev->chain_cfg));
		return 1;
	}
	
	return 0;
}

#if 1
INT32 DeviceCfgChainParse(void *handle, UINT8 *package)
{
	cJSON *root = NULL;
	cJSON *array = NULL;
	cJSON *Item = NULL;
	UINT8 *sub_pkg;
	INT32 ret = 0;
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	CFG_TYPE_E type = 0;

	root = cJSON_Parse(package);
	if(root == NULL)
	{
		return -1;
	}

	/*查找是否存在CfgList字段*/
	array = cJSON_GetObjectItem(root, KEY_STR_CFG_CHAIN);
	if(array != NULL)
	{
		sub_pkg = cJSON_PrintUnformatted(array);
		if(sub_pkg == NULL)
		{
			cJSON_Delete(root); 
			return 0;
		}
		ret = device_chain_cfg_parse(handle, sub_pkg);
		SAFE_FREE(sub_pkg);
	}
	else
	{
		/*查找是否存在CfgList字段*/
		array = cJSON_GetObjectItem(root, KEY_STR_CFG_LIST);
		if(array == NULL)
		{
			cJSON_Delete(root); 
			return 0;
		}

		/*遍历CfgList数组*/
		cJSON_ArrayForEach(Item, array)
		{
			sub_pkg = cJSON_Print(Item);

			/*获取配置类型*/
			len = 0;
			tmp = JsonCfgTypeGet(handle, sub_pkg, &len);
			if(tmp == NULL)
			{
				SAFE_FREE(sub_pkg);
				continue;
			}
			type = CFG_TYPE_INT(tmp);
			SAFE_FREE(tmp);

			if(type != CFG_TYPE_CHAIN)
			{
				SAFE_FREE(sub_pkg);
				continue;
			}

			ret = device_chain_cfg_parse(handle, sub_pkg);
			SAFE_FREE(sub_pkg);
		}
	}
	
	cJSON_Delete(root); 
	return ret;
}


#else
INT32 DeviceCfgChainParse(void *handle, UINT8 *package)
{
	cJSON *root = NULL;
	cJSON *array = NULL;
	cJSON *Item = NULL;
	UINT8 *sub_pkg;
	INT32 ret = 0;

	root = cJSON_Parse(package);
	if(root == NULL)
	{
		return -1;
	}

	/*查找是否存在CfgList字段*/
	array = cJSON_GetObjectItem(root, KEY_STR_CFG_CHAIN);
	if(array == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	
	sub_pkg = cJSON_PrintUnformatted(array);
	if(sub_pkg == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	ret = device_chain_cfg_parse(handle, sub_pkg);

	SAFE_FREE(sub_pkg);
	cJSON_Delete(root); 
	return ret;
}
#endif

UINT32 DeviceCfgChainSave(void *handle, UINT8 *file)
{
	UINT8 *filebuff = NULL;
	cJSON *array = NULL;
	cJSON *root = NULL;
	cJSON *Item = NULL;
	UINT8 *json = NULL;
	CHAIN_CFG *cfg = NULL;
	UINT8 *enc = NULL;
	UINT32 err_code = 0;

	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;
	if(dev->chain_cfg.flag = 0)
	{
		return 1;
	}
	cfg = &(dev->chain_cfg);

	filebuff = ReadFileAll(file);
	if(filebuff == NULL)
	{
		return 1;
	}

	root = cJSON_Parse(filebuff);
	if(root == NULL)
	{
		return 1;
	}

	Item = cJSON_CreateObject();
	if(cfg->data_onchain == ONCHAIN_YES)
	{
		cJSON_AddStringToObject(Item, KEY_STR_ONCHAIN, "yes");
	}
	else
	{
		cJSON_AddStringToObject(Item, KEY_STR_ONCHAIN, "no");
	}
		
	if(cfg->type)
	{
		cJSON_AddStringToObject(Item, KEY_STR_TYPE, CHAIN_TYPE_STR(cfg->type));
	}
	if(strlen(cfg->domain))
	{
		cJSON_AddStringToObject(Item, KEY_STR_DOMAIN, cfg->domain);
	}
	
	if(strlen(cfg->access_id))
	{
		cJSON_AddStringToObject(Item, KEY_STR_ACCESS_ID, cfg->access_id);
	}

	if(strlen(cfg->access_key))
	{
		enc = DataEncryption(cfg->access_key, 
							 strlen(cfg->access_key), 
							 ENC_TYPE_AES_ECB_PKCS5, 
							 AES_KEY_DEV, 
							 strlen(AES_KEY_DEV), 
							 &err_code);
		if(enc == NULL)
		{
			cJSON_Delete(root); 
			return 1;
		}
		cJSON_AddStringToObject(Item, KEY_STR_ACCESS_KEY, enc);
		SAFE_FREE(enc);
	}
	
	if(strlen(cfg->token))
	{
		cJSON_AddStringToObject(Item, KEY_STR_TOKEN, cfg->token);
	}	

	array = cJSON_GetObjectItem(root, KEY_STR_CFG_CHAIN);
	if(array)
	{
		cJSON_DeleteItemFromObject(root, KEY_STR_CFG_CHAIN);
	}

	array = cJSON_GetObjectItem(root, KEY_STR_CFG_LIST);
	if(array)
	{
		cJSON_DeleteItemFromObject(root, KEY_STR_CFG_LIST);
	}

	cJSON_AddItemToObject(root, KEY_STR_CFG_CHAIN, Item);

	json = cJSON_PrintUnformatted(root);
	if(json == NULL)
	{
		SAFE_FREE(filebuff);
		cJSON_Delete(root); 
		return 1;
	}
	DeviceCfgSaveFormatted(file, json);
	SAFE_FREE(filebuff);
	SAFE_FREE(json);
	cJSON_Delete(root); 

	return 0;
}


/*****************************************************************************
 * @Function	: cfg_parse_dev_key
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 从配置字串中获取设备的公私钥，若没有，则为该设备随机分配
 				  设备公钥：直接十六进制转字符串保存
 				  设备私钥：直接十六进制转字符串，进行AES ECB加密后，
 				  			再进行base64编码
 * @param[in]  	: DEV_DRIVER *dev  设备操作句柄
               	  UINT8 *package   配置JSON字串
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT32 device_key_cfg_parse(DEV_DRIVER *dev, UINT8 *package)
{
	UINT32 err_code = 0;
	UINT32 dec_len = 0;
	UINT8 *dec = NULL;
	UINT8 pri_key[PRI_KEY_MAX_LEN]; 
	UINT8 pub_key[PUB_KEY_MAX_LEN];

	UINT8 *tmp = NULL;
	UINT32 len = 0;
	UINT32 ret = 0;


	len = 0;
	tmp = JsonPrikeyGet((void *)dev, package, &len);
	if(tmp == NULL)
	{
		memset(pri_key, 0, sizeof(pri_key));
		memset(pub_key, 0, sizeof(pub_key));
		/*若不存在，则创建新的，并保存*/
		ret = KeyPairCreate(ENC_TYPE_ECDSA, pri_key, pub_key);
		if(ret)
		{
			return ret;
		}
		HexToStr(dev->dev_prikey, pri_key, PRI_KEY_MAX_LEN);
		HexToStr(dev->dev_pubkey, pub_key, PUB_KEY_MAX_LEN);
	}
	else
	{
		dec = DataDecryption(tmp, len, ENC_TYPE_AES_ECB_PKCS5,  
							 AES_KEY_DEV, strlen(AES_KEY_DEV), &err_code, 
							 &dec_len);
		if(dec == NULL)
		{
			SAFE_FREE(tmp);
			return err_code;
		}

		//StrToHex(pri, dec, dec_len/2);
		sprintf(dev->dev_prikey, "%s", dec);
		SAFE_FREE(dec);
		SAFE_FREE(tmp);

		len = 0;
		tmp = JsonPubkeyGet((void *)dev, package, &len);
		if(tmp)
		{
			//StrToHex(pub, key.key_value, key.value_len/2);
			sprintf(dev->dev_pubkey, "%s", tmp);
			SAFE_FREE(tmp);
		}
	}
	return 0;
}

/*****************************************************************************
 * @Function	: file_dev_key_save
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 将设备的公私钥保存到配置文件中
 				  设备公钥：直接十六进制转字符串保存
 				  设备私钥：直接十六进制转字符串，进行AES ECB加密后，
 				  			再进行base64编码
 * @param[in]  	: DEV_DRIVER *dev  设备句柄
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT32 device_key_cfg_save(DEV_DRIVER *dev, UINT8 *file)
{
	UINT32 ret = 0;
	data_package package;
	UINT8 *enc = NULL;
	UINT32 err_code = 0;

	memset(&package, 0, sizeof(package));
	PackageInit(&package);

	enc = DataEncryption(dev->dev_prikey, strlen(dev->dev_prikey), ENC_TYPE_AES_ECB_PKCS5, 
						 AES_KEY_DEV, strlen(AES_KEY_DEV),&err_code);
	if(enc == NULL)
	{
		PackageFree(&package);
		return 1;
	}

	ret = JsonPrikeyAdd((void *)dev, &package, enc);
	if(ret == 0)
	{
		SAFE_FREE(enc);
		PackageFree(&package);
		return 1;
	}
	SAFE_FREE(enc);

	ret = JsonPubkeyAdd((void *)dev, &package);
	if(ret == 0)
	{
		PackageFree(&package);
		return 1;
	}

	DeviceCfgSaveFormatted(file, package.data);
	PackageFree(&package);
	
	return 0;
	
}


UINT32 DeviceCfgIdParse(void *handle, UINT8 *data, UINT64 *cfgid)
{
	UINT32 len = 0;
	UINT8 *tmp = NULL;

	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return -1;
	}

	dev = (DEV_DRIVER *)handle;

	tmp = JsonResqIdGet((void *)dev, data, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy((UINT8 *)cfgid, tmp, len);
	SAFE_FREE(tmp);

	return 0;
}


UINT32 DeviceCfgIdSave(void *handle, UINT8 *file)
{
	UINT32 ret = 0;
	data_package package;
	UINT8 *filebuff = NULL;
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;

	filebuff = ReadFileAll(file);
	if(filebuff == NULL)
	{
		return 1;
	}

	memset(&package, 0, sizeof(package));
	PackageInit(&package);

	ret = PackageRawDataAdd(&package, filebuff, strlen(filebuff));
	if(ret == 0)
	{
		PackageFree(&package);
		SAFE_FREE(filebuff);
		return 1;
	}

	SAFE_FREE(filebuff);

	ret = JsonResqIdAdd((void *)dev, &package, dev->cfg_id);
	if(ret == 0)
	{
		PackageFree(&package);
		return 1;
	}

	DeviceCfgSaveFormatted(file, package.data);
	PackageFree(&package);
	
	return 0;
	
}

#if 0


INT32 DeviceCfgListParse(void *handle, UINT8 *data)
{
	cJSON *Item = NULL;
	cJSON *array = NULL;
	cJSON *root = NULL;
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	UINT8 *sub_pkg;
	CFG_TYPE_E type = 0;
	INT32 ret = 0;
	CHAIN_CFG chain_cfg;
	INT32 chain_result = 0;

	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return -1;
	}

	dev = (DEV_DRIVER *)handle;

	//dev->cfg_id = cfg_parse_cfgid(dev, data);

	root = cJSON_Parse(data);
	if(root == NULL)
	{
		return -1;
	}

	/*查找是否存在CfgList字段*/
	array = cJSON_GetObjectItem(root, KEY_STR_CFG_LIST);
	if(array == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}

	/*遍历CfgList数组*/
	cJSON_ArrayForEach(Item, array)
	{
		sub_pkg = cJSON_Print(Item);

		/*获取配置类型*/
		len = 0;
		tmp = JsonCfgTypeGet((void *)dev, sub_pkg, &len);
		if(tmp == NULL)
		{
			SAFE_FREE(sub_pkg);
			continue;
		}
		type = CFG_TYPE_INT(tmp);
		SAFE_FREE(tmp);

		switch(type)
		{
			case CFG_TYPE_CHAIN:
			{/*解析区块链存证相关配置*/
				memset(&chain_cfg, 0, sizeof(chain_cfg));
				chain_result = device_chain_cfg_parse(dev, sub_pkg, &chain_cfg);
				break;
			}
			default:
				LOG_PRINT(WX_LOG_ERROR, "Parse config fail, reason: unknown config type.");
				break;
		}
		SAFE_FREE(sub_pkg);
	}

	/*存在配置解析出错*/
	if(chain_result < 0)
	{
		//LOG_PRINT(WX_LOG_ERROR, "Device %s parse config fail.", dev->device_id);
		cJSON_Delete(root); 
		return -1;
	}

	/*所有配置与运行配置相同*/
	if(chain_result == 0)
	{
		cJSON_Delete(root);
		return 0;
	}

	/*区块链存证配置发生改变*/
	if(chain_result > 0)
	{
		memset(&dev->chain_cfg, 0, sizeof(dev->chain_cfg));
		memcpy(&dev->chain_cfg, &chain_cfg, sizeof(dev->chain_cfg));
	}

	cJSON_Delete(root); 
	return 1;
}

UINT32 DeviceCfgListSave(void *handle, UINT8 *file)
{
	UINT8 *filebuff = NULL;
	cJSON *array = NULL;
	cJSON *array_new = NULL;
	cJSON *root = NULL;
	cJSON *Item_chain = NULL;
	cJSON *Item_bp = NULL;
	cJSON *Item_file = NULL;
	UINT8 *json = NULL;
	UINT8 *enc = NULL;
	UINT32 err_code = 0;
	MQTT_CFG *mqtt_cfg = NULL;

	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;
	mqtt_cfg = &(dev->mqtt_cfg);
	
	filebuff = ReadFileAll(file);
	if(filebuff == NULL)
	{
		return 1;
	}

	root = cJSON_Parse(filebuff);
	if(root == NULL)
	{
		return 1;
	}

	array_new = cJSON_CreateArray();
	if(dev->chain_cfg.flag)
	{
		Item_chain = cJSON_CreateObject();
		cJSON_AddStringToObject(Item_chain, KEY_STR_CFG_TYPE, CFG_TYPE_STR(CFG_TYPE_CHAIN));
		if(dev->chain_cfg.data_onchain == ONCHAIN_YES)
			cJSON_AddStringToObject(Item_chain, KEY_STR_ONCHAIN, "yes");
		else
			cJSON_AddStringToObject(Item_chain, KEY_STR_ONCHAIN, "no");
		cJSON_AddStringToObject(Item_chain, KEY_STR_TYPE, CHAIN_TYPE_STR(dev->chain_cfg.type));
		cJSON_AddStringToObject(Item_chain, KEY_STR_DOMAIN, dev->chain_cfg.domain);
		cJSON_AddStringToObject(Item_chain, KEY_STR_ACCESS_ID, dev->chain_cfg.access_id);

		enc = DataEncryption(dev->chain_cfg.access_key, 
							 strlen(dev->chain_cfg.access_key), 
							 ENC_TYPE_AES_ECB_PKCS5, 
							 AES_KEY_DEV, 
							 strlen(AES_KEY_DEV), 
							 &err_code);
		if(enc == NULL)
		{
			cJSON_Delete(root); 
			return 1;
		}
		cJSON_AddStringToObject(Item_chain, KEY_STR_ACCESS_KEY, enc);
		SAFE_FREE(enc);

		cJSON_AddStringToObject(Item_chain, KEY_STR_TOKEN, dev->chain_cfg.token);
		cJSON_AddItemToArray(array_new, Item_chain);
	}

	array = cJSON_GetObjectItem(root, KEY_STR_CFG_LIST);
	if(array)
	{
		cJSON_DeleteItemFromObject(root, KEY_STR_CFG_LIST);
	}
	
	cJSON_AddItemToObject(root, KEY_STR_CFG_LIST, array_new);

	json = cJSON_Print(root);
	FileSave(file, json);
	SAFE_FREE(filebuff);
	SAFE_FREE(json);
	cJSON_Delete(root); 
	return 0;
}
#endif

/*****************************************************************************
 * @Function	: DeviceCfgInit
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 读取设备配置文件（网关本地保存），配置文件是以JSON格式保存
 * @param[in]  	: void *handle  设备操作句柄
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DeviceCfgKeyRead(void *handle, UINT8 *file)
{
	UINT8 *filebuff = NULL;
	UINT32 ret = 0;
	UINT8 pri_key[PRI_KEY_MAX_LEN]; 
	UINT8 pub_key[PUB_KEY_MAX_LEN];

	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if((access(file, F_OK)) != -1)   
    {   
        /*读出配置文件中所有内容*/
		filebuff = ReadFileAll(file);
		if(filebuff == NULL)
		{
			LOG_PRINT(WX_LOG_ERROR, "Read file %s fail, maybe not exist or empty!", file);
			return 1;
		}

		/*从配置文件中读取公私钥信息*/
		ret = device_key_cfg_parse(dev, filebuff);
		if(ret)
		{
			LOG_PRINT(WX_LOG_ERROR, "Json parse fail, data :%s", filebuff);
			SAFE_FREE(filebuff);
			return 1;
		}

		SAFE_FREE(filebuff);
    }   
	else
	{
		memset(pri_key, 0, sizeof(pri_key));
		memset(pub_key, 0, sizeof(pub_key));
		/*若不存在，则创建新的，并保存*/
		ret = KeyPairCreate(ENC_TYPE_ECDSA, pri_key, pub_key);
		if(ret)
		{
			return ret;
		}
		HexToStr(dev->dev_prikey, pri_key, PRI_KEY_MAX_LEN);
		HexToStr(dev->dev_pubkey, pub_key, PUB_KEY_MAX_LEN);
		/*配置保存*/
		/*该处再进行一次配置保存，是因为有些本地未有记录的设备，									*/
		/*在设备注册是会为其分配公私钥，该信息需要保存到配置文件中									*/
		/*目前子设备的公私钥，都是由网关设备分配											*/
		/*后续有设备身份认证平台，可能需要更改该流程											*/
		device_key_cfg_save(dev, file);
	}

	return 0;
}

static UINT32 device_file_server_compare(FILE_SERVER_CFG *cfg1, 
										 FILE_SERVER_CFG *cfg2)
{	 
	if(cfg1->interval != cfg2->interval)
	{
		return 1;
	}

	if(strcmp(cfg1->file_put, cfg2->file_put))
	{
		return 1;
	}

	if(strcmp(cfg1->file_get, cfg2->file_get))
	{
		return 1;
	}

	if(strcmp(cfg1->token, cfg2->token))
	{
		return 1;
	}
	return 0;
}


UINT32 DeviceCfgFileServerSave(void *handle, UINT8 *file)
{
	UINT8 *filebuff = NULL;
	cJSON *array = NULL;
	cJSON *root = NULL;
	cJSON *Item = NULL;
	UINT8 *json = NULL;
	FILE_SERVER_CFG *cfg = NULL;

	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;
	cfg = (FILE_SERVER_CFG *)dev->sub_dev_cfg;;

	filebuff = ReadFileAll(file);
	if(filebuff == NULL)
	{
		return 1;
	}

	root = cJSON_Parse(filebuff);
	if(root == NULL)
	{
		return 1;
	}

	Item = cJSON_CreateObject();
	if(strlen(cfg->file_put))
	{
		cJSON_AddStringToObject(Item, KEY_STR_FILE_PUT, cfg->file_put);
	}
	if(strlen(cfg->file_get))
	{
		cJSON_AddStringToObject(Item, KEY_STR_FILE_GET, cfg->file_get);
	}
	if(strlen(cfg->token))
	{
		cJSON_AddStringToObject(Item, KEY_STR_TOKEN, cfg->token);
	}
	cJSON_AddNumberToObject(Item, KEY_STR_INTERVAL, cfg->interval);
	

	array = cJSON_GetObjectItem(root, KEY_STR_CFG_FILE_SERVER);
	if(array)
	{
		cJSON_DeleteItemFromObject(root, KEY_STR_CFG_FILE_SERVER);
	}

	cJSON_AddItemToObject(root, KEY_STR_CFG_FILE_SERVER, Item);

	json = cJSON_PrintUnformatted(root);
	if(json == NULL)
	{
		SAFE_FREE(filebuff);
		cJSON_Delete(root); 
		return 1;
	}
	DeviceCfgSaveFormatted(file, json);
	SAFE_FREE(filebuff);
	SAFE_FREE(json);
	cJSON_Delete(root); 
	return 0;
}


/*****************************************************************************
 * @Function	: cfg_parse_filecfg
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 解析文件存储相关配置
 * @param[in]  	: DEV_DRIVER *dev  设备操作句柄
               	  UINT8 *package   配置信息字串
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static INT32 device_cfg_fileserver_parse(void *handle, UINT8 *package)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	UINT32 ret = 0;
	FILE_SERVER_CFG cfg;
	FILE_SERVER_CFG *cfg_p = NULL;
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return -1;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&cfg, 0, sizeof(cfg));
	len = 0;
	tmp = JsonPutFileGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(cfg.file_put, tmp, len);
		SAFE_FREE(tmp);
	}

	len = 0;
	tmp = JsonGetFileGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(cfg.file_get, tmp, len);
		SAFE_FREE(tmp);
	}

	len = 0;
	tmp = JsonTokenGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(cfg.token, tmp, len);
		SAFE_FREE(tmp);
	}

	len = 0;
	tmp = JsonIntervalGet((void *)dev, package, &len);
	{
		memcpy((UINT8 *)&(cfg.interval), tmp, len);
		SAFE_FREE(tmp);
	}

	cfg.flag = 1;

	cfg_p = (FILE_SERVER_CFG *)dev->sub_dev_cfg;
	
	ret = device_file_server_compare(&cfg, cfg_p);
	if(ret)
	{
		memset(cfg_p, 0, sizeof(FILE_SERVER_CFG));
		memcpy(cfg_p, &cfg, sizeof(FILE_SERVER_CFG));
		return 1;
	}

	return 0;
}

/*****************************************************************************
 * @Function	: cfg_parse_mqtt
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 解析MQTT相关配置
 * @param[in]  	: DEV_DRIVER *dev  设备句柄
               	  UINT8 *package   配置信息字串
 * @param[out]  : None
 * @return  	: 	-1:解析失败；
 *					 0:解析成功，但配置相同未改变
 *					 1:解析成功，配置不同
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
INT32 DeviceCfgFileServerParse(void *handle, UINT8 *package)
{
	cJSON *root = NULL;
	cJSON *array = NULL;
	cJSON *Item = NULL;
	UINT8 *sub_pkg;
	INT32 ret = 0;

	root = cJSON_Parse(package);
	if(root == NULL)
	{
		return -1;
	}

	/*查找是否存在CfgList字段*/
	array = cJSON_GetObjectItem(root, KEY_STR_CFG_FILE_SERVER);
	if(array == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	sub_pkg = cJSON_PrintUnformatted(array);
	if(sub_pkg == NULL)
	{
		cJSON_Delete(root);
		return 0;
	}
	ret = device_cfg_fileserver_parse(handle, sub_pkg);

	SAFE_FREE(sub_pkg);
	cJSON_Delete(root); 
	return ret;
}

static void rule_alg_add(RULE_INFO *rule, ALG_INFO *new_info)
{
	ALG_INFO *ptemp = NULL;

	if(new_info == NULL)
	{
		return ;
	}

	if (rule->alg_list == NULL)
	{
		new_info->next = NULL;
		rule->alg_list = new_info;
		return ;
	}

	for(ptemp=rule->alg_list; ptemp->next!=NULL; ptemp=ptemp->next)
	{
		;
	}

	ptemp->next = new_info;
	new_info->next = NULL;
	return ;
	
}

static void rule_alg_del(RULE_INFO *rule, ALG_INFO *del)
{
	ALG_INFO *ptemp = NULL;
	ALG_INFO *pre = NULL;

	if((rule == NULL) || (del == NULL) || (rule->alg_list == NULL))
	{
		return ;
	}


	pre = rule->alg_list;
	ptemp = rule->alg_list;
	if(ptemp == del)
	{
		rule->alg_list = NULL;
		return;
	}
	while(ptemp != NULL)
	{
		if(ptemp == del)
		{
			pre->next = del->next;
			break;
		}
		pre = ptemp;
		ptemp=ptemp->next;
	}

	return ;
	
}

static UINT32 average_alg_parse(DEV_DRIVER *dev, 
								RULE_INFO *rule_cfg, 
								UINT8 *package)
{
	ALG_AVE *alg = NULL;
	ALG_INFO *new_info = NULL;
	void *tmp = NULL;
	UINT8 *data = NULL;
	UINT32 len = 0;

	tmp = DevRuleAlgGet(rule_cfg, ALG_TYPE_AVERAGE);
	if(tmp == NULL)
	{
		alg = (ALG_AVE *)malloc(sizeof(ALG_AVE));
		if(alg == NULL)
		{
			return 1;
		}
		
		new_info = (ALG_INFO *)malloc(sizeof(ALG_INFO));
		if(new_info == NULL)
		{
			SAFE_FREE(alg);
			return 1;
		}
		memset(new_info, 0, sizeof(ALG_INFO));
		new_info->alg = (void *)alg;
		new_info->type = ALG_TYPE_AVERAGE;
		
		rule_alg_add(rule_cfg, new_info);
	}
	else
	{
		alg = (ALG_AVE *)tmp;
	}

	memset(alg, 0, sizeof(ALG_AVE));
	len = 0;
	data = JsonIntervalGet((void *)dev, package, &len);
	if(data == NULL)
	{
		SAFE_FREE(alg);
		return 1;
	}
	memcpy((UINT8 *)&(alg->interval), data, len);
	SAFE_FREE(data);

	len = 0;
	data = JsonSamplesGet((void *)dev, package, &len);
	if(data == NULL)
	{
		SAFE_FREE(alg);
		return 1;
	}
	memcpy((UINT8 *)&(alg->samples), data, len);
	SAFE_FREE(data);

	return 0;
}

static UINT32 bulletproof_alg_parse(DEV_DRIVER *dev, 
									RULE_INFO *rule_cfg, 
									UINT8 *package)
{
	ALG_PROOF *alg = NULL;
	ALG_INFO *new_info = NULL;
	void *tmp = NULL;
	UINT8 *data = NULL;
	UINT32 len = 0;

	tmp = DevRuleAlgGet(rule_cfg, ALG_TYPE_BULLET_PROOF);
	if(tmp == NULL)
	{
		alg = (ALG_PROOF *)malloc(sizeof(ALG_PROOF));
		if(alg == NULL)
		{
			return 1;
		}

		new_info = (ALG_INFO *)malloc(sizeof(ALG_INFO));
		if(new_info == NULL)
		{
			SAFE_FREE(alg);
			return 1;
		}
		memset(new_info, 0, sizeof(ALG_INFO));
		new_info->alg = (void *)alg;
		new_info->type = ALG_TYPE_BULLET_PROOF;
		
		rule_alg_add(rule_cfg, new_info);
	}
	else
	{
		alg = (ALG_PROOF *)tmp;
	}

	memset(alg, 0, sizeof(ALG_PROOF));

	len = 0;
	data = JsonRangeGet((void *)dev, package, &len);
	if(data == NULL)
	{
		SAFE_FREE(alg);
		return 1;
	}
	memcpy(alg->range, data, len);
	SAFE_FREE(data);

	#if 0
	len = 0;
	data = JsonUrlGet((void *)dev, package, &len);
	if(data == NULL)
	{
		SAFE_FREE(alg);
		return 1;
	}
	memcpy(alg->api_url, data, len);
	SAFE_FREE(data);
	#endif

	len = 0;
	data = JsonBpIndexGet((void *)dev, package, &len);
	if(data == NULL)
	{
		SAFE_FREE(alg);
		return 1;
	}
	memcpy((UINT8 *)&(alg->ct_index), data, len);
	SAFE_FREE(data);

	return 0;
}

static INT32 device_rule_compare(RULE_INFO *rule1, RULE_INFO *rule2)
{
	return 1;
}



/*****************************************************************************
 * @Function	: device_rule_parse
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 设备的rule配置解析
 * @param[in]  	: void *handle  设备句柄
               	  UINT8 *data   配置信息字串
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT32 device_rule_parse(void *handle, UINT8 *data, RULE_INFO *rule_new)
{
	cJSON *Item = NULL;
	cJSON *array = NULL;
	cJSON *root = NULL;
	ALG_TYPE_E type = 0;
	UINT8 *sub_pkg;
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	UINT32 ret = 0;
	RULE_INFO *rule_old;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	len = 0;
	tmp = JsonRuleNameGet(handle, data, &len);
	if(tmp)
	{
		memset(rule_new->rule_name, 0, sizeof(rule_new->rule_name));
		memcpy(rule_new->rule_name, tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		sprintf(rule_new->rule_name, "%s", RULE_DEFAULT_NAME);
	}

	len = 0;
	tmp = JsonIntervalGet(handle, data, &len);
	if(tmp)
	{
		memcpy((UINT8 *)&(rule_new->interval), tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		rule_new->interval = RULE_DEFAULT_INTERVAL;
	}

	root = cJSON_Parse(data);
	if(root == NULL)
	{
		return 1;
	}

	array = cJSON_GetObjectItem(root, KEY_STR_ALG_LIST);
	if(array == NULL)
	{
		cJSON_Delete(root); 
		return 1;
	}

	cJSON_ArrayForEach(Item, array)
	{
		sub_pkg = cJSON_PrintUnformatted(Item);
		if(sub_pkg == NULL)
		{
			continue;
		}
		
		len = 0;
		tmp = JsonAlgNameGet(handle, sub_pkg, &len);
		if(tmp == NULL)
		{
			SAFE_FREE(sub_pkg);
			continue;
		}

		type = ALG_TYPE_INT(tmp);
		SAFE_FREE(tmp);

		switch (type)
		{
			case ALG_TYPE_AVERAGE:
				ret = average_alg_parse(dev, rule_new, sub_pkg);
				break;
			case ALG_TYPE_BULLET_PROOF:
				ret = bulletproof_alg_parse(dev, rule_new, sub_pkg);
				break;
			default:
				break;
		}

		SAFE_FREE(sub_pkg);
	}

	cJSON_Delete(root); 

	rule_old = &(dev->rule_cfg);
	ret = device_rule_compare(rule_new, rule_old);
	if(ret)
	{
		//memset(rule_old, 0, sizeof(RULE_INFO));
		//memcpy(rule_old, &rule_new, sizeof(RULE_INFO));
		return 1;
	}
	
	return ret;
}


INT32 DeviceCfgRuleParse(void *handle, UINT8 *package, RULE_INFO *rule_new)
{
	cJSON *root = NULL;
	cJSON *array = NULL;
	//cJSON *Item = NULL;
	UINT8 *sub_pkg = NULL;
	INT32 ret = 0;

	root = cJSON_Parse(package);
	if(root == NULL)
	{
		return -1;
	}

	/*查找是否存在CfgList字段*/
	array = cJSON_GetObjectItem(root, KEY_STR_CFG_RULE);
	if(array == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	sub_pkg = cJSON_PrintUnformatted(array);
	if(sub_pkg == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	ret = device_rule_parse(handle, sub_pkg, rule_new);

	SAFE_FREE(sub_pkg);
	cJSON_Delete(root); 
	return ret;
}

static UINT32 _file_rule_save(data_package *pkg, void *rule, ALG_TYPE_E type)
{
	cJSON *array = NULL;
	cJSON *root = NULL;
	cJSON *Item_new = NULL;
	cJSON *Item = NULL;
	cJSON *old = NULL;
	UINT8 *json = NULL;
	int find = 0;
	
	if(rule == NULL)
	{
		return 0;
	}

	switch(type)
	{
		case ALG_TYPE_AVERAGE:
		{
			ALG_AVE *alg = (ALG_AVE *)rule;
			Item_new = cJSON_CreateObject();
			cJSON_AddStringToObject(Item_new, 
									KEY_STR_ALG_NAME, 
									ALG_TYPE_STR(type));
			cJSON_AddNumberToObject(Item_new, 
									KEY_STR_ALG_INTERVAL, 
									alg->interval);
			cJSON_AddNumberToObject(Item_new, 
									KEY_STR_ALG_SAMPLES, 
									alg->samples);
			break;
		}
		case ALG_TYPE_BULLET_PROOF:
		{
			ALG_PROOF *alg = (ALG_PROOF *)rule;
			Item_new = cJSON_CreateObject();
			cJSON_AddStringToObject(Item_new, 
									KEY_STR_ALG_NAME, 
									ALG_TYPE_STR(type));
			cJSON_AddStringToObject(Item_new, 
									KEY_STR_ALG_RANGE, 
									alg->range);
			if(strlen(alg->api_url))
			{
				cJSON_AddStringToObject(Item_new, 
										KEY_STR_URL, 
										alg->api_url);
			}
			cJSON_AddNumberToObject(Item_new, 
									KEY_STR_ALG_BP_INDEX, 
									alg->ct_index);
			break;
		}	
		default:
			break;
	}

	if(Item_new == NULL)
	{
		return 0;
	}

	if(pkg->data_len == 0)
	{
		root = cJSON_CreateObject();
	}
	else
	{
		root = cJSON_Parse(pkg->data);
	}

	if(root == NULL)
	{
		cJSON_Delete(Item_new); 
		return 0;
	}
	
	array = cJSON_GetObjectItem(root, KEY_STR_ALG_LIST);
	if(array != NULL)
	{	
		cJSON_ArrayForEach(Item, array)
		{
			old = cJSON_GetObjectItem(Item, KEY_STR_ALG_NAME);
			if((old != NULL) && cJSON_IsString(old))	
			{
				if(ALG_TYPE_INT(old->valuestring) == type)
				{
					cJSON_ReplaceItemViaPointer(array, Item, Item_new);
					find = 1;
					break;
				}
			}
		}

		if(find == 0)
		{
			cJSON_AddItemToArray(array, Item_new);
		}
	}
	else
	{
		array = cJSON_CreateArray();
		cJSON_AddItemToArray(array, Item_new);
		cJSON_AddItemToObject(root, KEY_STR_ALG_LIST, array);
	}

	json = (UINT8 *)cJSON_PrintUnformatted(root);	
	if(json == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	PackageInit(pkg);
	PackageRawDataAdd(pkg, json, strlen(json));
	SAFE_FREE(json);

	cJSON_Delete(root); 	
	return pkg->data_len;
}

static UINT32 _file_rule_delete(data_package *pkg, ALG_TYPE_E type)
{
	cJSON *array = NULL;
	cJSON *root = NULL;
	cJSON *Item = NULL;
	cJSON *old = NULL;
	UINT8 *json = NULL;
	int size = 0;
	int i = 0;
	

	if(pkg->data_len == 0)
	{
		return 0;
	}

	root = cJSON_Parse(pkg->data);
	if(root == NULL)
	{
		return 0;
	}
	
	array = cJSON_GetObjectItem(root, KEY_STR_ALG_LIST);
	if(array != NULL)
	{	
		/*获取json数组长度*/
		size = cJSON_GetArraySize(array);
	    for (i = 0; i < size; i++) 
		{
			Item = cJSON_GetArrayItem(array, i);
			old = cJSON_GetObjectItem(Item, KEY_STR_ALG_NAME);
			if((old != NULL) && cJSON_IsString(old))	
			{
				if(ALG_TYPE_INT(old->valuestring) == type)
				{
					cJSON_DeleteItemFromArray(array, i);
					break;
				}
			}
	    }
		/*删除动作完成之后，确认数组中是否还有元素，若没有，则删除该数组*/
		size = cJSON_GetArraySize(array);
		if(size == 0)
		{
			cJSON_DeleteItemFromObject(root, KEY_STR_ALG_LIST);
		}
	}

	json = (UINT8 *)cJSON_PrintUnformatted(root);	
	if(json == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	PackageInit(pkg);
	PackageRawDataAdd(pkg, json, strlen(json));
	SAFE_FREE(json);
	cJSON_Delete(root); 

	return pkg->data_len;
}


/*****************************************************************************
 * @Function	: FileRuleSave
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 将设备rule信息保存到配置文件中
 * @param[in]  	: void *handle  设备句柄
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DeviceCfgRuleSave(void *handle, UINT8 *file)
{
	UINT8 *filebuff = NULL;
	data_package pkg;
	int i = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	ALG_INFO *ptemp = NULL;
	ALG_INFO *head = NULL;
	UINT32 ret = 0;

	if ((dev == NULL) || (dev->rule_cfg.alg_list == NULL))
	{
		return 1;
	}

	filebuff = ReadFileAll(file);
	if(filebuff == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);
	PackageRawDataAdd(&pkg, filebuff, strlen(filebuff));
	//memcpy(pkg.data, filebuff, strlen(filebuff));
	//pkg.data_len = strlen(filebuff);
	SAFE_FREE(filebuff);

	ret = JsonRuleNameAdd(handle, &pkg, dev->rule_cfg.rule_name);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonIntervalAdd(handle, &pkg, dev->rule_cfg.interval);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	head = dev->rule_cfg.alg_list;
	for(ptemp=head; ptemp!=NULL; ptemp=ptemp->next)
	{
		_file_rule_save(&pkg, ptemp->alg, ptemp->type);
	}
	
	DeviceCfgSaveFormatted(file, pkg.data);
	PackageFree(&pkg);
	return 0;
}

/*****************************************************************************
 * @Function	: FileRuleDelete
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 删除配置文件中rule配置
 * @param[in]  	: void *handle  设备句柄
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DeviceCfgRuleDelete(void *handle)
{
	UINT8 *filebuff = NULL;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	ALG_INFO *ptemp = NULL;
	ALG_INFO *head = NULL;
	UINT32 ret = 0;
	CFG_KEY key;

	if((dev == NULL) || (dev->rule_cfg.alg_list == NULL))
	{
		return 1;
	}

	filebuff = ReadFileAll(dev->cfg_file);
	if(filebuff == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);
	PackageRawDataAdd(&pkg, filebuff, strlen(filebuff));
	//memcpy(pkg.data, filebuff, strlen(filebuff));
	//pkg.data_len = strlen(filebuff);
	SAFE_FREE(filebuff);

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_RULE_NAME;
	ret = PackageJsonDataDel(&pkg, &key);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_ALG_INTERVAL;
	ret = PackageJsonDataDel(&pkg, &key);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	head = dev->rule_cfg.alg_list;
	//ptemp = head;
	while(head != NULL)
	{
		ptemp = head;
		head = ptemp->next;
		_file_rule_delete(&pkg, ptemp->type);
		SAFE_FREE(ptemp->alg);
		SAFE_FREE(ptemp);
	}
	
	if(head == NULL)
	{
		dev->rule_cfg.alg_list = NULL;
	}

	DeviceCfgSaveFormatted(dev->cfg_file, pkg.data);
	PackageFree(&pkg);
	return 0;
}

static UINT32 device_cfg_bp_compare(BP_CFG *cfg1, BP_CFG *cfg2)
{
	if(cfg1->type != cfg2->type)
	{
		return 1;
	}

	if(strcmp(cfg1->domain, cfg2->domain))
	{
		return 1;
	}

	if(strcmp(cfg1->token, cfg2->token))
	{
		return 1;
	}

	if((cfg1->ct_cfg == NULL) && (cfg2->ct_cfg == NULL))
	{
		return 0;
	}

	if((cfg1->ct_cfg == NULL) || (cfg2->ct_cfg == NULL))
	{
		return 1;
	}

	if(strcmp(cfg1->ct_cfg, cfg2->ct_cfg))
	{
		return 1;
	}
	return 0;
}


/*****************************************************************************
 * @Function	: cfg_parse_bpcfg
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 解析BP相关配置
 * @param[in]  	: DEV_DRIVER *dev  设备句柄
               	  UINT8 *package   配置信息字串
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT32 device_cfg_bp_parse(DEV_DRIVER *dev, UINT8 *package)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	cJSON *root = NULL;
	cJSON *array = NULL;
	BP_CFG cfg_new;
	BP_CFG *cfg_old = NULL;
	INT32 ret = 0;

	cfg_old = &(dev->bp_cfg);

	memset(&cfg_new, 0, sizeof(BP_CFG));
	len = 0;
	tmp = JsonRawTypeGet((void *)dev, package, &len);
	if(tmp)
	{
		cfg_new.type = BP_TYPE_INT(tmp);
		SAFE_FREE(tmp);
	}
	
	len = 0;
	tmp = JsonDomainGet((void *)dev, package, &len);
	if(tmp)
	{
		memset(cfg_new.domain, 0, sizeof(cfg_new.domain));
		memcpy((UINT8 *)&(cfg_new.domain), tmp, len);
		SAFE_FREE(tmp);
	}

	len = 0;
	tmp = JsonTokenGet((void *)dev, package, &len);
	if(tmp)
	{
		memset(cfg_new.token, 0, sizeof(cfg_new.token));
		memcpy(cfg_new.token, tmp, len);
		SAFE_FREE(tmp);
	}

	/*解析BP标准化信息*/
	root = cJSON_Parse(package);
	if(root == NULL)
	{
		LOG_PRINT(WX_LOG_ERROR, "Json parse fail, data :%s", package);
		return 1;
	}
	array = cJSON_GetObjectItem(root, KEY_STR_BP_LIST);
	if(array)
	{
		cfg_new.ct_cfg = cJSON_PrintUnformatted(array);
	}
	
	cfg_new.flag = 1;

	ret = device_cfg_bp_compare(cfg_old, &cfg_new);
	if(ret)
	{
		SAFE_FREE(cfg_old->ct_cfg);
		memset(cfg_old, 0, sizeof(BP_CFG));
		memcpy(cfg_old, &cfg_new, sizeof(BP_CFG));
		cfg_old->ct_cfg = cfg_new.ct_cfg;
		return 1;
	}
	
	return 0;
}


/*****************************************************************************
 * @Function	: cfg_parse_mqtt
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 解析MQTT相关配置
 * @param[in]  	: DEV_DRIVER *dev  设备句柄
               	  UINT8 *package   配置信息字串
 * @param[out]  : None
 * @return  	: 	-1:解析失败；
 *					 0:解析成功，但配置相同未改变
 *					 1:解析成功，配置不同
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
INT32 DeviceCfgBpParse(void *handle, UINT8 *package)
{
	cJSON *root = NULL;
	cJSON *array = NULL;
	cJSON *Item = NULL;
	UINT8 *sub_pkg;
	INT32 ret = 0;

	root = cJSON_Parse(package);
	if(root == NULL)
	{
		return -1;
	}

	/*查找是否存在CfgList字段*/
	array = cJSON_GetObjectItem(root, KEY_STR_CFG_BP);
	if(array == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}

	sub_pkg = cJSON_PrintUnformatted(array);
	if(sub_pkg == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}
	ret = device_cfg_bp_parse(handle, sub_pkg);

	SAFE_FREE(sub_pkg);
	cJSON_Delete(root); 
	return ret;
}


UINT32 DeviceCfgBpSave(void *handle, UINT8 *file)
{
	UINT8 *filebuff = NULL;
	cJSON *array = NULL;
	cJSON *root = NULL;
	cJSON *Item = NULL;
	UINT8 *json = NULL;
	cJSON *ct_cfg = NULL;

	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;
	if(dev->bp_cfg.flag = 0)
	{
		return 1;
	}

	filebuff = ReadFileAll(file);
	if(filebuff == NULL)
	{
		return 1;
	}

	root = cJSON_Parse(filebuff);
	if(root == NULL)
	{
		return 1;
	}

	Item = cJSON_CreateObject();

			
	if(dev->bp_cfg.type)
	{
		cJSON_AddStringToObject(Item, KEY_STR_TYPE, BP_TYPE_STR(dev->bp_cfg.type));
	}
	if(strlen(dev->bp_cfg.domain))
	{
		cJSON_AddStringToObject(Item, KEY_STR_DOMAIN, dev->bp_cfg.domain);
	}
		
	if(strlen(dev->bp_cfg.token))
	{
		cJSON_AddStringToObject(Item, KEY_STR_TOKEN, dev->bp_cfg.token);
	}	

	if(dev->bp_cfg.ct_cfg)
	{
		ct_cfg = cJSON_Parse(dev->bp_cfg.ct_cfg);
		if(ct_cfg)
		{
			cJSON_AddItemToObject(Item, KEY_STR_BP_LIST, ct_cfg);
		}
	}

	array = cJSON_GetObjectItem(root, KEY_STR_CFG_BP);
	if(array)
	{
		cJSON_DeleteItemFromObject(root, KEY_STR_CFG_BP);
	}

	cJSON_AddItemToObject(root, KEY_STR_CFG_BP, Item);

	json = cJSON_PrintUnformatted(root);
	if(json == NULL)
	{
		SAFE_FREE(filebuff);
		cJSON_Delete(root); 
		return 1;
	}
	DeviceCfgSaveFormatted(file, json);
	SAFE_FREE(filebuff);
	SAFE_FREE(json);
	cJSON_Delete(root); 

	return 0;
}

UINT32 DeviceCfgSaveFormatted(UINT8 *file, UINT8 *filebuff)
{
	cJSON *root = NULL;
	UINT8 *json_str = NULL;

	root = cJSON_Parse((char *)filebuff);
	if(root == NULL)
	{
		return 1;
	}

	json_str = (UINT8 *)cJSON_Print(root);	
	if(json_str == NULL)
	{
		cJSON_Delete(root); 
		return 1;
	}

	FileSave(file, json_str);
	SAFE_FREE(json_str);
	return 0;
}


