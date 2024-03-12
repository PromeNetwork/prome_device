#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include "hexutils.h"
#include "log.h"
#include "dev_comm.h"
#include "rfid_db.h"
#include "dev_json.h"

static void rfid_cfg_clean(DEVICE_INFO *dev)
{
	UINT8 cmd[256];
	UINT8 cfg_file[FILE_NAME_MAX_LEN];

	if(dev == NULL)
	{
		return;
	}

	memset(cfg_file, 0, sizeof(cfg_file));
	sprintf(cfg_file, "%s/config/%s_%s_%d.json", 
					   DEV_DATA_ROOT(DEV_TYPE_RFID), 
					   DEV_TYPE_STR(DEV_TYPE_RFID), 
					   dev->host, dev->port);
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "rm -rf %s", cfg_file); 
	SystemNew(cmd);
}


UINT32 RfidDataPkg(void *handle, void *data, data_package *pkg)
{
	UINT32 ret = 0;
	RFID_DATA *tmp = NULL;

	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}

	tmp = (RFID_DATA *)data;

	ret = JsonRfidAdd(handle, pkg, tmp->Rfid);
	if(ret == 0)
	{
		return -1;
	}

	ret = JsonAtidAdd(handle, pkg, tmp->AntennaID);
	if(ret == 0)
	{
		return -1;
	}

	ret = JsonRssiAdd(handle, pkg, tmp->RSSI);
	if(ret == 0)
	{
		return -1;
	}
	
	return 0;
}

UINT32 RfidDataUnPkg(void *handle, UINT8 *pkg, void *data)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	RFID_DATA *tmp_data = NULL;

	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}
	tmp_data = (RFID_DATA *)data;
	
	tmp = JsonRfidGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy(tmp_data->Rfid, tmp, len);
		tmp_data->RfidLen = len;
		SAFE_FREE(tmp);
	}

	tmp = JsonAtidGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy((UINT8 *)&(tmp_data->AntennaID), tmp, len);
		SAFE_FREE(tmp);
	}

	tmp = JsonRssiGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy((UINT8 *)&(tmp_data->RSSI), tmp, len);
		SAFE_FREE(tmp);
	}
	
	return 0;
}

UINT32 RfidDataSave(void *handle, void *data, UINT8 *PoeHash, UINT32 len)
{
	//DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	
	//return RfidDataToDb(dev, (RFID_DATA *)data, PoeHash, len);
	return 0;
}

UINT32 RfidOnlineJava(UINT8 *deviceid)
{
	DEV_DRIVER *dev = NULL;

	dev = DeviceHandleRegister(deviceid, DEV_TYPE_RFID);
	if(dev == NULL)
	{
		return 1;
	}

	return 0;
}

UINT32 RfidDataSendJava(UINT8 *deviceid, void *data)
{
	DEV_DRIVER *dev = NULL;
	data_package pkg;
	UINT32 ret = 0;

	if(ProcessStatusGet() == THREAD_STATUS_STOP)
	{
		DeviceHandleUnRegister(deviceid, DEV_TYPE_RFID);
		DeviceBasicInfoDeInit();
		exit(0);
	}

	dev = DeviceHandleGet(deviceid, DEV_TYPE_RFID);
	if(dev == NULL)
	{
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

	ret = dev->data_report(dev, data);
	if(ret)
	{
		PackageFree(&pkg);
		return 1;
	}

	PackageFree(&pkg);
	return 0;
}  

UINT32 RfidTaskStop(DEVICE_INFO *dev)
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
									"Rfid", 
									dev->host, 
									port_str, 
									NULL);

	tmp = ThreadSearch(thread->argc, thread->argv);
	if(tmp == NULL)
	{
		LOG_PRINT(  Prome_LOG_INFO, "Running task %s not find ...", thread->task_name);
		ThreadArgFree(thread);
		SAFE_FREE(thread);
		rfid_cfg_clean(dev);
		return 1;
	}

	LOG_PRINT(  Prome_LOG_INFO, "Running task %s find ...", thread->task_name);
	//PrintThreadInfo(tmp);
	ThreadStop(tmp);
	rfid_cfg_clean(dev);
	SAFE_FREE(tmp);
	ThreadArgFree(thread);
	SAFE_FREE(thread);
	return 0;

}

UINT32 RfidTaskStart(DEVICE_INFO *dev)
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
	thread->argv = ThreadArgCreate(	thread->argc, 
									thread->task_name,
									"Rfid", 
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

UINT32 RfidCfgInit(void *handle)
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

	ret = DeviceCfgIdParse((void *)dev, filebuff, &dev->cfg_id);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	/*����MQTT����*/
	ret = DeviceMqttCfgInit((void *)dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	/*������������*/
	ret = DeviceCfgChainParse((void *)dev, filebuff);
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

UINT32 RfidCfgRecvCb(void *handle, void *data, UINT32 data_len)
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
	
	ret = DeviceCfgIdParse((void *)dev, data, &cfgid);
	if(ret)
	{
		/*��Ӧ�����������·�*/
		status = STATUS_READ_CONFIG_FAIL;
		DevicePkgCfgRes((void *)dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config fail.", dev->device_id);
		return 0;
	}
	
	if(cfgid == dev->cfg_id)
	{
		/*����ID��ͬ������Ϊ������ͬ������£������κβ���*/
		LOG_PRINT(  Prome_LOG_INFO, "The running config is same as the received.");
		/*��Ӧ�����������·�*/
		DevicePkgCfgRes((void *)dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
		return 0;
	}
	else
	{/*����ID��ͬ�����������*/
		
		/*������������*/
		ret = DeviceCfgChainParse((void *)dev, data);
		if(ret < 0)
		{
			/*��Ӧ�����������·�*/
			status = STATUS_READ_CONFIG_FAIL;
			DevicePkgCfgRes((void *)dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config fail.", dev->device_id);
			return 1;
		}
		else if(ret > 0)
		{
			cfg_result = 1;
		}

		if(cfg_result == 0)
		{
			LOG_PRINT(  Prome_LOG_INFO, "The running config is same as the received.");
			/*��Ӧ�����������·�*/
			DevicePkgCfgRes(dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
			return 0;
		}


		/*��������ID*/
		dev->cfg_id = cfgid;
		DeviceCfgIdSave((void *)dev, dev->cfg_file);

		/*�����������*/
		DeviceCfgChainSave((void *)dev, dev->cfg_file);

		/*��Ӧ�����������·�*/
		DevicePkgCfgRes((void *)dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
	}

	return 0;
}

UINT32 RfidOnline(void *handle)
{
	DeviceOnline(handle);
	return 0;
}

INT32 RfidMqttCfgRecvCb(void *handle, void *data, UINT32 data_len, UINT64 request_id)
{
	return DeviceMqttCfgRecvCb(handle, data, data_len, request_id);
}


#if 0
INT32 RfidMqttCfgRecvCb(void *handle, 
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
	{/*MQTT�����иı�*/
		DeviceMqttCfgSave((void *)dev, dev->cfg_file);
		need_restart = 1;
	}
	else if(ret == 0)
	{/*MQTT�����ޱ仯*/
		LOG_PRINT(  Prome_LOG_INFO, "The running mqtt config is same as the received.");
		need_restart = 0;
	}
	else
	{/*MQTT���ý�������*/
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
void RfidTaskClean()
{
	#if 0
	UINT8 cmd[256];
	
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "killall %s", "ipcamera"); 
	SystemNew(cmd);
	#endif
}


