#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_json.h"
#include "dev_pkg.h"

#include "log.h"
#include "hexutils.h"
#include "mosquitto_api.h"
#include "gateway.h"
#include "upgrade.h"


#ifdef HUMSENSOR
#include "humsensor.h"

#endif

#ifdef TEMPSENSOR
#include "tempsensor.h"
#endif


#ifdef RFID
#include "rfid.h"
#endif

#ifdef IPCAMERA
#include "ipcamera.h"
#endif

#ifdef PLC
#include "plc.h"
#endif


static UINT32 gateway_device_add(GATEWAY_CONFIG *cfg, DEVICE_INFO *new_dev)
{
	DEVICE_INFO *ptemp = NULL;
	DEVICE_INFO *head = NULL;

	if(cfg == NULL)
	{
		return 1;
	}

	head = cfg->dev_list;

	if (head == NULL)
	{
		new_dev->prev=new_dev->next = NULL;
		cfg->dev_list = new_dev;
		return 0;
	}

	for(ptemp=head; ptemp->next!=NULL; ptemp=ptemp->next)
	{
	}

	ptemp->next = new_dev;
	new_dev->prev = ptemp;
	new_dev->next = NULL;
	return 0;
	
}

static void gateway_device_del(GATEWAY_CONFIG *cfg, DEVICE_INFO *del)
{
	DEVICE_INFO *ptemp = NULL;
	DEVICE_INFO *pre = NULL;
	DEVICE_INFO *head = NULL;

	if(del == NULL)
	{
		return ;
	}

	head = cfg->dev_list;

	if (head == NULL)
	{
		return ;
	}
	
	pre = head;
	ptemp = head;
	if(ptemp == del)
	{
		head = NULL;
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



static UINT32 gateway_upgrade_cfg_prase(DEV_DRIVER *dev, UINT8 *package, UPGRADE_CFG *cfg)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	len = 0;
	tmp = JsonProtocolGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(cfg->protocol, tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		return -1;
	}

	if(strcmp(cfg->protocol, "http") == 0)
	{
		len = 0;
		tmp = JsonUrlGet((void *)dev, package, &len);
		if(tmp)
		{
			//SAFE_FREE(cfg->url);
			cfg->url = malloc(len + 1);
			if(cfg->url == NULL)
			{
				return -1;
			}
			memset(cfg->url, 0, len + 1);
			memcpy(cfg->url, tmp, len);
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
		tmp = JsonHostGet((void *)dev, package, &len);
		if(tmp)
		{
			memcpy(cfg->host, tmp, len);
			SAFE_FREE(tmp);
		}
		else
		{
			return -1;
		}

		len = 0;
		tmp = JsonPortGet((void *)dev, package, &len);
		if(tmp)
		{
			memcpy((UINT8 *)&(cfg->port), tmp, len);
			SAFE_FREE(tmp);
		}
		else
		{
			return -1;
		}

		len = 0;
		tmp = JsonUserGet((void *)dev, package, &len);
		if(tmp)
		{
			memcpy(cfg->user, tmp, len);
			SAFE_FREE(tmp);
		}
		else
		{
			return -1;
		}

		len = 0;
		tmp = JsonPwdGet((void *)dev, package, &len);
		if(tmp)
		{
			memcpy(cfg->pwd, tmp, len);
			SAFE_FREE(tmp);
		}
		else
		{
			return -1;
		}
	}

	len = 0;
	tmp = JsonFileGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(cfg->file, tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		return -1;
	}

	len = 0;
	tmp = JsonSoftVerGet((void *)dev, package, &len);
	if(tmp)
	{
		memcpy(cfg->ver, tmp, len);
		SAFE_FREE(tmp);
	}
	else
	{
		return -1;
	}

	return 0;
}


UINT32 GatewayUpgradeCfgCb(void *handle, void *data, UINT32 len, UINT64 request_id)
{
	DEV_DRIVER *dev = NULL;
	UINT32 ret = 0;
	GATEWAY_CONFIG *cfg = NULL;

	dev = (DEV_DRIVER *)handle;
	cfg = (GATEWAY_CONFIG *)dev->sub_dev_cfg;

	ret = gateway_upgrade_cfg_prase(dev, (UINT8 *)data, &cfg->upgrade);
	if(ret)
	{
		LOG_PRINT(WX_LOG_INFO, "Device %s parse upgrade config fail.", dev->device_id);
		//status = STATUS_UPGRADE_FAIL;
	}
	else
	{

		LOG_PRINT(WX_LOG_INFO, "Device %s parse upgrade config ok.", dev->device_id);
		//status = STATUS_SUCCESS;
	}


	return 0;
}


static UINT32 gateway_devlist_delete(DEV_DRIVER *dev)
{
	UINT8 *filebuff = NULL;
	data_package pkg;
	DEVICE_INFO *ptemp = NULL;
	DEVICE_INFO *head = NULL;
	//UINT32 ret = 0;
	CFG_KEY key;
	GATEWAY_CONFIG *cfg = NULL;

	if(dev == NULL)
	{
		return 1;
	}

	cfg = (GATEWAY_CONFIG *)dev->sub_dev_cfg;
	if(cfg->dev_list == NULL)
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
	key.key_str = KEY_STR_DEV_LIST;
	PackageJsonDataDel(&pkg, &key);

	head = cfg->dev_list;
	while(head != NULL)
	{
		ptemp = head;
		head = ptemp->next;
		//sGatewayThreadStop(ptemp);
		SAFE_FREE(ptemp->addr_list);
		SAFE_FREE(ptemp);
	}
	
	if(head == NULL)
	{
		cfg->dev_list = NULL;
	}

	DeviceCfgSaveFormatted(dev->cfg_file, pkg.data);
	
	PackageFree(&pkg);
	return 0;
}

#ifdef PLC
static UINT32 _gateway_devlist_save_plc_(	DEV_DRIVER *dev, 
									 		data_package *package, 
									 		DEVICE_INFO *devinfo)
{
	UINT32 ret = 0;
	
	ret = JsonDevTypeAdd((void *)dev, package, devinfo->dev_type);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonHostAdd((void *)dev, package, devinfo->host);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPortAdd((void *)dev, package, devinfo->port);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonSlaveIdAdd((void *)dev, package, devinfo->slave_id);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonProtocolAdd((void *)dev, package, devinfo->protocol);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonSampleIntAdd((void *)dev, package, devinfo->sample_interval);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPollTimeAdd((void *)dev, package, devinfo->poll_time);
	if(ret == 0)
	{
		return 1;
	}

	return 0;
}

static INT32 gateway_devlist_parse_plc( DEV_DRIVER *dev, 
										UINT8 *sub_pkg, 
										DEVICE_INFO *dev_info)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	tmp = JsonProtocolGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->protocol, tmp, len);
	SAFE_FREE(tmp);
	

	tmp = JsonHostGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->host, tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonPortGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy((UINT8 *)&(dev_info->port), tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonSlaveIdGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		dev_info->slave_id = 1;
	}
	else
	{
		memcpy((UINT8 *)&(dev_info->slave_id), tmp, len);
		SAFE_FREE(tmp);
	}

	tmp = JsonPollTimeGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		dev_info->poll_time = 1;
	}
	else
	{
		memcpy((UINT8 *)&(dev_info->poll_time), tmp, len);
		SAFE_FREE(tmp);
	}

	tmp = JsonSampleIntGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		dev_info->sample_interval = 30;
	}
	else
	{
		memcpy((UINT8 *)&(dev_info->sample_interval), tmp, len);
		SAFE_FREE(tmp);
	}

	return 0;
}
#endif

