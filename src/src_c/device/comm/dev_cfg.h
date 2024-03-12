

#ifndef __DEV_CFG_H__
#define __DEV_CFG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "dev_rule.h"

#define CFG_TYPE_MQTT_STR		"mqtt"
#define CFG_TYPE_BP_CT_STR 		"bpct"
#define CFG_TYPE_BP_CFG_STR		"bpcfg"
#define CFG_TYPE_UPGRADE_STR 	"upgrade"
#define CFG_TYPE_CHAIN_STR		"chain"
#define CFG_TYPE_RULE_STR		"rule"
#define CFG_TYPE_ADD_DEV_STR	"adddev"
#define CFG_TYPE_FILE_STR		"file"


typedef enum
{
	CFG_TYPE_DEFALUT	= 0,
	CFG_TYPE_MQTT 		= 1,
	CFG_TYPE_BP_CT 		= 2,	
	CFG_TYPE_BP_CFG		= 3,	
	CFG_TYPE_UPGRADE 	= 4,
	CFG_TYPE_CHAIN		= 5,
	CFG_TYPE_RULE		= 6,
	CFG_TYPE_ADD_DEV	= 7,
	CFG_TYPE_FILE		= 8,
}CFG_TYPE_E;


CFG_TYPE_E CFG_TYPE_INT(UINT8 *type);
UINT8 *CFG_TYPE_STR(CFG_TYPE_E type);
UINT32 DeviceCfgIdParse(void *handle, UINT8 *data, TIMESTAMP *cfgid);
UINT32 DeviceCfgIdSave(void *handle, UINT8 *file);
//INT32 DeviceCfgListParse(void *handle, UINT8 *data);
//UINT32 DeviceCfgListSave(void *handle, UINT8 *file);
UINT32 DeviceCfgKeyRead(void *handle, UINT8 *file);
UINT32 DeviceMqttCfgSave(void *handle, UINT8 *file);
INT32 DeviceMqttCfgParse(void *handle, UINT8 *package, UINT8 need_check);
INT32 DeviceMqttCfgInit(void *handle, UINT8 *package);
UINT32 DeviceCfgFileServerSave(void *handle, UINT8 *file);
INT32 DeviceCfgFileServerParse(void *handle, UINT8 *package);
INT32 DeviceCfgChainParse(void *handle, UINT8 *package);
UINT32 DeviceCfgChainSave(void *handle, UINT8 *file);
INT32 DeviceCfgRuleParse(void *handle, UINT8 *package, RULE_INFO *rule_new);
UINT32 DeviceCfgRuleSave(void *handle, UINT8 *file);
UINT32 DeviceCfgRuleDelete(void *handle);
INT32 DeviceCfgBpParse(void *handle, UINT8 *package);
UINT32 DeviceCfgBpSave(void *handle, UINT8 *file);
UINT32 DeviceCfgSaveFormatted(UINT8 *file, UINT8 *filebuff);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DEV_CFG_H__ */
