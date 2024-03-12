#include <string.h>
#include <stdlib.h>

#include "basic_types.h"
#include "log.h"

#include "cJSON.h"
#include "hexutils.h"
#include "dev_json.h"
#include "dev_comm.h"


UINT32 JsonPubkeyAdd(void *handle, data_package *pkg)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PUBKEY;
	key.key_value = dev->dev_pubkey;
	key.value_len = strlen(dev->dev_pubkey);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonDevIdAdd(void *handle, data_package *pkg)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}
	dev = (DEV_DRIVER *)handle;
	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_DEV_ID;
	key.key_value = dev->device_id;
	key.value_len = strlen(dev->device_id);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonTimeStampAdd(void *handle, data_package *pkg, TIMESTAMP stamp)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT64;
	key.key_str = KEY_STR_TIMESTAMP;
	key.key_value = (UINT8 *)&stamp;
	key.value_len = sizeof(stamp);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonSignAdd(void *handle, data_package *pkg, UINT8 *sign)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_SIGN;
	key.key_value = sign;
	key.value_len = strlen(sign);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonHumAdd(void *handle, data_package *pkg, double hum)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_DOUBLE;
	key.key_str = KEY_STR_HUM;
	key.key_value = (UINT8 *)&hum;
	key.value_len = sizeof(hum);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonTempAdd(void *handle, data_package *pkg, double temp)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_DOUBLE;
	key.key_str = KEY_STR_TEMP;
	key.key_value = (UINT8 *)&temp;
	key.value_len = sizeof(temp);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}


UINT32 JsonNodeIdAdd(void *handle, data_package *pkg, UINT32 id)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_NODE_ID;
	key.key_value = (UINT8 *)&id;
	key.value_len = sizeof(id);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonPkgTypeAdd(void *handle, data_package *pkg, PKG_TYPE_E pkgtype)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_PKG_TYPE;
	key.key_value = (UINT8 *)&pkgtype;
	key.value_len = sizeof(pkgtype);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonHashAdd(void *handle, data_package *pkg, UINT8 *hash)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_POEHASH;
	key.key_value = hash;
	key.value_len = strlen(hash);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonDevStatusAdd(void *handle, data_package *pkg, DEVICE_STATUS status)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_DEV_STATUS;
	key.key_value = (UINT8 *)&status;
	key.value_len = sizeof(status);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonDescAdd(void *handle, data_package *pkg, UINT8 *desc)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_DEV_DESC;
	key.key_value = desc;
	key.value_len = strlen(desc);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonDevTypeAdd(void *handle, data_package *pkg, DEV_TYPE_E dev_type)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_DEV_TYPE;
	key.key_value = DEV_TYPE_STR(dev_type);
	key.value_len = strlen(DEV_TYPE_STR(dev_type));
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonRfidAdd(void *handle, data_package *pkg, UINT8 *rfid)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_RFID;
	key.key_value = rfid;
	key.value_len = strlen(rfid);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonAtidAdd(void *handle, data_package *pkg, UINT32 atid)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;				
	key.key_str = KEY_STR_ATID;
	key.key_value = (UINT8 *)&atid;
	key.value_len = sizeof(atid);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonRssiAdd(void *handle, data_package *pkg, double rssi)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_DOUBLE;
	key.key_str = KEY_STR_RSSI;
	key.key_value = (UINT8 *)&rssi;
	key.value_len = sizeof(rssi);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonPicAdd(void *handle, data_package *pkg, UINT8 *pic)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PIC;
	key.key_value = pic;
	key.value_len = strlen(pic);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonPicIdAdd(void *handle, data_package *pkg, UINT32 pic_id)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_PIC_ID;
	key.key_value = (UINT8 *)&pic_id;
	key.value_len = sizeof(pic_id);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonPicHashAdd(void *handle, data_package *pkg, UINT8 *hash)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PIC_HASH;
	key.key_value = hash;
	key.value_len = strlen(hash);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonPlcDataAdd(void *handle, data_package *pkg, double data)
{
	return JsonRawDataAdd(handle, pkg, data);
}