#ifdef RFID
static UINT32 _gateway_devlist_save_rfid_(	DEV_DRIVER *dev, 
											data_package *package, 
											DEVICE_INFO *devinfo)
{
	UINT32 ret = 0;
	
	ret = JsonDevTypeAdd((void *)dev, package, devinfo->dev_type);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonHostAdd((void *)dev, package, devinfo->host);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPortAdd((void *)dev, package, devinfo->port);
	if(ret == 0)
	{
		return 1;
	}

	return 0;
}

static INT32 gateway_devlist_parse_rfid(	DEV_DRIVER *dev, 
											UINT8 *sub_pkg, 
											DEVICE_INFO *dev_info)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	tmp = JsonHostGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->host, tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonPortGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy((UINT8 *)&(dev_info->port), tmp, len);
	SAFE_FREE(tmp);

	return 0;
}
#endif
#ifdef HUMSENSOR										
static UINT32 _gateway_devlist_save_humsensor_(	DEV_DRIVER *dev, 
												data_package *package, 
												DEVICE_INFO *devinfo)
{
	UINT32 ret = 0;
	
	ret = JsonDevTypeAdd((void *)dev, package, devinfo->dev_type);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonHostAdd((void *)dev, package, devinfo->host);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPortAdd((void *)dev, package, devinfo->port);
	if(ret == 0)
	{
		return 1;
	}

	return 0;
}

