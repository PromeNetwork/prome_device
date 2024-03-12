#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include "hexutils.h"
#include "log.h"
#include "dev_comm.h"
#include "ipcamera_db.h"
#include "dev_json.h"

#include "http_client.h"

static void ipcamera_cfg_clean(DEVICE_INFO *dev)
{
	UINT8 cmd[256];
	UINT8 cfg_file[FILE_NAME_MAX_LEN];

	if(dev == NULL)
	{
		return;
	}

	memset(cfg_file, 0, sizeof(cfg_file));
	sprintf(cfg_file, "%s/config/%s_%s_%d_%d.json", 
					   DEV_DATA_ROOT(DEV_TYPE_IPCAMERA), 
					   DEV_TYPE_STR(DEV_TYPE_IPCAMERA), 
					   dev->host, dev->port, dev->slave_id);
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "rm -rf %s", cfg_file); 
	SystemNew(cmd);
}

static UINT32 ipcamera_cfg_alloc(void *handle)
{
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	
	dev->sub_dev_cfg = (void *)malloc(sizeof(FILE_SERVER_CFG));
	if(dev->sub_dev_cfg == NULL)
	{
		return 1;
	}
	memset(dev->sub_dev_cfg, 0, sizeof(FILE_SERVER_CFG));
	return 0;
}


UINT32 IpcameraDataPkg(void *handle, void *data, data_package *pkg)
{
	UINT32 ret = 0;
	IPCAMERA_DATA *tmp = NULL;

	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}
	tmp = (IPCAMERA_DATA *)data;

	if(strlen(tmp->Pic))
	{
		ret = JsonPicAdd(handle, pkg, tmp->Pic);
		if(ret == 0)
		{
			return -1;
		}
		
		ret = JsonPicIdAdd(handle, pkg, tmp->PicId);
		if(ret == 0)
		{
			return -1;
		}

		ret = JsonPicHashAdd(handle, pkg, tmp->PicHash);
		if(ret == 0)
		{
			return -1;
		}
	}
	
	return 0;
}

UINT32 IpcameraDataUnPkg(void *handle, UINT8 *pkg, void *data)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;
	IPCAMERA_DATA *tmp_data = NULL;
	
	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}

	tmp_data = (IPCAMERA_DATA *)data;

	tmp = JsonPicGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy(tmp_data->Pic, tmp, len);
		SAFE_FREE(tmp);
	}

	tmp = JsonPicIdGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy((UINT8 *)&(tmp_data->PicId), tmp, len);
		SAFE_FREE(tmp);
	}

	tmp = JsonPicHashGet(handle, pkg, &len);
	if(tmp)
	{
		memcpy(tmp_data->PicHash, tmp, len);
		tmp_data->PicHashLen = len;
		SAFE_FREE(tmp);
	}
	
	return 0;
}

UINT32 IpcameraDataSave(void *handle, void *data, 
								 UINT8 *PoeHash, UINT32 len)
{
	//DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	
	//return IpcameraDataToDb(dev, (IPCAMERA_DATA *)data, PoeHash, len);
	return 0;
}

static UINT32 filepost_http_body_parse(UINT8 *body, UINT8 *picname)
{
	HTTP_BODY body_t;
	UINT32 ret = 0;

	memset(&body_t, 0, sizeof(body_t));
	ret = http_body_parse(body, &body_t);
	if(ret)
	{
		return 1;
	}

	if(strlen(body_t.HttpRetDesc) == 0)
	{
		return 1;
	}

	if(strlen(body_t.HttpData) == 0)
	{
		return 1;
	}
	
	sprintf(picname, "%s", body_t.HttpData);
	return 0;
}

