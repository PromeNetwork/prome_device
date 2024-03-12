#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include "hexutils.h"
#include "log.h"
#include "dev_comm.h"

#include "humsensor.h"
#include "humsensor_db.h"
#include "dev_json.h"
#include "package.h"
#include "dev_rule.h"


//static int humsensor_init_flag = 0;

typedef struct _HUMSENSOR_AVE
{
	double 	Hum;
	UINT32 	count;
}HUMSENSOR_AVE ,*P_HUMSENSOR_AVE; 


static void humsensor_cfg_clean(DEVICE_INFO *dev)
{
	UINT8 cmd[256];
	UINT8 cfg_file[FILE_NAME_MAX_LEN];

	if(dev == NULL)
	{
		return;
	}

	memset(cfg_file, 0, sizeof(cfg_file));
	sprintf(cfg_file, "%s/config/%s_%s%s.json", 
					   DEV_DATA_ROOT(DEV_TYPE_HUMSENSOR), 
					   DEV_TYPE_STR(DEV_TYPE_HUMSENSOR), 
					   DEV_TYPE_STR(DEV_TYPE_HUMSENSOR), 
					   dev->host);
	printf("-----------cfg_file:%s\r\n", cfg_file);
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "rm -rf %s", cfg_file); 
	SystemNew(cmd);
}


static int hum_ave_rule_cb(void *para, int argc, char **argv, char **azColName)
{
	HUMSENSOR_AVE *ave = (HUMSENSOR_AVE *)para;

	ave->Hum = ave->Hum + atof(argv[HUMSENSOR_COL_INDEX_HUM]);
	ave->count = ave->count + 1;
	return 0;
}

static UINT32 hum_ave_rule_exec(DEV_DRIVER *dev, ALG_AVE *alg)
{
	TIMESTAMP start = GetTimestampInt();
	double result = 0;
	HUMSENSOR_AVE *ave = NULL;

	ave = (HUMSENSOR_AVE *)malloc(sizeof(HUMSENSOR_AVE));
	if(ave == NULL)
	{
		return 1;
	}
	memset(ave, 0, sizeof(HUMSENSOR_AVE));

	//printf("---------------alg->interval:%d---------------\r\n", alg->interval);
	HumsensorTableSearchByTime(dev, (start-(alg->interval * 1000)), 
							  start, hum_ave_rule_cb, ave);

	if(ave->count)
	{
		result = (ave->Hum)/(ave->count);
		result = ((double)((int)((result+0.005)*100)))/100;
	}
	if(result == 0)
	{
		SAFE_FREE(ave);
		return 1;
	}

	//printf("---------------count:%d---------------\r\n", ave->count);
	alg->value = result;
	alg->stamp = start;
	alg->samples = ave->count;
	//alg->interval = HUMSENSOR_SAMPLE_INT;

	SAFE_FREE(ave);
	return 0;
}


UINT32 HumSensorRuleExec(void *handle)
{
	ALG_AVE *alg_ave = NULL;
	ALG_PROOF *alg_proof = NULL;
	RULE_INFO *rule = NULL;
	UINT8 hasher[HASH_STR_MAX_LEN];
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(dev == NULL)
	{
		return RULE_DEV_NOT_FOUND;
	}

	rule = &dev->rule_cfg;
	alg_ave = (ALG_AVE *)DevRuleAlgGet(rule, ALG_TYPE_AVERAGE);
	if(alg_ave == NULL)
	{
		return RULE_ALG_NOT_FOUND;
	}

	/*1.ִ��ƽ��ֵ�㷨*/
	ret = hum_ave_rule_exec(dev, alg_ave);
	if(ret)
	{
		return RULE_AVE_GET_FAIL;
	}
	/*2.ƽ��ֵ��֤*/
	ret = DevAveOnchain(dev, alg_ave);
	if(ret)
	{
		return RULE_AVE_ONCHAIN_FAIL;
	}

	/*3.ִ��bullet proof�㷨*/
	alg_proof = (ALG_PROOF *)DevRuleAlgGet(rule, ALG_TYPE_BULLET_PROOF);
	if(alg_proof == NULL)
	{
		return RULE_ALG_NOT_FOUND;
	}
	ret = DevBulletProof((void *)dev, alg_proof, alg_ave->value);
	if(ret)
	{
		return RULE_PROOF_GET_FAIL;
	}
	/*4.proof��BPID������BPID��ƽ��ֵave��֤PoEHash*/
	memcpy(alg_proof->pid, alg_ave->hasher, alg_ave->hasher_len);
	alg_proof->pid_len = alg_ave->hasher_len;

	memset(hasher, 0, sizeof(hasher));
	ret = BulletProofOnchain((void *)dev, alg_proof->pid, 
							  alg_proof->proof, hasher);
	if(ret)
	{
		return RULE_PROOF_ONCHAIN_FAIL;
	}
	return RULE_SUCCESS;
}