static INT32 gateway_devlist_parse_humsensor(	DEV_DRIVER *dev, 
												UINT8 *sub_pkg, 
												DEVICE_INFO *dev_info)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	tmp = JsonHostGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->host, tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonPortGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy((UINT8 *)&(dev_info->port), tmp, len);
	SAFE_FREE(tmp);

	return 0;
}
#endif
#ifdef TEMPSENSOR
static UINT32 _gateway_devlist_save_tempsensor_( DEV_DRIVER *dev, 
												data_package *package, 
												DEVICE_INFO *devinfo)
{
	UINT32 ret = 0;
	
	ret = JsonDevTypeAdd((void *)dev, package, devinfo->dev_type);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonHostAdd((void *)dev, package, devinfo->host);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPortAdd((void *)dev, package, devinfo->port);
	if(ret == 0)
	{
		return 1;
	}

	return 0;
}

static INT32 gateway_devlist_parse_tempsensor(	DEV_DRIVER *dev, 
												UINT8 *sub_pkg, 
												DEVICE_INFO *dev_info)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	tmp = JsonHostGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->host, tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonPortGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy((UINT8 *)&(dev_info->port), tmp, len);
	SAFE_FREE(tmp);

	return 0;
}
#endif

#ifdef IPCAMERA
static UINT32 _gateway_devlist_save_ipcamera_(	DEV_DRIVER *dev, 
												data_package *package, 
												DEVICE_INFO *devinfo)
{
	UINT32 ret = 0;
	
	ret = JsonDevTypeAdd((void *)dev, package, devinfo->dev_type);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonHostAdd((void *)dev, package, devinfo->host);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPortAdd((void *)dev, package, devinfo->port);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonSlaveIdAdd((void *)dev, package, devinfo->slave_id);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonProtocolAdd((void *)dev, package, devinfo->protocol);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonUserAdd((void *)dev, package, devinfo->user);
	if(ret == 0)
	{
		return 1;
	}

	ret = JsonPwdAdd((void *)dev, package, devinfo->pwd);
	if(ret == 0)
	{
		return 1;
	}

	return 0;
}


