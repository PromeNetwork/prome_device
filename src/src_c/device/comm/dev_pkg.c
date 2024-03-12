#include <string.h>

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_json.h"
#include "dev_rule.h"
#include "hexutils.h"
#include "algorithm_api.h"
#include "log.h"
#include "package.h"
#include "mosquitto_api.h"
#include "cJSON_Utils.h"


UINT8 *PKG_TYPE_STR(PKG_TYPE_E type)
{
	UINT8 *tmp = NULL;
	
	switch(type)
	{
		case PKG_TYPE_DEV_ONLINE:
			tmp = (UINT8 *)PKG_TYPE_DEV_ONLINE_STR;
			break;
		case PKG_TYPE_DATA_REPORT:
			tmp = (UINT8 *)PKG_TYPE_DATA_REPORT_STR;
			break;
		case PKG_TYPE_DEV_CFG:
			tmp = (UINT8 *)PKG_TYPE_DEV_CFG_STR;
			break;
		case PKG_TYPE_RULE_RESULT:
			tmp = (UINT8 *)PKG_TYPE_RULE_RESULT_STR;
			break;
		case PKG_TYPE_DEV_OFFLINE:
			tmp = (UINT8 *)PKG_TYPE_DEV_OFFLINE_STR;
			break;
		#if 0
		case PKG_TYPE_DEV_UPGRADE:
			tmp = (UINT8 *)PKG_TYPE_DEV_UPGRADE_STR;
			break;
		#endif
		case PKG_TYPE_DEV_REQUEST:
			tmp = (UINT8 *)PKG_TYPE_DEV_REQUEST_STR;
			break;
		case PKG_TYPE_DEV_ATTRIBUTE:
			tmp = (UINT8 *)PKG_TYPE_DEV_ATTRIBUTE_STR;
			break;
		case PKG_TYPE_DEV_RESPONSE:
			tmp = (UINT8 *)PKG_TYPE_DEV_REPONSE_STR;
			break;
		case PKG_TYPE_DEV_TIME:
			tmp = (UINT8 *)PKG_TYPE_DEV_TIME_STR;
			break;
		case PKG_TYPE_DEV_RPC_MQTT_RESP:
			tmp = (UINT8 *)PKG_TYPE_DEV_RPC_MQTT_RESP_STR;
			break;
		case PKG_TYPE_DEV_RPC_UPGRADE_RESP:
			tmp = (UINT8 *)PKG_TYPE_DEV_RPC_UPGRADE_RESP_STR;
			break;
		case PKG_TYPE_DEV_RPC_DEV_INFO_RESP:
			tmp = (UINT8 *)PKG_TYPE_DEV_RPC_DEV_INFO_RESP_STR;
			break;
		default:
			tmp = (UINT8 *)"UNKNOWN";
			break;
	}
	return tmp;
}


