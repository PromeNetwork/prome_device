

#ifndef __DEV_JSON_H__
#define __DEV_JSON_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "dev_comm.h"
#include "dev_pkg.h"

#include "package.h"
#include "version.h"


#define KEY_ID(dev, type, index)	(dev*0x1000 + type*0x100 + index)

#define KEY_STR_PROTOCOL		"Protocol"
#define KEY_STR_HOST			"Host"
#define KEY_STR_PORT			"Port"
#define KEY_STR_USER			"User"
#define KEY_STR_PWD				"Pwd"
#define KEY_STR_FILE			"File"
#define KEY_STR_FILE_TYPE		"FileType"

#define KEY_STR_DEV_ID			"DeviceId"
#define KEY_STR_SIGN			"Sign"
#define KEY_STR_POEHASH			"PoEHash"
#define KEY_STR_TIMESTAMP		"TimeStamp"
#define KEY_STR_PUBKEY			"PubKey"
#define KEY_STR_PRIKEY			"PriKey"
#define KEY_STR_DEV_TYPE		"DeviceType"
#define KEY_STR_PKG_TYPE		"PkgType"
#define KEY_STR_CFG_TYPE		"CfgType"
#define KEY_STR_CFG_NUM			"CfgNum"
#define KEY_STR_CFG_LIST		"CfgList"
#define KEY_STR_BP_LIST			"BpList"
#define KEY_STR_ALG_LIST		"AlgList"
#define KEY_STR_ADDR_LIST		"AddrList"
#define KEY_STR_DEV_LIST 		"DevList"
#define KEY_STR_SLAVE_ID		"SlaveId"
#define KEY_STR_CFG_MQTT		"CfgMqtt"
#define KEY_STR_CFG_FILE_SERVER	"CfgFileServer"
#define KEY_STR_CFG_CHAIN		"CfgChain"
#define KEY_STR_CFG_BP			"CfgBp"
#define KEY_STR_CFG_RULE		"CfgRule"





#define KEY_STR_ALG_NAME				"AlgName"
#define KEY_STR_ALG_INTERVAL			"TimeInterval"
#define KEY_STR_ALG_SAMPLES				"Samples"
#define KEY_STR_URL						"Url"
#define KEY_STR_ALG_VALUE				"Value"
#define KEY_STR_ALG_PROOF				"Proof"
#define KEY_STR_ALG_BPID				"Pid"
#define KEY_STR_ALG_RANGE				"Range"
#define KEY_STR_ALG_BP_INDEX			"BpIndex"
#define KEY_STR_RULE_NAME				"RuleName"

#define KEY_STR_NODE_ID		"NodeId"
#define KEY_STR_TEMP		"Temperature"
#define KEY_STR_HUM			"Humidtity"
#define KEY_STR_LNG			"Longitude"
#define KEY_STR_LAT			"Latiude"


#define KEY_STR_BP_ID		"ID"
#define KEY_STR_BP_PTID		"PTID"
#define KEY_STR_BP_PTOWNER	"PTOwner"
#define KEY_STR_BP_USER		"PTUser"
#define KEY_STR_BP_MSTL		"mSTL"
#define KEY_STR_BP_MSTH		"mSTH"
#define KEY_STR_BP_MSTP		"mSTP"
#define KEY_STR_BP_CMGT		"cmGT"

#define KEY_STR_TYPE		"Type"
#define KEY_STR_ACCESS_ID	"AccessId"
#define KEY_STR_ACCESS_KEY	"AccessKey"
#define KEY_STR_DOMAIN		"Domain"
#define KEY_STR_TOKEN		"Token"
#define KEY_STR_ADDRESS		"Address"

#define KEY_STR_FILE_PUT	"FilePut"
#define KEY_STR_FILE_GET	"FileGet"
#define KEY_STR_INTERVAL	KEY_STR_ALG_INTERVAL
#define KEY_STR_ONCHAIN		"OnChain"


#define KEY_STR_PROOF		"proof"
#define KEY_STR_CODE		"Code"
#define KEY_STR_PID			"pid"
#define KEY_STR_EVIDENCE	"evidenceValue"


//#define KEY_STR_MQTT_HOST		"MqttHost"
//#define KEY_STR_MQTT_PORT		"MqttPort"
//#define KEY_STR_MQTT_USER		"MqttUser"
//#define KEY_STR_MQTT_PWD		"MqttPwd"
#define KEY_STR_CFG_DEV			"CfgDev"


#define KEY_STR_DEV_STATUS	"ExecCode"
#define KEY_STR_DEV_DESC	"ExecDesc"

#define KEY_STR_RETURN_CODE		"returnCode"
#define KEY_STR_RETURN_DESC		"returnDesc"
#define KEY_STR_DATA				"data"

#define KEY_STR_RFID	"Rfid"
#define KEY_STR_ATID	"AntennaId"
#define KEY_STR_RSSI	"Rssi"

#define KEY_STR_PIC				"Pic"
#define KEY_STR_PIC_ID			"PicId"
#define KEY_STR_PIC_HASH		"PicHash"
#define KEY_STR_SOFT_VER		"SoftVer"
#define KEY_STR_PROTOCOL_VER	"ProtocolVer"