UINT32 JsonSlaveIdAdd(void *handle, data_package *pkg, UINT32 slave_id)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_SLAVE_ID;
	key.key_value = (UINT8 *)&slave_id;
	key.value_len = sizeof(slave_id);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}


UINT32 JsonPortAdd(void *handle, data_package *pkg, UINT32 port)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_PORT;
	key.key_value = (UINT8 *)&port;
	key.value_len = sizeof(port);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonHostAdd(void *handle, data_package *pkg, UINT8 *host)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_HOST;
	key.key_value = host;
	key.value_len = strlen(host);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonUserAdd(void *handle, data_package *pkg, UINT8 *user)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_USER;
	key.key_value = user;
	key.value_len = strlen(user);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonPwdAdd(void *handle, data_package *pkg, UINT8 *pwd)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PWD;
	key.key_value = pwd;
	key.value_len = strlen(pwd);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}


UINT32 JsonProtocolAdd(void *handle, data_package *pkg, UINT8 *protocol)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PROTOCOL;
	key.key_value = protocol;
	key.value_len = strlen(protocol);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT8 *JsonPubkeyGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PUBKEY;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonDevIdGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_DEV_ID;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonTimeStampGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT64;
	key.key_str = KEY_STR_TIMESTAMP;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonSignGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_SIGN;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonHumGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_DOUBLE;
	key.key_str = KEY_STR_HUM;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonTempGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_DOUBLE;
	key.key_str = KEY_STR_TEMP;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonNodeIdGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_NODE_ID;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonTypeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_PKG_TYPE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonDevTypeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_DEV_TYPE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}


UINT8 *JsonRfidGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_RFID;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonAtidGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_ATID;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonRssiGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_DOUBLE;
	key.key_str = KEY_STR_RSSI;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonPicGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PIC;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonPicIdGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_PIC_ID;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonPicHashGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PIC_HASH;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonPlcDataGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	return JsonRawDataGet(handle, pkg, len);
}

UINT8 *JsonPortGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_PORT;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonHostGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_HOST;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonUserGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_USER;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonPwdGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PWD;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonProtocolGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PROTOCOL;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonFileGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_FILE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonFileTypeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_FILE_TYPE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 JsonSoftVerAdd(void *handle, data_package *pkg)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;
	VERSION ver;
	UINT8 ver_str[64];

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&ver, 0, sizeof(ver));
	SoftVerGet(&ver);
	memset(ver_str, 0, sizeof(ver_str));
	sprintf(ver_str, "%d.%d.%d-%s", ver.major_ver, 
								  	ver.minor_ver, 
								  	ver.stage_ver, 
								  	ver.build_time);

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_SOFT_VER;
	key.key_value = ver_str;
	key.value_len = strlen(ver_str);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT8 *JsonSoftVerGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_SOFT_VER;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 JsonProcotolVerAdd(void *handle, data_package *pkg)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;
	VERSION ver;
	UINT8 ver_str[64];

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&ver, 0, sizeof(ver));
	ProtocolVerGet(&ver);
	memset(ver_str, 0, sizeof(ver_str));
	sprintf(ver_str, "%d.%d.%d", ver.major_ver, 
								  	ver.minor_ver, 
								  	ver.stage_ver);

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PROTOCOL_VER;
	key.key_value = ver_str;
	key.value_len = strlen(ver_str);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT8 *JsonProcotolVerGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PROTOCOL_VER;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}


UINT8 *JsonSlaveIdGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_SLAVE_ID;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 JsonEvidenceAdd(void *handle, data_package *pkg, UINT8 *data)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}


	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_EVIDENCE;
	key.key_value = data;
	key.value_len = strlen(data);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT8 *JsonRetCodeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_RETURN_CODE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonProofGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PROOF;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 JsonProofAdd(void *handle, data_package *pkg, UINT8 *proof)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;
	
	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}


	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PROOF;
	key.key_value = proof;
	key.value_len = strlen(proof);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonPidAdd(void *handle, data_package *pkg, UINT8 *pid)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PID;
	key.key_value = pid;
	key.value_len = strlen(pid);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}