static INT32 gateway_devlist_parse_ipcamera(	DEV_DRIVER *dev, 
												UINT8 *sub_pkg, 
												DEVICE_INFO *dev_info)
{
	UINT8 *tmp = NULL;
	UINT32 len = 0;

	tmp = JsonProtocolGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->protocol, tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonHostGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->host, tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonPortGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy((UINT8 *)&(dev_info->port), tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonSlaveIdGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy((UINT8 *)&(dev_info->slave_id), tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonUserGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->user, tmp, len);
	SAFE_FREE(tmp);

	tmp = JsonPwdGet((void *)dev, sub_pkg, &len);
	if(tmp == NULL)
	{
		return 1;
	}
	memcpy(dev_info->pwd, tmp, len);
	SAFE_FREE(tmp);

	return 0;
}
#endif


static UINT32 _gateway_devlist_save_(	DEV_DRIVER *dev, 
									 	cJSON *array, 
									 	DEVICE_INFO *devinfo)
{
	cJSON *Item_new = NULL;
	cJSON *addr_list = NULL;
	data_package package;
	UINT32 ret = 0;

	memset(&package, 0, sizeof(package));

	switch(devinfo->dev_type)
	{
		#ifdef HUMSENSOR
		case DEV_TYPE_HUMSENSOR:
			ret = _gateway_devlist_save_humsensor_(dev, &package, devinfo);
			break;
		#endif
		#ifdef TEMPSENSOR
		case DEV_TYPE_TEMPSENSOR:
			ret = _gateway_devlist_save_tempsensor_(dev, &package, devinfo);
			break;
		#endif
		#ifdef RFID
		case DEV_TYPE_RFID:
			ret = _gateway_devlist_save_rfid_(dev, &package, devinfo);
			break;
		#endif
		#ifdef IPCAMERA
		case DEV_TYPE_IPCAMERA:
			ret = _gateway_devlist_save_ipcamera_(dev, &package, devinfo);
			break;
		#endif
		#ifdef PLC
		case DEV_TYPE_PLC:
			ret = _gateway_devlist_save_plc_(dev, &package, devinfo);
			break;
		#endif
		default:
			ret = 1;
			break;
	}

	if(ret)
	{
		PackageFree(&package);
		return 1;
	}

	Item_new = cJSON_Parse((char *)package.data);
	if(Item_new == NULL)
	{
		PackageFree(&package);
		return 1;
	}

	if(devinfo->addr_list)
	{
		addr_list = cJSON_Parse((char *)devinfo->addr_list);
		if(addr_list)
		{
			cJSON_AddItemToObject(Item_new, KEY_STR_ADDR_LIST, addr_list);
		}
	}
	cJSON_AddItemToArray(array, Item_new);
	

	//printf("array:%s\r\n", cJSON_Print(array));
	PackageFree(&package);
	//cJSON_Delete(Item_new); 

	return 0;

}


static UINT32 gateway_devlist_save(DEV_DRIVER *dev, UINT8 *file)
{
	UINT8 *filebuff = NULL;
	cJSON *array = NULL;
	cJSON *root = NULL;
	cJSON *Item_new = NULL;
	cJSON *Item = NULL;
	cJSON *old = NULL;
	UINT8 *json = NULL;
	//int find = 0;
	DEVICE_INFO *ptemp = NULL;
	DEVICE_INFO *head = NULL;
	GATEWAY_CONFIG *cfg = NULL;

	if(dev == NULL)
	{
		return 1;
	}

	cfg = (GATEWAY_CONFIG *)dev->sub_dev_cfg;

	if(cfg->dev_list == NULL)
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

	array = cJSON_GetObjectItem(root, KEY_STR_DEV_LIST);
	if(array != NULL)
	{
		cJSON_DeleteItemFromObject(root, KEY_STR_DEV_LIST);
	}
	array = cJSON_CreateArray();
	cJSON_AddItemToArray(array, Item_new);
	cJSON_AddItemToObject(root, KEY_STR_DEV_LIST, array);

	head = cfg->dev_list;
	for(ptemp=head; ptemp!=NULL; ptemp=ptemp->next)
	{
		_gateway_devlist_save_(dev, array, ptemp);
		//GatewayThreadStart((void *) ptemp);
	}

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



static INT32 gateway_devlist_parse(DEV_DRIVER *dev, 
									UINT8 *data, 
									GATEWAY_CONFIG *cfg)
{
	cJSON *array = NULL;
	cJSON *sub_array = NULL;
	cJSON *root = NULL;
	cJSON *Item = NULL;
	UINT8 *sub_pkg = NULL;
	UINT8 *tmp = NULL;
	DEVICE_INFO *dev_info = NULL;
	UINT32 len = 0;
	UINT32 ret = 0;

	root = cJSON_Parse(data);
	if(root == NULL)
	{
		return -1;
	}

	array = cJSON_GetObjectItem(root, KEY_STR_DEV_LIST);
	if(array == NULL)
	{
		cJSON_Delete(root); 
		return 0;
	}

	cJSON_ArrayForEach(Item, array)
	{
		sub_pkg = cJSON_PrintUnformatted(Item);
		if(sub_pkg == NULL)
		{
			continue;
		}

		dev_info = malloc(sizeof(DEVICE_INFO));
		if(dev_info == NULL)
		{
			SAFE_FREE(sub_pkg);
			continue;
		}
		memset(dev_info, 0, sizeof(DEVICE_INFO));

		tmp = JsonDevTypeGet((void *)dev, sub_pkg, &len);
		if(tmp)
		{
			dev_info->dev_type = DEV_TYPE_INT(tmp);
			SAFE_FREE(tmp);
		}
		else
		{
			SAFE_FREE(sub_pkg);
			SAFE_FREE(dev_info);
			continue;
		}

		switch(dev_info->dev_type)
		{
			#ifdef HUMSENSOR
			case DEV_TYPE_HUMSENSOR:
				ret = gateway_devlist_parse_humsensor(dev, sub_pkg, dev_info);
				break;
			#endif
			#ifdef TEMPSENSOR
			case DEV_TYPE_TEMPSENSOR:
				ret = gateway_devlist_parse_tempsensor(dev, sub_pkg, dev_info);
				break;
			#endif
			#ifdef RFID
			case DEV_TYPE_RFID:
				ret = gateway_devlist_parse_rfid(dev, sub_pkg, dev_info);
				break;
			#endif
			#ifdef IPCAMERA
			case DEV_TYPE_IPCAMERA:
				ret = gateway_devlist_parse_ipcamera(dev, sub_pkg, dev_info);
				break;
			#endif
			#ifdef PLC
			case DEV_TYPE_PLC:
				ret = gateway_devlist_parse_plc(dev, sub_pkg, dev_info);
				break;
			#endif
			default:
				ret = 1;
				break;
		}

		if(ret)
		{
			SAFE_FREE(sub_pkg);
			SAFE_FREE(dev_info);
			continue;
		}
		
		sub_array = cJSON_GetObjectItem(Item, KEY_STR_ADDR_LIST);
		if(sub_array)
		{
			if(dev_info->addr_list)
			{
				SAFE_FREE(dev_info->addr_list);
			}
			dev_info->addr_list = cJSON_PrintUnformatted(sub_array);
			if(dev_info->addr_list == NULL)
			{
				SAFE_FREE(sub_pkg);
				SAFE_FREE(dev_info);
				continue;
			}
		}
		ret = gateway_device_add(cfg, dev_info);
		if(ret)
		{
			SAFE_FREE(dev_info);
		}
		SAFE_FREE(sub_pkg);
	}

	cJSON_Delete(root); 
	return 1;
}

static UINT32 gateway_cfg_alloc(void *handle)
{
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	
	dev->sub_dev_cfg = (void *)malloc(sizeof(GATEWAY_CONFIG));
	if(dev->sub_dev_cfg == NULL)
	{
		return 1;
	}
	memset(dev->sub_dev_cfg, 0, sizeof(GATEWAY_CONFIG));
	return 0;
}

UINT32 GatewayThreadStop(void *handle)
{
	UINT32 ret = 0;
	DEVICE_INFO *dev = NULL;

	if(handle == NULL)
	{
		return 0;
	}

	dev = (DEVICE_INFO *)handle;
	
	switch(dev->dev_type)
	{
		#ifdef PLC
		case DEV_TYPE_PLC:
		{
			ret = PlcTaskStop(dev);
			break;
		}
		#endif
		#ifdef IPCAMERA
		case DEV_TYPE_IPCAMERA:
			ret = IpcameraTaskStop(dev);
			break;
		#endif
		#ifdef HUMSENSOR
		case DEV_TYPE_HUMSENSOR:
			ret = HumsensorTaskStop(dev);
			break;
		#endif
		#ifdef TEMPSENSOR
		case DEV_TYPE_TEMPSENSOR:
			ret = TempsensorTaskStop(dev);
			break;
		#endif
		#ifdef RFID
		case DEV_TYPE_RFID:
			ret = RfidTaskStop(dev);
			break;
		#endif
	}
	return ret;
}


UINT32 GatewayThreadStart(void *handle)
{
	UINT32 ret = 0;
	DEVICE_INFO *dev = NULL;

	if(handle == NULL)
	{
		return 0;
	}

	dev = (DEVICE_INFO *)handle;
	
	switch(dev->dev_type)
	{
		#ifdef PLC
		case DEV_TYPE_PLC:
		{
			ret = PlcTaskStart(dev);
			break;
		}
		#endif
		#ifdef IPCAMERA
		case DEV_TYPE_IPCAMERA:
			ret = IpcameraTaskStart(dev);
			break;
		#endif
		#ifdef HUMSENSOR
		case DEV_TYPE_HUMSENSOR:
			ret = HumsensorTaskStart(dev);
			break;
		#endif
		#ifdef TEMPSENSOR
		case DEV_TYPE_TEMPSENSOR:
			ret = TempsensorTaskStart(dev);
			break;
		#endif
		#ifdef RFID
		case DEV_TYPE_RFID:
			ret = RfidTaskStart(dev);
			break;
		#endif
	}

	return ret;
}

static void *gateway_time_request(void *ptr)
{
	DEV_DRIVER *dev = (DEV_DRIVER *)ptr;
	
	while(1)
	{
		sleep(60);
		DevicePkgTimeReq(dev);
	}
}


void GatewayTimeRequestStart(void *handle)
{
	pthread_attr_t attr; 
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return;
	}

	dev = (DEV_DRIVER *)handle;

	pthread_attr_init( &attr ); 
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	if (!pthread_create(&dev->timesync_tid, &attr, 
						gateway_time_request, (void *)dev))
    {
        LOG_PRINT(WX_LOG_INFO, "Create %s request time thread OK!", 
							 dev->device_id);
    }
    else
    {
        LOG_PRINT(WX_LOG_ERROR, "Create %s request time thread fail!", 
							  dev->device_id);
    }
	pthread_attr_destroy(&attr);
	sleep(1);
}

void GatewayTimeRequestStop(void *handle)
{
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return;
	}

	dev = (DEV_DRIVER *)handle;

	if(dev->timesync_tid != 0)
	{
		pthread_cancel(dev->timesync_tid);
	}
}

