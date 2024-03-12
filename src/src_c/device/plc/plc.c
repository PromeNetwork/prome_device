#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include "hexutils.h"
#include "log.h"
#include "dev_comm.h"
#include "plc_db.h"
#include "dev_json.h"
#include "dev_cfg.h"
#include "thread_api.h"


static void plc_cfg_clean(DEVICE_INFO *dev)
{
	UINT32 i = 0;
	UINT8 cmd[256];
	cJSON *root = NULL;
	cJSON *tmp = NULL;
	UINT8 cfg_file[FILE_NAME_MAX_LEN];

	if(dev == NULL)
	{
		return;
	}

	root = cJSON_Parse((char *)dev->addr_list);
	if(root == NULL)
	{
		return ;
	}

	for(i = 0; i < cJSON_GetArraySize(root); ++i)
	{
		tmp = cJSON_GetArrayItem(root, i);
		memset(cfg_file, 0, sizeof(cfg_file));
		sprintf(cfg_file, "%s/config/%s_%s_%d_%d.json", 
						   DEV_DATA_ROOT(DEV_TYPE_PLC), 
						   DEV_TYPE_STR(DEV_TYPE_PLC), 
						   dev->host, dev->port, tmp->valueint);
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "rm -rf %s", cfg_file); 
		SystemNew(cmd);
	}
	cJSON_Delete(root);
}


UINT32 PlcDataPkg(void *handle, void *data, data_package *pkg)
{
	UINT32 ret = 0;
	PLC_DATA *tmp = NULL;

	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}
	tmp = (PLC_DATA *)data;

	ret = JsonHostAdd(handle, pkg, tmp->host);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPortAdd(handle, pkg, tmp->port);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonSlaveIdAdd(handle, pkg, tmp->slave_id);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPlcDataAdd(handle, pkg, tmp->PlcData);
	if(ret == 0)
	{
		return 1;
	}
	
	return 0;
}

UINT32 PlcDataUnPkg(void *handle, UINT8 *pkg, void *data)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	PLC_DATA *tmp_data = NULL;
	
	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}

	tmp_data = (PLC_DATA *)data;

	tmp = JsonPlcDataGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy((UINT8 *)&(tmp_data->PlcData), tmp, len);
		SAFE_FREE(tmp);
	}

	return 0;
}

UINT32 PlcDataSave(void *handle, void *data, UINT8 *PoeHash, UINT32 len)
{
	//DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	
	//return PlcDataToDb(dev, (PLC_DATA *)data, PoeHash, len);
	return 0;
}


UINT32 PlcTaskStop(DEVICE_INFO *dev)
{
	thread_info *thread = NULL;
	thread_info *tmp = NULL;

	UINT8 port_str[32];
	UINT8 id_str[32];

	UINT8 poll_str[32];
	UINT8 sample_str[32];

	if(dev == NULL || dev->addr_list == NULL)
	{
		return 1;
	}

	thread = (thread_info *)malloc(sizeof(thread_info));
	if (thread == NULL)
	{
		return 1;
	}
	memset(thread, 0, sizeof(thread_info));

	
	sprintf(thread->task_name, "%s", "plc");
	thread->argc = 8;
	memset(port_str, 0, sizeof(port_str));
	sprintf(port_str, "%d", dev->port);
	memset(id_str, 0, sizeof(id_str));
	sprintf(id_str, "%d", dev->slave_id);
	memset(poll_str, 0, sizeof(poll_str));
	sprintf(poll_str, "%d", dev->poll_time);
	memset(sample_str, 0, sizeof(sample_str));
	sprintf(sample_str, "%d", dev->sample_interval);
	thread->argv = ThreadArgCreate(thread->argc, 
								   thread->task_name, 
								   dev->host,
								   port_str,
								   id_str,
								   poll_str,
								   sample_str,
								   dev->addr_list,
								   NULL);

	tmp = ThreadSearch(thread->argc, thread->argv);
	if(tmp == NULL)
	{
		LOG_PRINT(  Prome_LOG_INFO, "Running task %s not find ...", thread->task_name);
		ThreadArgFree(thread);
		SAFE_FREE(thread);
		plc_cfg_clean(dev);
		return 1;
	}

	LOG_PRINT(  Prome_LOG_INFO, "Running task %s find ...", thread->task_name);
	//PrintThreadInfo(tmp);
	ThreadStop(tmp);
	plc_cfg_clean(dev);
	SAFE_FREE(tmp);
	ThreadArgFree(thread);
	SAFE_FREE(thread);
	return 0;
	
}

UINT32 PlcTaskStart(DEVICE_INFO *dev)
{
	thread_info *thread = NULL;
	UINT8 port_str[32];
	UINT8 id_str[32];
	UINT8 poll_str[32];
	UINT8 sample_str[32];
	UINT32  ret = 0;

	if(dev == NULL || dev->addr_list == NULL)
	{
		return 1;
	}

	thread = (thread_info *)malloc(sizeof(thread_info));
	if (thread == NULL)
	{
		return 1;
	}
	
	memset(thread, 0, sizeof(thread_info));
	sprintf(thread->task_name, "%s", "plc");
	thread->argc = 8;
	memset(port_str, 0, sizeof(port_str));
	sprintf(port_str, "%d", dev->port);
	memset(id_str, 0, sizeof(id_str));
	sprintf(id_str, "%d", dev->slave_id);
	memset(poll_str, 0, sizeof(poll_str));
	sprintf(poll_str, "%d", dev->poll_time);
	memset(sample_str, 0, sizeof(sample_str));
	sprintf(sample_str, "%d", dev->sample_interval);
	thread->argv = ThreadArgCreate(thread->argc, 
								   thread->task_name, 
								   dev->host,
								   port_str,
								   id_str,
								   poll_str,
								   sample_str,
								   dev->addr_list,
								   NULL);
	ret = ThreadStart(thread);
	if(ret)
	{
		ThreadArgFree(thread);
		SAFE_FREE(thread);
	}

	return 0;
}