UINT8 *JsonRawDataGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_DATA;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 JsonRawDataAdd(void *handle, data_package *pkg, double data)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_DOUBLE;
	key.key_str = KEY_STR_DATA;
	key.key_value = (UINT8 *)&data;
	key.value_len = sizeof(data);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s addt data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT8 *JsonOnChainGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_ONCHAIN;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonDomainGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_DOMAIN;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonAccessIdGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_ACCESS_ID;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonAccessKeyGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_ACCESS_KEY;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonTokenGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_TOKEN;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonPrikeyGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PRIKEY;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}


UINT8 *JsonRawTypeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_TYPE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 JsonRawTypeAdd(void *handle, data_package *pkg, UINT8 *type)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_TYPE;
	key.key_value = type;
	key.value_len = strlen(type);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}


UINT8 *JsonCodeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_CODE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonPutFileGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_FILE_PUT;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonGetFileGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_FILE_GET;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonIntervalGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_INTERVAL;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonRuleNameGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_RULE_NAME;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonAlgNameGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_ALG_NAME;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}


UINT8 *JsonSamplesGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_ALG_SAMPLES;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonRangeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_ALG_RANGE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonUrlGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_URL;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT8 *JsonBpIndexGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_ALG_BP_INDEX;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}



UINT8 *JsonCfgTypeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_CFG_TYPE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 JsonCfgTypeAdd(void *handle, data_package *pkg, UINT8 *type)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_CFG_TYPE;
	key.key_value = type;
	key.value_len = strlen(type);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}


UINT32 JsonPrikeyAdd(void *handle, data_package *pkg, UINT8 *prikey)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PRIKEY;
	key.key_value = prikey;
	key.value_len = strlen(prikey);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonRuleNameAdd(void *handle, data_package *pkg, UINT8 *name)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_RULE_NAME;
	key.key_value = name;
	key.value_len = strlen(name);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonIntervalAdd(void *handle, data_package *pkg, UINT32 interval)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_ALG_INTERVAL;
	key.key_value = (UINT8 *)&interval;
	key.value_len = sizeof(interval);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonAlgListAdd(void *handle, data_package *pkg, UINT8 *list)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_ALG_LIST;
	key.key_value = list;
	key.value_len = strlen(list);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT32 JsonResqIdAdd(void *handle, data_package *pkg, UINT64 cfgid)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT64;
	key.key_str = KEY_STR_RESQ_ID;
	key.key_value = (UINT8 *)&cfgid;
	key.value_len = sizeof(cfgid);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT8 *JsonResqIdGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT64;
	key.key_str = KEY_STR_RESQ_ID;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 JsonLogAdd(void *handle, data_package *pkg, UINT8 *log)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_Log;
	key.key_value = log;
	key.value_len = strlen(log);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT8 *JsonPollTimeGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_POLL_TIME;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}


UINT32 JsonPollTimeAdd(void *handle, data_package *pkg, UINT32 poll_time)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_POLL_TIME;
	key.key_value = (UINT8 *)&poll_time;
	key.value_len = sizeof(poll_time);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}

UINT8 *JsonSampleIntGet(void *handle, UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;
	DEV_DRIVER *dev = (DEV_DRIVER *)handle;

	if(handle == NULL || pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_SAMPLE_INTERVAL;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s get data %s from package fail.", 
		  		  dev->device_id, key.key_str);
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}


UINT32 JsonSampleIntAdd(void *handle, data_package *pkg, UINT32 interval)
{
	DEV_DRIVER *dev = NULL;
	CFG_KEY key;
	UINT32 ret = 0;

	if(handle == NULL || pkg == NULL)
	{
		return 0;
	}

	dev = (DEV_DRIVER *)handle;

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_INT;
	key.key_str = KEY_STR_SAMPLE_INTERVAL;
	key.key_value = (UINT8 *)&interval;
	key.value_len = sizeof(interval);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		LOG_PRINT(WX_LOG_WARN, "Device %s add data %s to package fail.", 
		  		  dev->device_id, key.key_str);
		return 0;
	}

	return pkg->data_len;
}



