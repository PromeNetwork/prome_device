#ifndef TLV_H
#define TLV_H
#ifdef	__cplusplus
extern "C" {
#endif

#include "basic_types.h"

#define TLV_MESSAGE_MAX_LENGTH		1000


#define __packed __attribute__ ((packed))

typedef enum
{
	TLV_TYPE_RFID_PKG		= 0x100,
	TLV_TYPE_RFID_DEVID		= 0x101,
	TLV_TYPE_RFID_ID		= 0x102,
	TLV_TYPE_RFID_ATID		= 0x103,
	TLV_TYPE_RFID_RSSI 		= 0x104,

	TLV_TYPE_HT_SENSOR		= 0x200,
	TLV_TYPE_HT_DEVID		= 0x201,
	TLV_TYPE_HT_NODEID		= 0x202,
	TLV_TYPE_HT_TEMP		= 0x203,
	TLV_TYPE_HT_HUMIDITY	= 0x204,
	TLV_TYPE_HT_LONGITUDE	= 0x205,
	TLV_TYPE_HT_LATITUDE	= 0x206,
	TLV_TYPE_HT_CD_TYPE		= 0x207,
	TLV_TYPE_HT_RELAY_STATUS= 0x208,


	
	TLV_TYPE_SIGN_MSG	= 0x1001,
	TLV_TYPE_POE_HASH	= 0x1002
}TLV_TYPE_E;

/*报文头部结构体*/
typedef struct package_header_t
{
	UINT16 len;				/*整个数据包的大小，包含头部和数据部分*/
	UINT16 type;			/*报文类型*/
}__packed package_header;

typedef struct package_struct{
	package_header header;
	UINT8 data[TLV_MESSAGE_MAX_LENGTH-sizeof(package_header)];
}__packed TLV_package;


UINT32 add_tlv(TLV_package *package, TLV_TYPE_E TLV_TYPE, UINT32 value_len, UINT8* value);
UINT8 *get_tlv(TLV_package *package, TLV_TYPE_E type, UINT32 *len);
void free_tlv(TLV_package *package);

#ifdef	__cplusplus
}
#endif

#endif