UINT32 PlcCfgInit(void *handle)
{
	UINT8 *filebuff = NULL;
	DEV_DRIVER *dev = NULL;
	INT32 ret = 0;

	if(handle == NULL)
	{
		return 0;
	}
	dev = (DEV_DRIVER *)handle;

	/*���������ļ�����������*/
	filebuff = ReadFileAll(dev->cfg_file);
	if(filebuff == NULL)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Read file %s fail, maybe not exist or empty!", dev->cfg_file);
		return 1;
	}

	ret = DeviceCfgIdParse(dev, filebuff, &dev->cfg_id);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	
	/*?a??MQTT????*/
	ret = DeviceMqttCfgInit(dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	/*?a???����?????*/
	ret = DeviceCfgChainParse(dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	SAFE_FREE(filebuff);

	LOG_PRINT(  Prome_LOG_INFO, "Device %s init config ok.", dev->device_id);
	return 0;
	
}

UINT32 PlcCfgRecvCb(void *handle, void *data, UINT32 data_len)
{
	UINT64 cfgid = 0;
	DEVICE_STATUS status = 0;
	INT32 ret = 0;
	INT32 cfg_result = 0;
	
	DEV_DRIVER *dev = NULL;
	
	if(handle == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	LOG_PRINT(  Prome_LOG_INFO, "Device %s receiver config.", dev->device_id);
	//LOG_PRINT(  Prome_LOG_INFO, "%s", data);
	LOG_PRINT(  Prome_LOG_INFO, "Device %s start parse config.", dev->device_id);
	
	ret = DeviceCfgIdParse(dev, data, &cfgid);
	if(ret)
	{
		/*?����|��t???��??????����*/
		status = STATUS_READ_CONFIG_FAIL;
		DevicePkgCfgRes(dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config fail.", dev->device_id);
		return 0;
	}
	
	if(cfgid == dev->cfg_id)
	{
		/*????ID?����?��??����??a?????����??TD��?��D?��?2?��?��?o?2������*/
		LOG_PRINT(  Prome_LOG_INFO, "The running config is same as the received.");
		/*?����|��t???��??????����*/
		DevicePkgCfgRes(dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
		return 0;
	}
	else
	{/*????ID2?��?��??��?a??????*/
		
		/*?a???����?????*/
		cfg_result = DeviceCfgChainParse(dev, data);
		if(cfg_result < 0)
		{
			/*?����|��t???��??????����*/
			status = STATUS_READ_CONFIG_FAIL;
			DevicePkgCfgRes(dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config fail.", dev->device_id);
			return 1;
		}
		else if(cfg_result == 0)
		{
			LOG_PRINT(  Prome_LOG_INFO, "The running config is same as the received.");
			/*?����|��t???��??????����*/
			DevicePkgCfgRes(dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
			return 0;
		}

		/*�����?????ID*/
		dev->cfg_id = cfgid;
		DeviceCfgIdSave(dev, dev->cfg_file);

		/*�����??����?????*/
		DeviceCfgChainSave(dev, dev->cfg_file);

		/*?����|��t???��??????����*/
		DevicePkgCfgRes(dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
	}

	return 0;
}

UINT32 PlcOnline(void *handle)
{
	DeviceOnline(handle);
	return 0;
}

UINT32 PlcCfgAlloc(void *handle)
{
	#if 0
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	
	dev->sub_dev_cfg = (void *)malloc(sizeof(GATEWAY_CONFIG));
	if(dev->sub_dev_cfg == NULL)
	{
		return 1;
	}
	memset(dev->sub_dev_cfg, 0, sizeof(GATEWAY_CONFIG));
	#endif
	return 0;
}

INT32 PlcMqttCfgRecvCb(void *handle, void *data, UINT32 data_len, UINT64 request_id)
{
	return DeviceMqttCfgRecvCb(handle, data, data_len, request_id);
}

#if 0
INT32 PlcMqttCfgRecvCb(	 void *handle, 
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

	ret = DeviceMqttCfgParse(dev, data, 1);
	if(ret > 0)
	{/*MQTT????��D??��?*/
		DeviceMqttCfgSave((void *)dev, dev->cfg_file);
		need_restart = 1;
	}
	else if(ret == 0)
	{/*MQTT?????T��??��*/
		LOG_PRINT(  Prome_LOG_INFO, "The running mqtt config is same as the received.");
		need_restart = 0;
	}
	else
	{/*MQTT?????a??3?�䨪*/
		dev->dev_status = STATUS_READ_CONFIG_FAIL;
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s parse mqtt config fail.", dev->device_id);
		need_restart = 0;
	}

	return need_restart;
}
#endif
void PlcTaskClean()
{
	UINT8 cmd[256];
	
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "killall %s", "plc"); 
	SystemNew(cmd);
}