UINT32 HumSensorDataPkg(void *handle, void *data, data_package *pkg)
{
	UINT32 ret = 0;
	HUMSENSOR_DATA *tmp = NULL;

	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}

	tmp = (HUMSENSOR_DATA *)data;

	//printf("NodeId:%d\r\n", tmp->NodeId);
	ret = JsonNodeIdAdd(handle, pkg, tmp->NodeId);
	if(ret == 0)
	{
		return 1;
	}

	//printf("Hum:%f\r\n", tmp->Hum);
	ret = JsonHumAdd(handle, pkg, tmp->Hum);
	if(ret == 0)
	{
		return 1;
	}

	return 0;
}

UINT32 HumSensorDataUnPkg(void *handle, UINT8 *pkg, void *data)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	HUMSENSOR_DATA *tmp_data = NULL;

	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}
	tmp_data = (HUMSENSOR_DATA *)data;
	
	tmp = JsonNodeIdGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy((UINT8 *)&(tmp_data->NodeId), tmp, len);
		SAFE_FREE(tmp);
	}

	tmp = JsonHumGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy((UINT8 *)&(tmp_data->Hum), tmp, len);
		SAFE_FREE(tmp);
	}

	return 0;
}

UINT32 HumSensorDataSave(void *handle, void *data)
{
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	return HumsensorDataToDb(dev, (HUMSENSOR_DATA *)data);
}

int HumsensorOnlineJava(UINT8 *deviceid)
{
	DEV_DRIVER *dev = NULL;

	UINT8 device_id[DEV_ID_MAX_LEN];
	memset(device_id, 0, sizeof(device_id));
	sprintf(device_id, "%s%s", DEV_DESC_HUMSENSOR, deviceid);

	dev = DeviceHandleRegister(device_id, DEV_TYPE_HUMSENSOR);
	if(dev == NULL)
	{
		return 1;
	}

	return 0;
}

int HumsensorDataSendJava(UINT8 *deviceid, void *data)
{
	DEV_DRIVER *dev = NULL;
	data_package pkg;
	UINT32 ret = 0;
	UINT8 device_id[DEV_ID_MAX_LEN];
	memset(device_id, 0, sizeof(device_id));
	sprintf(device_id, "%s%s", DEV_DESC_HUMSENSOR, deviceid);

	if(ProcessStatusGet() == THREAD_STATUS_STOP)
	{
		DeviceHandleUnRegister(device_id, DEV_TYPE_HUMSENSOR);
		DeviceBasicInfoDeInit();
		exit(0);
	}

	dev = DeviceHandleGet(device_id, DEV_TYPE_HUMSENSOR);
	if(dev == NULL)
	{
		dev = DeviceHandleRegister(device_id, DEV_TYPE_HUMSENSOR);
		if(dev == NULL)
		{
			return 1;
		}
		return 1;
	}

	memset(&pkg, 0, sizeof(pkg));
	PackageInit(&pkg);

	if(dev->data_pkg == NULL)
	{
		PackageFree(&pkg);
		return 1;
	}
	ret = dev->data_pkg(dev, data, &pkg);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	if(dev->data_report == NULL)	
	{
		PackageFree(&pkg);
		return 1;
	}
	ret = dev->data_report(dev, data);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	ret = HumSensorDataSave((void *)dev, data);
	if(ret)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s save data to database fail.",
  		  		  dev->device_id);
	}
	else
	{
		LOG_PRINT(  Prome_LOG_INFO, "Device %s save data to database ok.",
		  		  dev->device_id);
	}

	PackageFree(&pkg);
	return 0;
}	