void GatewayTimeRequestRestart(void *handle)
{
	GatewayTimeRequestStop(handle);
	GatewayTimeRequestStart(handle);
}

static void *gateway_log_send(void *ptr)
{
	DEV_DRIVER *dev = (DEV_DRIVER *)ptr;
	char buffer[1024*2] = {0,};

	//创建socket对象
    int sockfd=socket(AF_INET,SOCK_DGRAM,0);

    //创建网络通信对象
    struct sockaddr_in addr;
    addr.sin_family =AF_INET;
    addr.sin_port =htons(LOG_SERVER_PORT);
    addr.sin_addr.s_addr=inet_addr(LOG_SERVER_IPADDR);

    //绑定socket对象与通信链接
    int ret =bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
    if(0>ret)
    {
        printf("bind\n");
		close(sockfd);
        return NULL;

    }
    struct sockaddr_in cli;
    socklen_t len=sizeof(cli);
	
	while(1)
	{
		memset(buffer, 0, sizeof(buffer));
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli,&len);
        //printf("recv buffer =%s\n",buffer);
		
        dev->data_report((void *)dev, (void *)buffer);
	}
}

void GatewayLogSendStart(void *handle)
{
	pthread_attr_t attr; 
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return;
	}

	dev = (DEV_DRIVER *)handle;

	pthread_attr_init( &attr ); 
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	if (!pthread_create(&dev->log_tid, &attr, 
						gateway_log_send, (void *)dev))
    {
        LOG_PRINT(WX_LOG_INFO, "Create %s log send thread OK!", 
							 dev->device_id);
    }
    else
    {
        LOG_PRINT(WX_LOG_ERROR, "Create %s log send thread fail!", 
							  dev->device_id);
    }
	pthread_attr_destroy(&attr);
	sleep(1);
}