UINT32 IpcameraFileToPlatform(DEV_DRIVER *dev, UINT8 *picname_in, 
							  UINT8 *picname_out, INT32 *pic_id)
{
	http_respone post_return;
	ERR_CODE ret = ERR_SUCCESS;
	FILE_SERVER_CFG *cfg_p = NULL;

	memset(&post_return, 0, sizeof(post_return));

	cfg_p = (FILE_SERVER_CFG *)dev->sub_dev_cfg;
	ret = http_post_file_multipart(cfg_p->file_put, picname_in, 
							(void *)&post_return, cfg_p->token);
	if(ret != ERR_SUCCESS)
	{
		//SAFE_FREE(form.content);
		//SAFE_FREE(post_return.data);
		return 1;
	}

	if((post_return.data == NULL) || (post_return.data_len == 0))
	{
		return 1;
	}

	ret = filepost_http_body_parse(post_return.data, picname_out);
	if(ret != ERR_SUCCESS)
	{
		//SAFE_FREE(form.content);
		SAFE_FREE(post_return.data);
		return 1;
	}

	#if 0
	//printf("post_return.data:%s\r\n", post_return.data);
	*pic_id = get_picid(post_return.data, picname_out, url);
	if(*pic_id <= 0)
	{
		//SAFE_FREE(form.content);
		SAFE_FREE(post_return.data);
		return 1;
	}
	#endif

	//SAFE_FREE(form.content);
	SAFE_FREE(post_return.data);

	//printf("图片保存业务平台成功.\r\n");
	return 0;
}

UINT32 IpcameraTaskStop(DEVICE_INFO *dev)
{
	thread_info *thread = NULL;
	thread_info *tmp = NULL;

	UINT8	DeviceId[DEV_ID_MAX_LEN];
	UINT8	url[256];

	thread = (thread_info *)malloc(sizeof(thread_info));
	if (thread == NULL)
	{
		return 1;
	}
	memset(thread, 0, sizeof(thread_info));


	memset(DeviceId, 0, sizeof(DeviceId));
	sprintf(DeviceId, "%s_%d_%d", dev->host, dev->port, dev->slave_id);
	memset(url, 0, sizeof(url));
	sprintf(url, "%s://%s:%s@%s:%d/cam/realmonitor?channel=%d&subtype=1", 
				  dev->protocol, dev->user, dev->pwd, dev->host, dev->port, 
				  dev->slave_id);

	memset(thread, 0, sizeof(thread_info));
	sprintf(thread->task_name, "%s", "ipcamera");
	thread->argc = 4;
	thread->argv = ThreadArgCreate(	 thread->argc, 
									 thread->task_name, 
									 DeviceId,
									 url,
									 NULL);

	tmp = ThreadSearch(thread->argc, thread->argv);
	if(tmp == NULL)
	{
		LOG_PRINT(  Prome_LOG_INFO, "Running task %s not find ...", thread->task_name);
		ThreadArgFree(thread);
		SAFE_FREE(thread);
		ipcamera_cfg_clean(dev);
		return 1;
	}

	LOG_PRINT(  Prome_LOG_INFO, "Running task %s find ...", thread->task_name);
	//PrintThreadInfo(tmp);
	ThreadStop(tmp);
	ipcamera_cfg_clean(dev);
	SAFE_FREE(tmp);
	ThreadArgFree(thread);
	SAFE_FREE(thread);
	return 0;

}

UINT32 IpcameraTaskStart(DEVICE_INFO *dev)
{
	thread_info *thread = NULL;
	UINT8	DeviceId[DEV_ID_MAX_LEN];
	UINT8	url[256];
	UINT32  ret = 0;

	thread = (thread_info *)malloc(sizeof(thread_info));
	if (thread == NULL)
	{
		return 1;
	}

	memset(DeviceId, 0, sizeof(DeviceId));
	sprintf(DeviceId, "%s_%d_%d", dev->host, dev->port, dev->slave_id);
	memset(url, 0, sizeof(url));
	sprintf(url, "%s://%s:%s@%s:%d/cam/realmonitor?channel=%d&subtype=1", 
				  dev->protocol, dev->user, dev->pwd, dev->host, dev->port, 
				  dev->slave_id);

	memset(thread, 0, sizeof(thread_info));
	sprintf(thread->task_name, "%s", "ipcamera");
	thread->argc = 4;
	thread->argv = ThreadArgCreate(	 thread->argc, 
									 thread->task_name, 
									 DeviceId,
									 url,
									 NULL);
	ret = ThreadStart(thread);
	if(ret)
	{
		ThreadArgFree(thread);
		SAFE_FREE(thread);
	}

	return 0;
}

