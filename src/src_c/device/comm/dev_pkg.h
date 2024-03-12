

#ifndef __DEV_PKG_H__
#define __DEV_PKG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "dev_comm.h"
#include "dev_rule.h"

#define PKG_TYPE_DEV_ONLINE_STR				"connect"
#define PKG_TYPE_DATA_REPORT_STR			"report"
#define PKG_TYPE_DEV_CFG_STR				"config"
#define PKG_TYPE_RULE_RESULT_STR			"result"
#define PKG_TYPE_DEV_OFFLINE_STR			"disconnect"
#define PKG_TYPE_DEV_REQUEST_STR			"request"
#define PKG_TYPE_DEV_ATTRIBUTE_STR			"attribute"
#define PKG_TYPE_DEV_REPONSE_STR			"response"
//#define PKG_TYPE_DEV_UPGRADE_STR			"upgrade"
#define PKG_TYPE_DEV_TIME_STR				"time"
#define PKG_TYPE_DEV_RPC_MQTT_RESP_STR		"rpc mqtt response"
#define PKG_TYPE_DEV_RPC_UPGRADE_RESP_STR	"rpc upgrade response"
#define PKG_TYPE_DEV_RPC_DEV_INFO_RESP_STR	"rpc devinfo response"




/*报文类型枚举*/
typedef enum
{
	PKG_TYPE_DEV_ONLINE 			= 1,
	PKG_TYPE_DATA_REPORT			= 2,
	PKG_TYPE_DEV_CFG				= 3,
	PKG_TYPE_RULE_RESULT			= 4,
	PKG_TYPE_DEV_OFFLINE			= 5,
	PKG_TYPE_DEV_REQUEST			= 6,
	PKG_TYPE_DEV_ATTRIBUTE			= 7,
	PKG_TYPE_DEV_RULE				= 8,
	PKG_TYPE_DEV_RESPONSE			= 9,
	//PKG_TYPE_DEV_UPGRADE			= 10,
	PKG_TYPE_DEV_TIME				= 11,
	PKG_TYPE_DEV_RPC_MQTT_RESP		= 12,
	PKG_TYPE_DEV_RPC_UPGRADE_RESP	= 13,
	PKG_TYPE_DEV_RPC_DEV_INFO_RESP	= 14,
	PKG_TYPE_UNKOWN
}PKG_TYPE_E;


UINT32 DevicePkgCfgReq(void *handle);
UINT32 DevicePkgCfgRes(void *handle, UINT8 *data, UINT32 len, 
					   DEVICE_STATUS status);
UINT32 DevicePkgOnline(void *handle);
UINT32 DevicePkgReport(void *handle, void *data);
UINT32 DevicePkgReportError(void *handle, void *data);
//UINT32 DevicePkgRuleResult(void *handle, RULE_STATUS status);
UINT32 DevicePkgTimeReq(void *handle);
//UINT32 DevicePkgUpgradeReq(void *handle);
//UINT32 DevicePkgUpgradeRes(void *handle, UINT8 *data, UINT32 len, 
//					   	   DEVICE_STATUS status);
UINT8 *PKG_TYPE_STR(PKG_TYPE_E type);
UINT32 DevicePkgRpcResp(void *handle, 
						UINT8 *data, 
						UINT32 len, 
						TIMESTAMP request_id,
						PKG_TYPE_E pkg_type);
UINT32 DevicePkgRuleResult(void *handle, RULE_STATUS status);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DEV_PKG_H__ */