#if 0
/*****************************************************************************
 * @Function	: rule_result_to_string
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 将设备rule执行结果JSON转成字符串，由于IoT -
                  平台无法处理JSON嵌套，暂时转成字符串处理
 * @param[in]  	: DEV_DRIVER *dev  设备句柄
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT8 *device_pkg_rule_result(DEV_DRIVER *dev)
{
	cJSON *array = NULL;
	cJSON *ave = NULL;
	cJSON *proof = NULL;
	UINT8 *json_str = NULL;
	ALG_INFO *ptemp = NULL;
	ALG_INFO *head = NULL;
	ALG_AVE *alg_ave = NULL;
	ALG_PROOF *alg_proof = NULL;

	head = dev->rule.alg_list;
	if (head == NULL)
	{
		return NULL;
	}
	array = cJSON_CreateArray();
	for(ptemp=head; ptemp!=NULL; ptemp=ptemp->next)
	{
		switch (ptemp->type)
		{
			case ALG_TYPE_AVERAGE:
				alg_ave = (ALG_AVE *)ptemp->alg;
				ave = cJSON_CreateObject();
				cJSON_AddNumberToObject(ave, KEY_STR_ALG_SAMPLES, alg_ave->samples);
				cJSON_AddNumberToObject(ave, KEY_STR_ALG_INTERVAL, alg_ave->interval);
				cJSON_AddNumberToObject(ave, KEY_STR_ALG_VALUE, alg_ave->value);
				cJSON_AddStringToObject(ave, KEY_STR_POEHASH, alg_ave->hasher);
				cJSON_AddItemToArray(array, ave);
				break;
			case ALG_TYPE_BULLET_PROOF:
				alg_proof = (ALG_PROOF *)ptemp->alg;
				proof = cJSON_CreateObject();
				cJSON_AddStringToObject(proof, KEY_STR_ALG_PROOF, alg_proof->proof);
				cJSON_AddStringToObject(proof, KEY_STR_ALG_BPID, alg_proof->pid);
				cJSON_AddNumberToObject(proof, KEY_STR_ALG_BP_INDEX, alg_proof->ct_index);
				cJSON_AddItemToArray(array, proof);
				break;
			default:
				break;
		}
	}

	json_str = (UINT8 *)cJSON_PrintUnformatted(array);	
	if(json_str == NULL)
	{
		cJSON_Delete(array); 
		return NULL;
	}
	cJSON_Delete(array); 
	return json_str;
}
#endif

/*****************************************************************************
 * @Function	: device_data_sign
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 使用设备的私钥为指定数据进行签名
 * @param[in]  	: void *handle  设备句柄
               	  UINT8 *data   数据内容
               	  UINT32 len    数据长度
               	  UINT8 *msg    存储签名信息指针
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT32 device_pkg_sign(void *handle, UINT8 *data, UINT32 len, UINT8 *msg)
{
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	UINT8 pri_key[PRI_KEY_MAX_LEN];
	UINT8 signed_msg[SIGN_MAX_LEN];

	memset(pri_key, 0, sizeof(pri_key));
	memset(signed_msg, 0, sizeof(signed_msg));
	StrToHex(pri_key, dev->dev_prikey, strlen(dev->dev_prikey)/2);
	ret = DataSignature(data, len, SIGN_ECDSA, pri_key, signed_msg);
	if(ret)
	{
		return 1;
	}
	HexToStr(msg, signed_msg, sizeof(signed_msg));
	return 0;
}

static UINT32 device_pkg_basic_info(DEV_DRIVER *dev, data_package *pkg)
{
	UINT32 ret = ERR_SUCCESS;
	TIMESTAMP stamp = 0;

	if(dev == NULL || pkg == NULL)
	{
		return 1;
	}
	
	ret = JsonSoftVerAdd((void *)dev, pkg);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonProcotolVerAdd((void *)dev, pkg);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPubkeyAdd((void *)dev, pkg);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonDevIdAdd((void *)dev, pkg);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonDevTypeAdd((void *)dev, pkg, dev->dev_type);
	if(ret == 0)
	{
		return 1;
	}

	stamp = GetTimestampInt();
	ret = JsonTimeStampAdd((void *)dev, pkg, stamp);
	if(ret == 0)
	{
		return 1;
	}

	return 0;
}

/*****************************************************************************
 * @Function	: device_pkg_send
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 数据发送接口
 * @param[in]  	: DEV_DRIVER *dev     设备句柄
               	  UINT8 *data         数据
               	  UINT32 len          数据长度
               	  PKG_TYPE_E pkgtype  数据包类型
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT32 device_pkg_send(DEV_DRIVER *dev, data_package *pkg, 
							  PKG_TYPE_E pkgtype, ON_CHAIN flag)
{
	UINT32 ret = 0;
	UINT8 topic[256];
	UINT8 sig[SIGN_STR_MAX_LEN];
	UINT8 PoeHash[HASH_STR_MAX_LEN];
	cJSON *root = NULL;
	UINT8 *json_str = NULL;
	
	ret = device_pkg_basic_info(dev, pkg);
	if(ret)
	{
		return 1;
	}

	#if 1
	root = cJSON_Parse((char *)pkg->data);
	if(root == NULL)
	{
		return 1;
	}

	cJSONUtils_SortObject(root);
	json_str = (UINT8 *)cJSON_PrintUnformatted(root);	
	if(json_str == NULL)
	{
		cJSON_Delete(root); 
		return 1;
	}

	PackageInit(pkg);
	PackageRawDataAdd(pkg, json_str, strlen((char *)json_str));
	cJSON_Delete(root); 
	SAFE_FREE(json_str);
	
	memset(sig, 0, sizeof(sig));
	ret = device_pkg_sign(dev, pkg->data, pkg->data_len, sig);
	if(ret)
	{
		return 1;
	}

	ret = JsonSignAdd(dev, pkg, sig);
	if(ret == 0)
	{
		return 1;
	}
	#endif
	
	memset(PoeHash, 0, sizeof(PoeHash));
	//printf("pkgtype:%d, flag:%d\r\n", pkgtype, flag);
	if(dev->dev_type != DEV_TYPE_GATEWAY)
	{
		if(dev->chain_cfg.data_onchain && flag)
		{		
			/*4.将整个数据包，哈希存证*/
			if(dev->data_onchain == NULL) 
			{
				dev->dev_status = STATUS_DATA_ONCHAIN_FAIL;
				return 1;
			}
			
			ret = dev->data_onchain(dev, pkg->data, pkg->data_len, PoeHash);
			if(ret)
			{
				dev->dev_status = STATUS_DATA_ONCHAIN_FAIL;
				return 1;
			}
			LOG_PRINT(WX_LOG_INFO, "Device %s data on blockchain ok.", dev->device_id);

			ret = JsonHashAdd((void *)dev, pkg, PoeHash);
			if(ret == 0)
			{
				dev->dev_status = STATUS_DATA_PKG_FAIL;
				return 1;
			}
		}
	}

	memset(topic, 0, sizeof(topic));

	switch(pkgtype)
	{
		case PKG_TYPE_DEV_REQUEST:
			sprintf(topic, "devices/%s/%s/config", 
						PKG_TYPE_STR(PKG_TYPE_DEV_REQUEST),
						dev->dev_pubkey);
			break;
		case PKG_TYPE_DEV_RESPONSE:
			sprintf(topic, "devices/%s/%s/config", 
						PKG_TYPE_STR(PKG_TYPE_DEV_RESPONSE),
						dev->dev_pubkey);
			break;
		case PKG_TYPE_DEV_ONLINE:
		case PKG_TYPE_DATA_REPORT:
		case PKG_TYPE_DEV_CFG:
		case PKG_TYPE_RULE_RESULT:
		case PKG_TYPE_DEV_OFFLINE:
		case PKG_TYPE_DEV_ATTRIBUTE:
		case PKG_TYPE_DEV_RULE:
			sprintf(topic, "devices/%s/%s/%s", PKG_TYPE_STR(pkgtype), 
							DEV_TYPE_STR(dev->dev_type), 
							dev->dev_pubkey);
			break;
		#if 0
		case PKG_TYPE_DEV_UPGRADE:
			sprintf(topic, "devices/%s/%s/upgrade", 
							PKG_TYPE_STR(PKG_TYPE_DEV_REQUEST),
							dev->dev_pubkey);
			break;
		#endif
		case PKG_TYPE_DEV_TIME:
			sprintf(topic, "devices/%s/%s/time", 
							PKG_TYPE_STR(PKG_TYPE_DEV_REQUEST),
							dev->dev_pubkey);
			break;
		case PKG_TYPE_DEV_RPC_MQTT_RESP:
		{
			UINT8 *tmp = NULL;
			TIMESTAMP resqid = 0;
			UINT32 len = 0;

			tmp = JsonResqIdGet((void *)dev, pkg->data, &len);
			if(tmp == NULL)
			{
				return 1;
			}
			memcpy((UINT8 *)&resqid, tmp, len);
			SAFE_FREE(tmp);
			sprintf(topic,  RPC_RESPONSE_TOPIC, 
							dev->dev_pubkey, 
							RPC_METHOD_MQTT, 
							resqid);
			break;
		}
		case PKG_TYPE_DEV_RPC_UPGRADE_RESP:
		{
			UINT8 *tmp = NULL;
			TIMESTAMP resqid = 0;
			UINT32 len = 0;

			tmp = JsonResqIdGet((void *)dev, pkg->data, &len);
			if(tmp == NULL)
			{
				return 1;
			}
			memcpy((UINT8 *)&resqid, tmp, len);
			SAFE_FREE(tmp);
			sprintf(topic,  RPC_RESPONSE_TOPIC, 
							dev->dev_pubkey, 
							RPC_METHOD_UPGRADE, 
							resqid);
			break;
		}
		case PKG_TYPE_DEV_RPC_DEV_INFO_RESP:
		{
			UINT8 *tmp = NULL;
			TIMESTAMP resqid = 0;
			UINT32 len = 0;

			tmp = JsonResqIdGet((void *)dev, pkg->data, &len);
			if(tmp == NULL)
			{
				return 1;
			}
			memcpy((UINT8 *)&resqid, tmp, len);
			SAFE_FREE(tmp);
			sprintf(topic,  RPC_RESPONSE_TOPIC, 
							dev->dev_pubkey, 
							RPC_METHOD_DEV_INFO, 
							resqid);
			break;
		}
			
	}
	#if 0
	//printf("pkg->data:%s\r\n", pkg->data);
	ret = MqttConnect(&(dev->mqtt_cfg.mqtt), 
					 (struct mosquitto **)&dev->mqtt_cfg.receiver_mosq);
					 //(struct mosquitto **)&dev->mqtt_cfg.report_mosq);
	if(ret)
	{
		LOG_PRINT(WX_LOG_ERROR, "Device %s connect to mqtt broker fail.", 
							  dev->device_id);
		return ret;
	}
	
	ret = DeviceServerConnect((void *)dev);
	if(ret)
	{
		LOG_PRINT(WX_LOG_ERROR, "Device %s connect to mqtt broker fail.", 
							  dev->device_id);
		return ret;
	}
	#endif
	
	ret = MqttPublish(topic, pkg->data, pkg->data_len, dev->mqtt_cfg.mqtt.mosq);
					 //(struct mosquitto *)dev->mqtt_cfg.report_mosq);
	if(ret)
	{
		LOG_PRINT(WX_LOG_ERROR, "Mqtt publish topic %s fail, ret:%d.", topic, ret);
		//DeviceServerDisconnect((void *)dev);
		return ret;
	}

	LOG_PRINT(WX_LOG_INFO, "Device %s send %s package to platform ok.", 
			  dev->device_id, PKG_TYPE_STR(pkgtype));
	dev->dev_status = STATUS_SUCCESS;

	return 0;
}