UINT32 HumsensorTaskStop(DEVICE_INFO *dev)
{
	thread_info *thread = NULL;
	thread_info *tmp = NULL;
	UINT8 port_str[32];

	thread = (thread_info *)malloc(sizeof(thread_info));
	if (thread == NULL)
	{
		return 1;
	}
	memset(thread, 0, sizeof(thread_info));


	sprintf(thread->task_name, "%s", "java");
	memset(port_str, 0, sizeof(port_str));
	sprintf(port_str, "%d", dev->port);
	thread->argc = 5;
	thread->argv = ThreadArgCreate(	thread->argc, 
									thread->task_name,
									"Htsensor", 
									dev->host, 
									port_str, 
									NULL);

	tmp = ThreadSearch(thread->argc, thread->argv);
	if(tmp == NULL)
	{
		LOG_PRINT(  Prome_LOG_INFO, "Running task %s not find ...", thread->task_name);
		ThreadArgFree(thread);
		SAFE_FREE(thread);
		humsensor_cfg_clean(dev);
		return 1;
	}

	LOG_PRINT(  Prome_LOG_INFO, "Running task %s find ...", thread->task_name);
	//PrintThreadInfo(tmp);
	ThreadStop(tmp);
	humsensor_cfg_clean(dev);
	SAFE_FREE(tmp);
	ThreadArgFree(thread);
	SAFE_FREE(thread);
	return 0;

}

UINT32 HumsensorTaskStart(DEVICE_INFO *dev)
{
	thread_info *thread = NULL;
	UINT8 port_str[32];
	UINT32  ret = 0;

	thread = (thread_info *)malloc(sizeof(thread_info));
	if (thread == NULL)
	{
		return 1;
	}

	memset(thread, 0, sizeof(thread_info));
	sprintf(thread->task_name, "%s", "java");
	memset(port_str, 0, sizeof(port_str));
	sprintf(port_str, "%d", dev->port);
	thread->argc = 5;
	thread->argv = ThreadArgCreate(thread->argc, 
								   thread->task_name,
								   "Htsensor", 
								   dev->host, 
								   port_str, 
								   NULL);
	ret = ThreadStart(thread);
	if(ret)
	{
		ThreadArgFree(thread);
		SAFE_FREE(thread);
	}

	return 0;
}

UINT32 HumsensorCfgInit(void *handle)
{
	UINT8 *filebuff = NULL;
	DEV_DRIVER *dev = NULL;
	INT32 ret = 0;

	if(handle == NULL)
	{
		return 0;
	}
	dev = (DEV_DRIVER *)handle;
	
	filebuff = ReadFileAll(dev->cfg_file);
	if(filebuff == NULL)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Read file %s fail, maybe not exist or empty!", dev->cfg_file);
		return 1;
	}
	ret = DeviceCfgIdParse((void *)dev, filebuff, &dev->cfg_id);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	
	ret = DeviceMqttCfgInit((void *)dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init mqtt config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}
	
	ret = DeviceCfgChainParse((void *)dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init chain config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	ret = DeviceCfgRuleParse((void *)dev, filebuff, &(dev->rule_cfg));
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init rule config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}
	ret = DeviceCfgBpParse((void *)dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init bp config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	SAFE_FREE(filebuff);

	LOG_PRINT(  Prome_LOG_INFO, "Device %s init config ok.", dev->device_id);
	return 0;
	
}