#define KEY_STR_RESQ_ID			"RequestId"
#define KEY_STR_Log				"Log"
#define KEY_STR_POLL_TIME		"PollTime"
#define KEY_STR_SAMPLE_INTERVAL	"SampleInterval"



#define RULE_DEFAULT_NAME			"default"
#define RULE_DEFAULT_INTERVAL		30


UINT8 *JsonAccessIdGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonAccessKeyGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonAlgListAdd(void *handle, data_package *pkg, UINT8 *list);
UINT8 *JsonAlgNameGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonAtidAdd(void *handle, data_package *pkg, UINT32 atid);
UINT8 *JsonAtidGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonBpIndexGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonCfgTypeAdd(void *handle, data_package *pkg, UINT8 *type);
UINT8 *JsonCfgTypeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonCodeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonDescAdd(void *handle, data_package *pkg, UINT8 *desc);
UINT32 JsonDevIdAdd(void *handle, data_package *pkg);
UINT8 *JsonDevIdGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonDevStatusAdd(void *handle, data_package *pkg, DEVICE_STATUS status);
UINT32 JsonDevTypeAdd(void *handle, data_package *pkg, DEV_TYPE_E dev_type);
UINT8 *JsonDevTypeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonDomainGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonEvidenceAdd(void *handle, data_package *pkg, UINT8 *data);
UINT8 *JsonFileGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonFileTypeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonGetFileGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonHashAdd(void *handle, data_package *pkg, UINT8 *hash);
UINT32 JsonHostAdd(void *handle, data_package *pkg, UINT8 *host);
UINT8 *JsonHostGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonHumAdd(void *handle, data_package *pkg, double hum);
UINT8 *JsonHumGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonIntervalAdd(void *handle, data_package *pkg, UINT32 interval);
UINT8 *JsonIntervalGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonNodeIdAdd(void *handle, data_package *pkg, UINT32 id);
UINT8 *JsonNodeIdGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonOnChainGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPicAdd(void *handle, data_package *pkg, UINT8 *pic);
UINT8 *JsonPicGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPicHashAdd(void *handle, data_package *pkg, UINT8 *hash);
UINT8 *JsonPicHashGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPicIdAdd(void *handle, data_package *pkg, UINT32 pic_id);
UINT8 *JsonPicIdGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPidAdd(void *handle, data_package *pkg, UINT8 *pid);
UINT32 JsonPkgTypeAdd(void *handle, data_package *pkg, PKG_TYPE_E pkgtype);
UINT32 JsonPlcDataAdd(void *handle, data_package *pkg, double data);
UINT8 *JsonPlcDataGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPortAdd(void *handle, data_package *pkg, UINT32 port);
UINT8 *JsonPortGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPrikeyAdd(void *handle, data_package *pkg, UINT8 *prikey);
UINT8 *JsonPrikeyGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonProcotolVerAdd(void *handle, data_package *pkg);
UINT8 *JsonProcotolVerGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonProofAdd(void *handle, data_package *pkg, UINT8 *proof);
UINT8 *JsonProofGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonProtocolAdd(void *handle, data_package *pkg, UINT8 *protocol);
UINT8 *JsonProtocolGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPubkeyAdd(void *handle, data_package *pkg);
UINT8 *JsonPubkeyGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonPutFileGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPwdAdd(void *handle, data_package *pkg, UINT8 *pwd);
UINT8 *JsonPwdGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonRangeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonRawDataAdd(void *handle, data_package *pkg, double data);
UINT8 *JsonRawDataGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonRawTypeAdd(void *handle, data_package *pkg, UINT8 *type);
UINT8 *JsonRawTypeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonRetCodeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonRfidAdd(void *handle, data_package *pkg, UINT8 *rfid);
UINT8 *JsonRfidGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonRssiAdd(void *handle, data_package *pkg, double rssi);
UINT8 *JsonRssiGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonRuleNameAdd(void *handle, data_package *pkg, UINT8 *name);
UINT8 *JsonRuleNameGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonSamplesGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonSignAdd(void *handle, data_package *pkg, UINT8 *sign);
UINT8 *JsonSignGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonSlaveIdAdd(void *handle, data_package *pkg, UINT32 slave_id);
UINT8 *JsonSlaveIdGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonSoftVerAdd(void *handle, data_package *pkg);
UINT8 *JsonSoftVerGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonTempAdd(void *handle, data_package *pkg, double temp);
UINT8 *JsonTempGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonTimeStampAdd(void *handle, data_package *pkg, TIMESTAMP stamp);
UINT8 *JsonTimeStampGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonTokenGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonTypeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT8 *JsonUrlGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonUserAdd(void *handle, data_package *pkg, UINT8 *user);
UINT8 *JsonUserGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonResqIdAdd(void *handle, data_package *pkg, UINT64 cfgid);
UINT8 *JsonResqIdGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonLogAdd(void *handle, data_package *pkg, UINT8 *log);
UINT8 *JsonSampleIntGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonSampleIntAdd(void *handle, data_package *pkg, UINT32 interval);
UINT8 *JsonPollTimeGet(void *handle, UINT8 *pkg, UINT32 *len);
UINT32 JsonPollTimeAdd(void *handle, data_package *pkg, UINT32 poll_time);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DEV_JSON_H__ */
