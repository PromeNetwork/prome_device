#include <string.h>
#include <stdlib.h>

#include "basic_types.h"
#include "cJSON.h"
#include "hexutils.h"
#include "dev_json.h"
#include "dev_rule.h"
#include "dev_comm.h"
#include "timer.h"
#include "bulletproof.h"
#include "log.h"

UINT8 *RULE_STATUS_STR(RULE_STATUS status)
{
	UINT8 *tmp = NULL;

	switch(status)
	{
		case RULE_SUCCESS:
			tmp = (UINT8 *)"Success";
			break;
		case RULE_AVE_GET_FAIL:
			tmp = (UINT8 *)"ave get fail";
			break;
		case RULE_AVE_ONCHAIN_FAIL:
			tmp = (UINT8 *)"ave onchain fail";
			break;
		case RULE_PROOF_GET_FAIL:
			tmp = (UINT8 *)"proof get fail";
			break;
		case RULE_PROOF_ONCHAIN_FAIL:
			tmp = (UINT8 *)"proof onchain fail";
			break;
		case RULE_DEV_NOT_FOUND:
			tmp = (UINT8 *)"device not found";
			break;
		case RULE_ALG_NOT_FOUND:
			tmp = (UINT8 *)"alg not found";
			break;
		default:
			tmp = (UINT8 *)"unknown error";
			break;
	}
	return tmp;
}


UINT8 *ALG_TYPE_STR(ALG_TYPE_E type)
{
	UINT8 *tmp = NULL;

	switch(type)
	{
		case ALG_TYPE_AVERAGE:
			tmp = (UINT8 *)ALG_TYPE_AVERAGE_STR;
			break;
		case ALG_TYPE_BULLET_PROOF:
			tmp = (UINT8 *)ALG_TYPE_BULLET_PROOF_STR;
			break;
		default:
			tmp = (UINT8 *)ALG_TYPE_DEFAULT_STR;
			break;
	}
	return tmp;
}

ALG_TYPE_E ALG_TYPE_INT(UINT8 *type)
{
	if(strstr(type, ALG_TYPE_AVERAGE_STR))
	{
		return ALG_TYPE_AVERAGE;
	}
	else if(strstr(type, ALG_TYPE_BULLET_PROOF_STR))
	{
		return ALG_TYPE_BULLET_PROOF;
	}
	else
	{
		return ALG_TYPE_DEFAULT;
	}
}

void *DevRuleAlgGet(RULE_INFO *rule, ALG_TYPE_E type)
{
	ALG_INFO *ptemp = NULL;
	ALG_INFO *head = NULL;

	head = rule->alg_list;
	if (head == NULL)
	{
		return NULL;
	}

	for(ptemp=head; ptemp!=NULL; ptemp=ptemp->next)
	{
		if(ptemp->type == type)
		{
			return ptemp->alg;
		}
	}

	return NULL;
}

/*****************************************************************************
 * @Function	: DevBulletProof

 * @date		: 2021-4-30
 * @brief       : 将制定的设备数据生成proof证明
 * @param[in]  	: void *handle    设备句柄
               	  ALG_PROOF *alg  proof算法结构体
               	  double value    待生成proof证明的值
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DevBulletProof(void *handle, ALG_PROOF *alg, double value)
{
	DEV_DRIVER *dev = NULL;
	void *para = NULL;
	UINT32 ret = 0;
	
	dev = (DEV_DRIVER *)handle;
	if(dev == NULL)
	{
		return 1;
	}

	switch(dev->bp_cfg.type)
	{
		case BP_LOCAL_LIB:
			para = (void *)&(alg->ct_index);
			break;
		case BP_LOCAL_SERVER:
		case BP_REMOTE_SERVER:
			para = (void *)BULLETPROOF_HUMIDITY_URL;
			break;
		default:
			para = NULL;
			break;
	}

	if(para == NULL)
	{
		return 1;
	}
	ret = BulletProofGet(dev, value, alg->proof, para);
	if(ret)
	{
		return 1;
	}
	printf("proof:%s\r\n", alg->proof);
	alg->proof_len = strlen(alg->proof);

	return 0;
}

/*****************************************************************************
 * @Function	: DevAveOnchain

 * @date		: 2021-4-30
 * @brief       : 将设备数据平均值区块链存证
 * @param[in]  	: void *handle  设备句柄
               	  ALG_AVE *alg  平均值算法指针
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DevAveOnchain(void *handle, ALG_AVE *alg)
{
	UINT32 ret = 0;
	UINT8 PoeHash[HASH_STR_MAX_LEN];
	UINT8 ave_str[16];
	DEV_DRIVER *dev = NULL;

	dev = (DEV_DRIVER *)handle;
	if(dev == NULL)
	{
		return 1;
	}

	/*4.将整个数据包，哈希存证*/
	memset(PoeHash, 0, sizeof(PoeHash));
	memset(ave_str, 0, sizeof(ave_str));
	sprintf(ave_str, "%.2f", alg->value);

	if(dev->data_onchain == NULL) return 1;
	ret = dev->data_onchain(dev, (UINT8 *)ave_str, strlen(ave_str), PoeHash);
	if(ret)
	{
		return 1;
	}

	sprintf(alg->hasher, "%s", PoeHash);
	alg->hasher_len = strlen(PoeHash);
	return 0;
}

