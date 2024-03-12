#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> 


#include "basic_types.h"
#include "dev_comm.h"
#include "dev_json.h"
#include "cJSON.h"
#ifdef BULLETPROOF_LIB
#include "libbulletproof.h"
#endif
#include "http_client.h"
#include "bulletproof.h"
#include "log.h"

pthread_mutex_t bplib_mutex = PTHREAD_MUTEX_INITIALIZER;

#define BPLIB_LOCK()	pthread_mutex_lock(&bplib_mutex);
#define BPLIB_UNLOCK()	pthread_mutex_unlock(&bplib_mutex);


UINT32 BulletProofHttp(DEV_DRIVER *dev, double data, UINT8 *proof, UINT8 *sub_url)
{
	UINT32 ret = 0;
	UINT8 url[128];
	http_respone post_return;
	data_package package;
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	memset(url, 0, sizeof(url));
	sprintf(url, "%s%s", dev->bp_cfg.domain, sub_url);

	memset(&package, 0, sizeof(package));
	PackageInit(&package);

	ret = JsonTempAdd((void *)dev, &package, data);
	if(ret == 0)
	{
		PackageFree(&package);
		return 1;
	}

	/*4.HTTP data post*/
	memset(&post_return, 0, sizeof(post_return));
	ret = http_post((char *)url, (char *)package.data, package.data_len, 
					(void *)&post_return, NULL);
	if(ret)
	{
		PackageFree(&package);
		return 1;
	}

	if((post_return.data == NULL) || (post_return.data_len == 0))
	{
		PackageFree(&package);
		return 1;
	}

	len = 0;
	tmp = JsonCodeGet((void *)dev, post_return.data, &len);
	if(tmp == NULL)
	{
		SAFE_FREE(post_return.data);
		PackageFree(&package);
		return 1;
	}
	memcpy(&ret, tmp, len);
	SAFE_FREE(tmp);
	if(ret != ERR_REQUEST_SUCCESS)
	{
		SAFE_FREE(post_return.data);
		PackageFree(&package);
		return 1;
	}

	len = 0;
	tmp = JsonProofGet((void *)dev, post_return.data, &len);
	if(tmp == NULL)
	{
		SAFE_FREE(post_return.data);
		PackageFree(&package);
		return 1;
	}
	sprintf(proof, "%s", tmp);
	SAFE_FREE(tmp);

	SAFE_FREE(post_return.data);
	PackageFree(&package);
	return 0;
}

#ifdef BULLETPROOF_LIB

UINT32 BulletProofLocal(DEV_DRIVER *dev, double data, UINT8 *proof, UINT32 ct_id)
{
	UINT32 ret = 0;
	UINT8 *tmp = NULL;
	
	if(dev == NULL || proof == NULL)
	{
		return 1;
	}

	tmp = malloc(PROOF_LENGTH);
	if(tmp == NULL)
	{
		return 1;
	}
	memset(tmp, 0, PROOF_LENGTH);

	if(dev->bp_cfg.ct_cfg == NULL)
	{
		LOG_PRINT(WX_LOG_ERROR, "BulletProof fail, reason: no proof config.\r\n");
		SAFE_FREE(tmp);
		return 1;
	}

	BPLIB_LOCK();
	ret = BulletProof(dev->bp_cfg.ct_cfg, data, ct_id, (char **)&tmp);
	BPLIB_UNLOCK();
	if(ret != 0)
	{
		SAFE_FREE(tmp);
		return 1;
	}

	sprintf(proof, "%s", tmp);
	SAFE_FREE(tmp);

	return 0;
}
#endif


/*****************************************************************************
 * @Function	: BulletProofGet
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 为指定的设备数据生成proof证明
 * @param[in]  	: void *handle  设备句柄
               	  double data   待生成证明的数据
               	  UINT8 *proof  用于存储proof的内存空间
               	  void *para    参数：proof本地计算，则是BP标准化信息；
               	  					  若是向服务端请求，则是请求的URL
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 BulletProofGet(void *handle, double data, UINT8 *proof, void *para)
{
	UINT32 ret = 0;
	DEV_DRIVER *dev = NULL;
	
	if(handle == NULL || proof == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;
	switch(dev->bp_cfg.type)
	{
		#ifdef BULLETPROOF_LIB
		case BP_LOCAL_LIB:
			ret = BulletProofLocal(dev, data, proof, *(UINT32 *)para);
			break;
		#endif
		case BP_LOCAL_SERVER:
		case BP_REMOTE_SERVER:
			ret = BulletProofHttp(dev, data, proof, (UINT8 *)para);
			break;
		default:
			ret = 1;
			break;
	}

	return ret;
}

/*****************************************************************************
 * @Function	: BulletProofOnchain
 * @author		: xqchen
 * @date		: 2021-4-30
 * @brief       : 将proof区块链存证，并返回存证凭证
 * @param[in]  	: void *handle   设备句柄
               	  UINT8 *pid     proof pid
               	  UINT8 *proof   proof信息
               	  UINT8 *hasher  区块链存证凭证
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 BulletProofOnchain(void *handle, UINT8 *pid, UINT8 *proof, UINT8 *hasher)
{
	UINT32 ret = 0;
	UINT8 url[128];
	http_respone post_return;
	data_package package;
	DEV_DRIVER *dev = NULL;
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	
	if(handle == NULL || proof == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;
	
	
	memset(url, 0, sizeof(url));
	sprintf(url, "%s%s", dev->chain_cfg.domain, VERIFYPROOF_URL);

	memset(&package, 0, sizeof(package));
	PackageInit(&package);
	ret = JsonPidAdd(handle, &package, pid);
	if(ret == 0)
	{
		PackageFree(&package);
		return 1;
	}

	ret = JsonProofAdd(handle, &package, proof);
	if(ret == 0)
	{
		PackageFree(&package);
		return 1;
	}


	/*4.HTTP data post*/
	memset(&post_return, 0, sizeof(post_return));
	ret = http_post((char *)url, (char *)package.data, package.data_len, 
					(void *)&post_return, dev->chain_cfg.token);
	if(ret)
	{
		PackageFree(&package);
		return 1;
	}

	if((post_return.data == NULL) || (post_return.data_len == 0))
	{
		PackageFree(&package);
		return 1;
	}

	len = 0;
	tmp = JsonRetCodeGet(handle, post_return.data, &len);
	if(tmp == NULL)
	{
		SAFE_FREE(post_return.data);
		PackageFree(&package);
		return 1;
	}
	memcpy(&ret, tmp, len);
	SAFE_FREE(tmp);
	if(ret != ERR_REQUEST_SUCCESS)
	{
		SAFE_FREE(post_return.data);
		PackageFree(&package);
		return 1;
	}

	len = 0;
	tmp = JsonRawDataGet(handle, post_return.data, &len);
	if(tmp == NULL)
	{
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


