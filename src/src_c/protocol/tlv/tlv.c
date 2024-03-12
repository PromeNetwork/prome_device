#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


#include "tlv.h"
#include "hexutils.h"


#define TLV_VALUE_MAX_LENGTH	256


typedef struct tlv_header_t {
	UINT16   	type;
	UINT16 		length;
}__packed TLV_header;

typedef struct tlv_struct{
	TLV_header header;
	UINT8 value[TLV_VALUE_MAX_LENGTH];
}__packed TLV_t;


UINT32 add_tlv(TLV_package *package, TLV_TYPE_E TLV_TYPE, UINT32 value_len, UINT8* value)
{
	int now_data_len = 0;
	TLV_t new_tlv;

	if(package == NULL || value == NULL)
	{
		return -1;
	}

	/*判断数据包中是否有足够空间存放TLV*/
	if(package->header.len >= TLV_MESSAGE_MAX_LENGTH || value_len > TLV_VALUE_MAX_LENGTH)
	{
		return -1;
	}

	if((package->header.len + sizeof(TLV_header) + value_len) > TLV_MESSAGE_MAX_LENGTH)
	{
		return -1;
	}

	if(package->header.len == 0)
	{
		now_data_len = 0;
		package->header.len = sizeof(package_header);
	}
	else
	{
		now_data_len = package->header.len - sizeof(package_header);
	}
	memset(&new_tlv, 0, sizeof(new_tlv));
	new_tlv.header.type = htons(TLV_TYPE);
	new_tlv.header.length = htons(value_len);
	memcpy(new_tlv.value, value, value_len);

	memcpy(package->data + now_data_len, &new_tlv, sizeof(TLV_header) + value_len);
	package->header.len = package->header.len + sizeof(TLV_header) + value_len;

	return 0;
}


void free_tlv(TLV_package *package)
{

	return;
}

UINT8 *get_tlv(TLV_package *package, TLV_TYPE_E type, UINT32 *len)
{
	UINT16 data_len = 0;
	TLV_t *new_tlv = NULL;
	UINT8 *tmp = NULL;
	UINT16		type_tmp;
	UINT16		length;
	UINT8 *data = NULL;

	if(package == NULL || len == NULL)
	{
		return NULL;
	}
	
	data_len = ntohs(package->header.len) - sizeof(package_header);
	tmp = package->data;
	while(data_len)
	{
		new_tlv = (TLV_t *)tmp;
		type_tmp = ntohs(new_tlv->header.type);
		length = ntohs(new_tlv->header.length);
		
		/*当TLV中length字段的值大于剩余数据实际长度时，应报错*/
		if(length > (data_len - sizeof(TLV_header)))
		{
			return NULL;
		}
		if(type_tmp == type)
		{
			data = (UINT8 *)malloc(length + 1);
			if(data == NULL)
			{
				return NULL;
			}
			memset(data, 0, length + 1);
			memcpy(data, new_tlv->value, length);
			*len = length;
			break;
		}		
		tmp = tmp + sizeof(TLV_header) + length;
		data_len = data_len - sizeof(TLV_header) - length;
	}
	return data;
}