void GatewayLogSendStop(void *handle)
{
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return;
	}

	dev = (DEV_DRIVER *)handle;

	if(dev->log_tid != 0)
	{
		pthread_cancel(dev->log_tid);
	}
}

void GatewayLogSendRestart(void *handle)
{
	GatewayLogSendStop(handle);
	GatewayLogSendStart(handle);
}



UINT32 GatewayDevlistStart(void *handle)
{
	DEV_DRIVER *dev = NULL;
	DEVICE_INFO *ptemp = NULL;
	GATEWAY_CONFIG *cfg = NULL;
	DEVICE_INFO *head = NULL;

	if(handle == NULL)
	{
		return 1;
	}
	dev = (DEV_DRIVER *)handle;

	//#ifdef PLC
	//PlcTaskClean();
	//#endif
	
	cfg = (GATEWAY_CONFIG *)dev->sub_dev_cfg;
	if(cfg == NULL)
	{
		return 1;
	}
	
	head = cfg->dev_list;
	if (head == NULL)
	{
		return 1;
	}

	for(ptemp=head; ptemp!=NULL; ptemp=ptemp->next)
	{
		GatewayThreadStart(ptemp);
		//sleep(3);
	}

	return 0;
}


UINT32 GatewayDevlistStop(void *handle)
{
	DEV_DRIVER *dev = NULL;
	DEVICE_INFO *ptemp = NULL;
	GATEWAY_CONFIG *cfg = NULL;
	DEVICE_INFO *head = NULL;

	if(handle == NULL)
	{
		return 1;
	}
	dev = (DEV_DRIVER *)handle;
	
	cfg = (GATEWAY_CONFIG *)dev->sub_dev_cfg;
	if(cfg == NULL)
	{
		return 1;
	}

	head = cfg->dev_list;
	if (head == NULL)
	{
		return 1;
	}

	for(ptemp=head; ptemp!=NULL; ptemp=ptemp->next)
	{
		GatewayThreadStop(ptemp);
		//dev->task_stop(ptemp);
	}

	return 0;
}