UINT32 HumsensorCfgRecvCb(void *handle, void *data, UINT32 data_len)
{
	UINT64 cfgid = 0;
	DEVICE_STATUS status = 0;
	INT32 ret = 0;
	INT32 cfg_result = 0;
	RULE_INFO rule_new;
	
	DEV_DRIVER *dev = NULL;
	
	if(handle == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	LOG_PRINT(  Prome_LOG_INFO, "Device %s receiver config.", dev->device_id);
	//LOG_PRINT(  Prome_LOG_INFO, "%s", data);
	LOG_PRINT(  Prome_LOG_INFO, "Device %s start parse config.", dev->device_id);
	
	ret = DeviceCfgIdParse((void *)dev, data, &cfgid);
	if(ret)
	{
		status = STATUS_READ_CONFIG_FAIL;
		DevicePkgCfgRes((void *)dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config fail.", dev->device_id);
		return 0;
	}
	
	if(cfgid == dev->cfg_id)
	{
		LOG_PRINT(  Prome_LOG_INFO, "The running config is same as the received.");
		DevicePkgCfgRes((void *)dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
		return 0;
	}
	else
	{
		
		ret = DeviceCfgChainParse((void *)dev, data);
		if(ret < 0)
		{
			status = STATUS_READ_CONFIG_FAIL;
			DevicePkgCfgRes((void *)dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_ERROR, "Device %s parse chain config fail.", dev->device_id);
			return 1;
		}
		else if(ret > 0)
		{
			cfg_result = 1;
		}

		memset(&rule_new, 0, sizeof(RULE_INFO));
		ret = DeviceCfgRuleParse((void *)dev, data, &rule_new);
		if(ret < 0)
		{
			status = STATUS_READ_CONFIG_FAIL;
			DevicePkgCfgRes((void *)dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_ERROR, "Device %s parse rule config fail.", dev->device_id);
			return 1;
		}
		else if(ret > 0)
		{
			cfg_result = 1;
			DevRuleStop(handle);
			DeviceCfgRuleDelete(handle);
			memcpy(&(dev->rule_cfg), &rule_new, sizeof(RULE_INFO));
			DevRuleStart(handle);
		}

		ret = DeviceCfgBpParse((void *)dev, data);
		if(ret < 0)
		{
			status = STATUS_READ_CONFIG_FAIL;
			DevicePkgCfgRes((void *)dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_ERROR, "Device %s parse bp config fail.", dev->device_id);
			return 1;
		}
		else if(ret > 0)
		{
			cfg_result = 1;
		}
		

		if(cfg_result == 0)
		{
			LOG_PRINT(  Prome_LOG_INFO, "The running config is same as the received.");
			DevicePkgCfgRes(dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
			return 0;
		}
		
		dev->cfg_id = cfgid;
		
		DeviceCfgIdSave((void *)dev, dev->cfg_file);

		DeviceCfgChainSave((void *)dev, dev->cfg_file);

		DeviceCfgRuleSave((void *)dev, dev->cfg_file);

		DeviceCfgBpSave((void *)dev, dev->cfg_file);

		DevicePkgCfgRes((void *)dev, data, data_len, status);
		
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
	}

	return 0;
}

UINT32 HumsensorOnline(void *handle)
{
	DevRuleStart(handle);
	DeviceOnline(handle);
	return 0;
}

INT32 HumsensorMqttCfgRecvCb(void *handle, 
							 void *data, 
							 UINT32 data_len, 
							 UINT64 request_id)
{
	return DeviceMqttCfgRecvCb(handle, data, data_len, request_id);
}

#if 0
UINT32 HumsensorMqttCfgRecvCb(void *handle, 
							 void *data, 
							 UINT32 data_len, 
							 TIMESTAMP request_id)
{
	DEV_DRIVER *dev = NULL;
	INT32 ret = 0;
	INT32 need_restart = 0;

	dev = (DEV_DRIVER *)handle;

	LOG_PRINT(  Prome_LOG_INFO, "Device %s receiver mqtt config.", dev->device_id);
	//LOG_PRINT(  Prome_LOG_INFO, "%s", data);
	LOG_PRINT(  Prome_LOG_INFO, "Device %s start parse mqtt config.", dev->device_id);

	ret = DeviceMqttCfgParse((void *)dev, data, 1);
	if(ret > 0)
	{
		DeviceMqttCfgSave((void *)dev, dev->cfg_file);
		need_restart = 1;
	}
	else if(ret == 0)
	{
		LOG_PRINT(  Prome_LOG_INFO, "The running mqtt config is same as the received.");
		need_restart = 0;
	}
	else
	{
		dev->dev_status = STATUS_READ_CONFIG_FAIL;
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s parse mqtt config fail.", dev->device_id);
		need_restart = 0;
	}

	ret = DevicePkgRpcResp(handle, data, data_len, request_id, 
						   PKG_TYPE_DEV_RPC_MQTT_RESP);
	if(ret)
	{
		
	}
	if(need_restart)
	{
		sleep(1);
		exit(0);
	}
	

	return 0;
}
#endif
void HumsensorTaskClean()
{
	#if 0
	UINT8 cmd[256];
	
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "killall %s", "ipcamera"); 
	SystemNew(cmd);
	#endif
}