UINT32 DevicePkgOnline(void *handle)
{
	UINT32 ret = ERR_SUCCESS;
	UINT32 pkg_type = PKG_TYPE_DEV_ONLINE;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);
	
	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);;
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}

#if 0
UINT32 DevicePkgUpgradeReq(void *handle)
{
	UINT32 ret = ERR_SUCCESS;
	UINT32 pkg_type = PKG_TYPE_DEV_UPGRADE;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);

	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);;
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}


UINT32 DevicePkgUpgradeRes(void *handle, UINT8 *data, UINT32 len, 
						   DEVICE_STATUS status)
{
	UINT32 ret = ERR_SUCCESS;
	UINT32 pkg_type = PKG_TYPE_DEV_RESPONSE;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);

	PackageDataAdd(&pkg, data, len);

	ret = JsonDevStatusAdd((void *)dev, &pkg, status);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}
	
	ret = JsonDescAdd((void *)dev, &pkg, DEV_STATUS_STR(status));
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}
#endif


UINT32 DevicePkgCfgReq(void *handle)
{
	UINT32 ret = ERR_SUCCESS;
	UINT32 pkg_type = PKG_TYPE_DEV_REQUEST;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);

	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);;
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}

UINT32 DevicePkgCfgRes(void *handle, UINT8 *data, UINT32 len, 
					   DEVICE_STATUS status)
{
	UINT32 ret = ERR_SUCCESS;
	UINT32 pkg_type = PKG_TYPE_DEV_RESPONSE;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);

	PackageRawDataAdd(&pkg, data, len);

	ret = JsonDevStatusAdd((void *)dev, &pkg, status);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}
	
	ret = JsonDescAdd((void *)dev, &pkg, DEV_STATUS_STR(status));
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);;
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}