UINT32 GatewayCfgInit(void *handle)
{
	UINT8 *filebuff = NULL;
	DEV_DRIVER *dev = NULL;
	GATEWAY_CONFIG *cfg = NULL;
	INT32 ret = 0;

	if(handle == NULL)
	{
		return 1;
	}
	dev = (DEV_DRIVER *)handle;

	if(dev->sub_dev_cfg == NULL)
	{
		gateway_cfg_alloc(handle);
	}
	cfg = (GATEWAY_CONFIG *)dev->sub_dev_cfg;

	/*读出配置文件中所有内容*/
	filebuff = ReadFileAll(dev->cfg_file);
	if(filebuff == NULL)
	{
		LOG_PRINT(WX_LOG_ERROR, "Read file %s fail, maybe not exist or empty!", dev->cfg_file);
		return 1;
	}

	ret = DeviceCfgIdParse(dev, filebuff, &dev->cfg_id);
	if(ret < 0)
	{
		LOG_PRINT(WX_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	/*解析MQTT配置*/
	ret = DeviceMqttCfgInit(dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(WX_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	/*解析基本配置*/
	ret = DeviceCfgChainParse(dev, filebuff);
	if(ret < 0)
	{
		LOG_PRINT(WX_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	/*解析设备列表*/
	ret = gateway_devlist_parse(dev, filebuff, cfg);
	if(ret < 0)
	{
		LOG_PRINT(WX_LOG_ERROR, "Device %s init config fail.", dev->device_id);
		SAFE_FREE(filebuff);
		return 1;
	}

	SAFE_FREE(filebuff);

	LOG_PRINT(WX_LOG_INFO, "Device %s init config ok.", dev->device_id);

	DeviceCfgIdSave(dev, DEV_DEFAULT_CFG);
	DeviceCfgChainSave(dev, DEV_DEFAULT_CFG);

	
	return 0;
	
}

UINT32 GatewayCfgRecvCb(void *handle, void *data, UINT32 data_len)
{
	UINT64 cfgid = 0;
	DEVICE_STATUS status = 0;
	INT32 cfg_result = 0;
	INT32 dev_result = 0;
	INT32 ret = 0;
	GATEWAY_CONFIG gate_cfg;
	GATEWAY_CONFIG *tmp = NULL;
	
	DEV_DRIVER *dev = NULL;
	
	if(handle == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	LOG_PRINT(WX_LOG_INFO, "Device %s receiver config.", dev->device_id);
	//LOG_PRINT(WX_LOG_INFO, "%s", data);
	LOG_PRINT(WX_LOG_INFO, "Device %s start parse config.", dev->device_id);
	
	ret = DeviceCfgIdParse(dev, data, &cfgid);
	if(ret)
	{
		/*响应服务器配置下发*/
		status = STATUS_READ_CONFIG_FAIL;
		DevicePkgCfgRes(dev, data, data_len, status);
		LOG_PRINT(WX_LOG_ERROR, "Device %s parse config fail.", dev->device_id);
		return 0;
	}
	
	if(cfgid == dev->cfg_id)
	{/*配置ID相同，则认为配置相同无需更新，不做任何操作*/
		LOG_PRINT(WX_LOG_INFO, "The running config is same as the received.");
		/*响应服务器配置下发*/
		DevicePkgCfgRes(dev, data, data_len, status);
		LOG_PRINT(WX_LOG_INFO, "Device %s parse config ok.", dev->device_id);
		return 0;
	}
	else
	{/*配置ID不同，则解析配置*/
		
		/*解析基本配置*/
		cfg_result = DeviceCfgChainParse(dev, data);
		if(cfg_result < 0)
		{
			/*响应服务器配置下发*/
			status = STATUS_READ_CONFIG_FAIL;
			DevicePkgCfgRes(dev, data, data_len, status);
			LOG_PRINT(WX_LOG_ERROR, "Device %s parse config fail.", dev->device_id);
			return 0;
		}
		
		/*解析设备列表*/
		memset(&gate_cfg, 0, sizeof(gate_cfg));
		dev_result = gateway_devlist_parse(dev, data, &gate_cfg);
		if(dev_result < 0)
		{
			/*响应服务器配置下发*/
			status = STATUS_READ_CONFIG_FAIL;
			DevicePkgCfgRes(dev, data, data_len, status);
			LOG_PRINT(WX_LOG_ERROR, "Device %s parse config fail.", dev->device_id);
			return 0;
		}

		/*配置未发生改变*/
		if((cfg_result == 0) && (dev_result == 0))
		{
			LOG_PRINT(WX_LOG_INFO, "The running config is same as the received.");
			/*响应服务器配置下发*/
			DevicePkgCfgRes(dev, data, data_len, status);
			LOG_PRINT(WX_LOG_INFO, "Device %s parse config ok.", dev->device_id);
			return 0;
		}

		/*配置有发生改变*/
		GatewayDevlistStop(handle);
		/*更新设备列表*/
		if(dev_result > 0)
		{
			gateway_devlist_delete(dev);
			tmp = (GATEWAY_CONFIG *)dev->sub_dev_cfg;
			SAFE_FREE(tmp->dev_list);
			tmp->dev_list = gate_cfg.dev_list;
		}

		/*保存配置ID*/
		dev->cfg_id = cfgid;
		DeviceCfgIdSave(dev, dev->cfg_file);

		/*保存基本配置*/
		DeviceCfgChainSave(dev, dev->cfg_file);
		
		/*网关设备需要保存相关配置到默认配置文件，以便下联设备继承*/
		DeviceCfgIdSave(dev, DEV_DEFAULT_CFG);
		DeviceCfgChainSave(dev, DEV_DEFAULT_CFG);

		/*保存设备列表*/
		gateway_devlist_save(dev, dev->cfg_file);

		/*响应服务器配置下发*/
		DevicePkgCfgRes(dev, data, data_len, status);
		LOG_PRINT(WX_LOG_INFO, "Device %s parse config ok.", dev->device_id);
		
		GatewayDevlistStart(handle);
	}

	return 0;
}

UINT32 GatewayOnline(void *handle)
{
	UINT32 ret = 0;

	if(handle == NULL)
	{
		return 1;
	}

	DeviceOnline(handle);
	GatewayTimeRequestStart(handle);
	GatewayLogSendStart(handle);
	
	return 0;
}

UINT32 GatewayOffline(void *handle)
{
	UINT32 ret = 0;
	DEVICE_INFO *ptemp = NULL;
	DEVICE_INFO *next = NULL;
	DEVICE_INFO *head = NULL;
	GATEWAY_CONFIG * cfg = NULL;
	DEV_DRIVER *dev = NULL;

	if(handle == NULL)
	{
		return 1;
	}

	dev = (DEV_DRIVER *)handle;
	GatewayTimeRequestStop(handle);
	GatewayLogSendStop(handle);

	cfg = (GATEWAY_CONFIG *)dev->sub_dev_cfg;
	if(cfg == NULL)
	{
		return 0;
	}

	head = cfg->dev_list;
	if(head == NULL)
	{
		return 0;
	}

	ptemp = head;
	while(ptemp != NULL)
	{
		next = ptemp->next;
		gateway_device_del(cfg, ptemp);
		SAFE_FREE(ptemp->addr_list);
		SAFE_FREE(ptemp);
		ptemp = next;
	}

	return 0;
}


UINT32 GatewayDataPkg(void *handle, void *data, data_package *pkg)
{
	UINT32 ret = 0;

	if(handle == NULL || data == NULL || pkg == NULL)
	{
		return 1;
	}

	ret = JsonLogAdd(handle, pkg, (UINT8 *)data);
	if(ret == 0)
	{
		return 1;
	}
	
	return 0;
}


INT32 GatewayMqttCfgRecvCb(void *handle, void *data, UINT32 data_len, UINT64 request_id)
{
	DEV_DRIVER *dev = NULL;
	INT32 ret = 0;
	INT32 need_restart = 0;

	dev = (DEV_DRIVER *)handle;

	LOG_PRINT(WX_LOG_INFO, "Device %s receiver mqtt config.", dev->device_id);
	//LOG_PRINT(WX_LOG_INFO, "%s", data);
	LOG_PRINT(WX_LOG_INFO, "Device %s start parse mqtt config.", dev->device_id);

	ret = DeviceMqttCfgParse(dev, data, 1);
	if(ret > 0)
	{/*MQTT配置有改变*/
		GatewayDevlistStop((void *)dev);
		DeviceMqttCfgSave((void *)dev, dev->cfg_file);
		DeviceMqttCfgSave((void *)dev, DEV_DEFAULT_CFG);
		//GatewayPowerOff((void *)dev);
		need_restart = 1;
	}
	else if(ret == 0)
	{/*MQTT配置无变化*/
		LOG_PRINT(WX_LOG_INFO, "The running mqtt config is same as the received.");
		need_restart = 0;
	}
	else
	{/*MQTT配置解析出错*/
		dev->dev_status = STATUS_READ_CONFIG_FAIL;
		LOG_PRINT(WX_LOG_ERROR, "Device %s parse mqtt config fail.", dev->device_id);
		need_restart = 0;
	}

	return need_restart;
}



