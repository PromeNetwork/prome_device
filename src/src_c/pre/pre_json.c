#include <string.h>
#include <stdlib.h>

#include "basic_types.h"
#include "log.h"

#include "cJSON.h"
#include "hexutils.h"
#include "package.h"
#include "pre_json.h"


UINT8 *PreRecvPublicKeyGet(UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PUBKEY_RECV;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 PreRecvPublicKeyAdd(data_package *pkg, UINT8 *pb_key)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return 0;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PUBKEY_RECV;
	key.key_value = pb_key;
	key.value_len = strlen(pb_key);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		return 0;
	}

	return pkg->data_len;
}

UINT8 *PreSendPublicKeyGet(UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PUBKEY_SEND;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 PreSendPublicKeyAdd(data_package *pkg, UINT8 *pb_key)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return 0;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PUBKEY_SEND;
	key.key_value = pb_key;
	key.value_len = strlen(pb_key);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		return 0;
	}

	return pkg->data_len;
}

UINT8 *PreProxyPublicKeyGet(UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PUBKEY_PROXY;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

UINT32 PreProxyPublicKeyAdd(data_package *pkg, UINT8 *pb_key)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return 0;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_PUBKEY_PROXY;
	key.key_value = pb_key;
	key.value_len = strlen(pb_key);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		return 0;
	}

	return pkg->data_len;
}




UINT32 PreReencryptKeyAdd(data_package *pkg, UINT8 *pb_key)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return 0;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_REENCRYPT_KEY;
	key.key_value = pb_key;
	key.value_len = strlen(pb_key);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		return 0;
	}

	return pkg->data_len;
}



UINT8 *PreReencryptKeyGet(UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_REENCRYPT_KEY;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

INT32 PreCipherText1Add(data_package *pkg, UINT8 *pb_key)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return 0;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_CIPHER_TEXT1;
	key.key_value = pb_key;
	key.value_len = strlen(pb_key);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		return 0;
	}

	return pkg->data_len;
}



UINT8 *PreCipherText1Get(UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_CIPHER_TEXT1;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

INT32 PreCipherText2Add(data_package *pkg, UINT8 *pb_key)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return 0;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_CIPHER_TEXT2;
	key.key_value = pb_key;
	key.value_len = strlen(pb_key);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		return 0;
	}

	return pkg->data_len;
}



UINT8 *PreCipherText2Get(UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_CIPHER_TEXT2;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

INT32 PreRoleAdd(data_package *pkg, UINT8 *pb_key)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return 0;
	}

	memset(&key, 0, sizeof(key));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_ROLE;
	key.key_value = pb_key;
	key.value_len = strlen(pb_key);
	ret = PackageJsonDataAdd(pkg, &key);
	if(ret == 0)
	{
		return 0;
	}

	return pkg->data_len;
}



UINT8 *PreRoleGet(UINT8 *pkg, UINT32 *len)
{
	CFG_KEY key;
	UINT32 ret = 0;

	if(pkg == NULL)
	{
		return NULL;
	}

	memset(&key, 0, sizeof(CFG_KEY));
	key.json_type = JSON_TYPE_STRING;
	key.key_str = KEY_STR_ROLE;

	ret = PackageJsonDataGet(pkg, &key);
	if(ret)
	{
		return NULL;
	}
	*len = key.value_len;

	return key.key_value;
}

void PrePackageFormattPrint(UINT8 *data)
{
	cJSON *root = NULL;
	UINT8 *json_str = NULL;

	root = cJSON_Parse((char *)data);
	if(root == NULL)
	{
		return ;
	}

	json_str = (UINT8 *)cJSON_Print(root);	
	if(json_str == NULL)
	{
		cJSON_Delete(root); 
		return ;
	}

	printf("%s\r\n", json_str);
	SAFE_FREE(json_str);
	return ;
}