UINT32 DevicePkgTimeReq(void *handle)
{
	UINT32 ret = ERR_SUCCESS;
	UINT32 pkg_type = PKG_TYPE_DEV_TIME;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);

	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);;
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}

UINT32 DevicePkgReportError(void *handle, void *data)
{
	UINT32 ret = ERR_SUCCESS;
	UINT32 pkg_type = PKG_TYPE_DATA_REPORT;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);

	ret = dev->data_pkg(dev, data, &pkg);
	if(ret != ERR_SUCCESS)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonDevStatusAdd((void *)dev, &pkg, dev->dev_status);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonDescAdd((void *)dev, &pkg, DEV_STATUS_STR(dev->dev_status));
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);

	return 0;
}

UINT32 DevicePkgReport(void *handle, void *data)
{
	UINT32 ret = ERR_SUCCESS;
	UINT32 pkg_type = PKG_TYPE_DATA_REPORT;
	data_package pkg;
	DEV_DRIVER *dev = NULL;

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);

	if(handle == NULL || data == NULL) 
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;

	if(dev->dev_status != STATUS_SUCCESS)
	{
		//dev->dev_status = STATUS_DATA_PKG_FAIL;
		return 1;
	}

	if(dev->data_pkg == NULL) 
	{
		dev->dev_status = STATUS_DATA_PKG_FAIL;
		return 1;
	}

	ret = dev->data_pkg(dev, data, &pkg);
	if(ret != ERR_SUCCESS)
	{
		PackageFree(&pkg);
		dev->dev_status = STATUS_DATA_PKG_FAIL;
		return 1;
	}
	ret = JsonDevStatusAdd((void *)dev, &pkg, dev->dev_status);
	if(ret == 0)
	{
		PackageFree(&pkg);
		dev->dev_status = STATUS_DATA_PKG_FAIL;
		return 1;
	}

	ret = JsonDescAdd((void *)dev, &pkg, DEV_STATUS_STR(dev->dev_status));
	if(ret == 0)
	{
		PackageFree(&pkg);
		dev->dev_status = STATUS_DATA_PKG_FAIL;
		return 1;
	}

	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);
	if(ret == 0)
	{
		PackageFree(&pkg);
		dev->dev_status = STATUS_DATA_PKG_FAIL;
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_YES);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);

	return 0;
}