/*****************************************************************************
 * @Function	: dev_rul_exec

 * @date		: 2021-4-30
 * @brief       : 设备rule执行处理函数
 * @param[in]  	: void *ptr  设备句柄参数
               	  int len    参数长度
 * @param[out]  : None
 * @return  	: static
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static void dev_rul_exec(void *ptr, int len)
{
	DEV_DRIVER *dev = NULL;
	UINT32 ret = 0;

	dev = (DEV_DRIVER *)ptr;
	if(dev == NULL)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Exec rule fail, Reason:dev handle is NULL.");
		return;
	}

	if(dev->dev_rule_func == NULL)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s exec rule fail, Reason:exec function is NULL.",
				  dev->device_id);
		return;
	}

	ret = dev->dev_rule_func(dev);
	if(ret)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s exec rule fail, Reason:%s.",
				  dev->device_id, RULE_STATUS_STR(ret));
	}
	else
	{
		LOG_PRINT(  Prome_LOG_INFO, "Device %s rule execution ok.", dev->device_id);
	}

	ret = DevicePkgRuleResult(ptr, ret);
	if(ret)
	{
		LOG_PRINT(  Prome_LOG_ERROR,
				  "Device %s send rule execution reslut to platform fail.", 
				  dev->device_id);
		return;
	}

	LOG_PRINT(  Prome_LOG_INFO,
			  "Device %s send rule execution reslut to platform ok.", 
			  dev->device_id);
}

/*****************************************************************************
 * @Function	: DevRuleStart

 * @date		: 2021-4-30
 * @brief       : 启动指定设备的rule运行，目前一台设备只能运行一条rule
 * @param[in]  	: void *handle  设备句柄
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DevRuleStart(void *handle)
{
	UINT8 *tmp = NULL;
	RULE_INFO *rule = NULL;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	rule = &(dev->rule_cfg);
	if(rule->alg_list == NULL)
	{
		return 1;
	}

	if(rule->timer > 0)
	{
		rule->timer = TimerUpdate(rule->timer, 
 								  rule->interval, 
 								  dev_rul_exec, handle, 
 								  sizeof(DEV_DRIVER), 
 								  TIMER_ALWAYS);
	}
	else
	{
		rule->timer = TimerAdd(rule->interval, 
							   dev_rul_exec, handle, 
							   sizeof(DEV_DRIVER), 
							   TIMER_ALWAYS);
	}

	return 0;
}

/*****************************************************************************
 * @Function	: DevRuleStop

 * @date		: 2021-4-30
 * @brief       : 停止指定设备的rule运行
 * @param[in]  	: void *handle  设备句柄
 * @param[out]  : None
 * @return  	: UINT32
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DevRuleStop(void *handle)
{
	RULE_INFO *rule = NULL;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL)
	{
		return 1;
	}

	rule = &(dev->rule_cfg);
	if(rule->alg_list == NULL)
	{
		return 1;
	}

	if(rule->timer > 0)
	{
		TimerDelete(rule->timer);
		rule->timer = 0;
	}
	return 0;
}


