#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_json.h"
#include "cJSON.h"
#include "http_client.h"
#include "log.h"
#include "samurai_client_api.h"
#include "hexutils.h"

UINT8 *CHAIN_TYPE_STR(CHAIN_TYPE_E type)
{
	UINT8 *tmp = NULL;
	
	switch(type)
	{
		case CHAIN_SAMURAI:
			tmp = (UINT8 *)CHAIN_SAMURAI_STR;
			break;
		case CHAIN_BCOP:
			tmp = (UINT8 *)CHAIN_BCOP_STR;
			break;
		case CHAIN_NONE:
			tmp = (UINT8 *)CHAIN_NONE_STR;
			break;
		default:
			tmp = (UINT8 *)"UNKNOWN";
			break;
	}
	return tmp;
}

CHAIN_TYPE_E CHAIN_TYPE_INT(UINT8 *type)
{
	if(strstr(type, CHAIN_SAMURAI_STR))
	{
		return CHAIN_SAMURAI;
	}
	else if(strstr(type, CHAIN_BCOP_STR))
	{
		return CHAIN_BCOP;
	}
	else
	{
		return CHAIN_NONE;
	}
}


static UINT32 setevidencedata(DEV_DRIVER *dev, UINT8 *data, 
							  UINT8 *hasher, UINT8 *sub_url)
{
	UINT32 ret = 0;

	http_respone post_return;
	data_package package;
	UINT8 url[128];	
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	memset(url, 0, sizeof(url));
	sprintf(url, "%s%s", dev->chain_cfg.domain, sub_url);

	memset(&package, 0, sizeof(package));
	PackageInit(&package);
	ret = JsonEvidenceAdd((void *)dev, &package, data);
	if(ret == 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s data on blockchain fail.",
				  dev->device_id);
		dev->dev_status = STATUS_DATA_ONCHAIN_FAIL;
		PackageFree(&package);
		return 1;
	}

	/*4.HTTP data post*/
	memset(&post_return, 0, sizeof(post_return));
	ret = http_post((char *)url, (char *)package.data, package.data_len, 
					(void *)&post_return, dev->chain_cfg.token);
	if(ret)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s data on blockchain fail.",
				  dev->device_id);
		dev->dev_status = STATUS_DATA_ONCHAIN_FAIL;
		PackageFree(&package);
		return 1;
	}

	if((post_return.data == NULL) || (post_return.data_len == 0))
	{
		PackageFree(&package);
		return 1;
	}

	len = 0;
	tmp = JsonRetCodeGet((void *)dev, post_return.data, &len);
	if(tmp == NULL)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s data on blockchain fail, msg:%s.",
				  dev->device_id, post_return.data);
		dev->dev_status = STATUS_DATA_ONCHAIN_FAIL;
		SAFE_FREE(post_return.data);
		PackageFree(&package);
		return 1;
	}
	memcpy(&ret, tmp, len);
	SAFE_FREE(tmp);
	if(ret != ERR_REQUEST_SUCCESS)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s data on blockchain fail, msg:%s.",
				  dev->device_id, post_return.data);
		dev->dev_status = STATUS_DATA_ONCHAIN_FAIL;
		SAFE_FREE(post_return.data);
		PackageFree(&package);
		return 1;
	}

	len = 0;
	tmp = JsonRawDataGet((void *)dev, post_return.data, &len);
	if(tmp == NULL)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s data on blockchain fail, msg:%s.",
				  dev->device_id, post_return.data);
		dev->dev_status = STATUS_DATA_ONCHAIN_FAIL;
		SAFE_FREE(post_return.data);
		PackageFree(&package);
		return 1;
	}

	sprintf(hasher, "%s", tmp);
	SAFE_FREE(tmp);
	SAFE_FREE(post_return.data);
	PackageFree(&package);
	return 0;
}


UINT32 DataOnchain(void *handle, UINT8 *data, UINT32 data_len, UINT8 *hasher)
{
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	UINT32 ret = 0;

	
	switch(dev->chain_cfg.type)
	{
		case CHAIN_SAMURAI:
		{
			DATA_ONCHAIN_RESPONE respone;
			memset(&respone, 0, sizeof(respone));
			ret = TextHashPoeSave(dev->chain_cfg.domain, 
								  dev->chain_cfg.access_id, 
								  dev->chain_cfg.access_key, 
								  data, data_len, &respone);
			if(ret || (respone.SaveRetCode != ERR_SUCCESS) 
			   && (respone.SaveRetCode != ERR_REQUEST_SUCCESS))
			{
				LOG_PRINT(  Prome_LOG_ERROR, "Device %s data on blockchain fail, msg:%s.",
				dev->device_id, respone.SaveRetDesc);
				dev->dev_status = STATUS_DATA_ONCHAIN_FAIL;
				return ret;
			}
			HexToStr(hasher, respone.SavePoeHash, respone.HashLen);
			ret = 0;
			break;
		}
		case CHAIN_BCOP:
			ret = setevidencedata(dev, data, hasher, CHAIN_ON_CHAIN_URL);
			break;
		default:
			ret = 1;
			break;
	}

	
	return ret;
}