/*****************************************************************************
 * @Function	: rule_result_to_string
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 将设备rule执行结果JSON转成字符串，由于IoT -
                  平台无法处理JSON嵌套，暂时转成字符串处理
 * @param[in]  	: DEV_DRIVER *dev  设备句柄
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT8 *rule_result_to_string(DEV_DRIVER *dev)
{
	cJSON *array = NULL;
	cJSON *ave = NULL;
	cJSON *proof = NULL;
	UINT8 *json_str = NULL;
	ALG_INFO *ptemp = NULL;
	ALG_INFO *head = NULL;
	ALG_AVE *alg_ave = NULL;
	ALG_PROOF *alg_proof = NULL;

	head = dev->rule_cfg.alg_list;
	if (head == NULL)
	{
		return NULL;
	}
	array = cJSON_CreateArray();
	for(ptemp=head; ptemp!=NULL; ptemp=ptemp->next)
	{
		switch (ptemp->type)
		{
			case ALG_TYPE_AVERAGE:
				alg_ave = (ALG_AVE *)ptemp->alg;
				ave = cJSON_CreateObject();
				cJSON_AddNumberToObject(ave, KEY_STR_ALG_SAMPLES, alg_ave->samples);
				cJSON_AddNumberToObject(ave, KEY_STR_ALG_INTERVAL, alg_ave->interval);
				cJSON_AddNumberToObject(ave, KEY_STR_ALG_VALUE, alg_ave->value);
				cJSON_AddStringToObject(ave, KEY_STR_POEHASH, alg_ave->hasher);
				cJSON_AddItemToArray(array, ave);
				break;
			case ALG_TYPE_BULLET_PROOF:
				alg_proof = (ALG_PROOF *)ptemp->alg;
				proof = cJSON_CreateObject();
				cJSON_AddStringToObject(proof, KEY_STR_ALG_PROOF, alg_proof->proof);
				cJSON_AddStringToObject(proof, KEY_STR_ALG_BPID, alg_proof->pid);
				cJSON_AddNumberToObject(proof, KEY_STR_ALG_BP_INDEX, alg_proof->ct_index);
				cJSON_AddItemToArray(array, proof);
				break;
			default:
				break;
		}
	}

	json_str = (UINT8 *)cJSON_PrintUnformatted(array);	
	if(json_str == NULL)
	{
		cJSON_Delete(array); 
		return NULL;
	}
	cJSON_Delete(array); 
	return json_str;
}


/*****************************************************************************
 * @Function	: rule_result_pkg
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 将设备rule执行结果封装报文
 * @param[in]  	: void *handle           设备句柄
               	  data_package *package  数据包
               	  RULE_STATUS status     rule执行状态
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DevicePkgRuleResult(void *handle, RULE_STATUS status)
{
	UINT32 ret = ERR_SUCCESS;
	DEV_DRIVER *dev = NULL;
	UINT8 *tmp = NULL;
	data_package pkg;
	UINT32 pkg_type = PKG_TYPE_RULE_RESULT;

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);
	
	dev = (DEV_DRIVER *)handle;
	if(dev == NULL)
	{
		return 1;
	}

	if (dev->rule_cfg.alg_list == NULL)
	{
		return 1;
	}

	ret = JsonPkgTypeAdd(handle, &pkg, pkg_type);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}
	
	ret = JsonDevStatusAdd(handle, &pkg, status);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonDescAdd(handle, &pkg, RULE_STATUS_STR(status));
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonRuleNameAdd(handle, &pkg, dev->rule_cfg.rule_name);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}
	
	tmp = rule_result_to_string(dev);
	if(tmp)
	{
		ret = JsonAlgListAdd(handle, &pkg, tmp);
		if(ret == 0)
		{
			PackageFree(&pkg);
			return 1;
		}
		SAFE_FREE(tmp);
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}

UINT32 DevicePkgRpcResp(void *handle, 
						UINT8 *data, 
						UINT32 len, 
						UINT64 request_id,
						PKG_TYPE_E pkg_type)
{
	UINT32 ret = ERR_SUCCESS;
	data_package pkg;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);
	
	PackageRawDataAdd(&pkg, data, len);

	ret = JsonResqIdAdd(handle, &pkg, request_id);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonDevStatusAdd((void *)dev, &pkg, dev->dev_status);
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = JsonPkgTypeAdd((void *)dev, &pkg, pkg_type);;
	if(ret == 0)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = device_pkg_send(dev, &pkg, pkg_type, ONCHAIN_NO);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}