UINT32 IpcameraCfgInit(void *handle)
{
	UINT8 *filebuff = NULL;
	DEV_DRIVER *dev = NULL;
	INT32 ret = 0;

	if(handle == NULL)
	{
		return 0;
	}
	dev = (DEV_DRIVER *)handle;

	if(dev->sub_dev_cfg == NULL)
	{
		ipcamera_cfg_alloc(handle);
	}

	/*读出配置文件中所有内容*/
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

	/*解析MQTT配置*/
	ret = DeviceMqttCfgInit((void *)dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	/*解析基本配置*/
	ret = DeviceCfgChainParse((void *)dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	ret = DeviceCfgFileServerParse((void *)dev, filebuff);
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

UINT32 IpcameraCfgRecvCb(void *handle, void *data, UINT32 data_len)
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
		/*响应服务器配置下发*/
		status = STATUS_READ_CONFIG_FAIL;
		DevicePkgCfgRes((void *)dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config fail.", dev->device_id);
		return 0;
	}
	
	if(cfgid == dev->cfg_id)
	{
		/*配置ID相同，则认为配置相同无需更新，不做任何操作*/
		LOG_PRINT(  Prome_LOG_INFO, "The running config is same as the received.");
		/*响应服务器配置下发*/
		DevicePkgCfgRes((void *)dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
		return 0;
	}
	else
	{/*配置ID不同，则解析配置*/
		
		/*解析基本配置*/
		ret = DeviceCfgChainParse((void *)dev, data);
		if(ret < 0)
		{
			/*响应服务器配置下发*/
			status = STATUS_READ_CONFIG_FAIL;
			DevicePkgCfgRes((void *)dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config fail.", dev->device_id);
			return 1;
		}
		else if(ret > 0)
		{
			cfg_result = 1;
		}

		ret = DeviceCfgFileServerParse((void *)dev, data);
		if(ret < 0)
		{
			/*响应服务器配置下发*/
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
			/*响应服务器配置下发*/
			DevicePkgCfgRes(dev, data, data_len, status);
			LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
			return 0;
		}


		/*保存配置ID*/
		dev->cfg_id = cfgid;
		DeviceCfgIdSave((void *)dev, dev->cfg_file);

		/*保存基本配置*/
		DeviceCfgChainSave((void *)dev, dev->cfg_file);

		DeviceCfgFileServerSave((void *)dev, dev->cfg_file);

		/*响应服务器配置下发*/
		DevicePkgCfgRes((void *)dev, data, data_len, status);
		LOG_PRINT(  Prome_LOG_INFO, "Device %s parse config ok.", dev->device_id);
	}

	return 0;
}

UINT32 IpcameraOnline(void *handle)
{
	DeviceOnline(handle);
	return 0;
}

INT32 IpcameraMqttCfgRecvCb(void *handle, void *data, UINT32 data_len, UINT64 request_id)
{
	return DeviceMqttCfgRecvCb(handle, data, data_len, request_id);
}

#if 0
INT32 IpcameraMqttCfgRecvCb( void *handle, 
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
	{/*MQTT配置有改变*/
		DeviceMqttCfgSave((void *)dev, dev->cfg_file);
		need_restart = 1;
	}
	else if(ret == 0)
	{/*MQTT配置无变化*/
		LOG_PRINT(  Prome_LOG_INFO, "The running mqtt config is same as the received.");
		need_restart = 0;
	}
	else
	{/*MQTT配置解析出错*/
		dev->dev_status = STATUS_READ_CONFIG_FAIL;
		LOG_PRINT(  Prome_LOG_ERROR, "Device %s parse mqtt config fail.", dev->device_id);
		need_restart = 0;
	}

	return need_restart;
}
#endif
void IpcameraTaskClean()
{
	UINT8 cmd[256];
	
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "killall %s", "ipcamera"); 
	SystemNew(cmd);
}


