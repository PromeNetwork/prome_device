

#ifndef __DEV_RULE_H__
#define __DEV_RULE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "bulletproof.h"
#include "package.h"


typedef UINT32 (RULE_EXEC_FUNC) (void *handle);

#define ALG_TYPE_DEFAULT_STR				"unknown"
#define ALG_TYPE_BULLET_PROOF_STR			"bulletproof"
#define ALG_TYPE_AVERAGE_STR				"average"


typedef enum
{
	ALG_TYPE_DEFAULT		= 0,
	ALG_TYPE_BULLET_PROOF 	= 1,
	ALG_TYPE_AVERAGE 		= 2,	
}ALG_TYPE_E;

typedef enum
{
	RULE_SUCCESS				= 0,
	
	RULE_AVE_GET_FAIL			= 1,
	RULE_AVE_ONCHAIN_FAIL		= 2,
	RULE_PROOF_GET_FAIL		= 3,
	RULE_PROOF_ONCHAIN_FAIL	= 4,
	RULE_DEV_NOT_FOUND	= 5,
	RULE_ALG_NOT_FOUND	= 6,
	
}RULE_STATUS;


typedef struct _ALG_PROOF
{
	UINT8		range[64];
	UINT8 		api_url[URL_MAX_LEN];
	UINT32 		ct_index;
	UINT8		proof[PROOF_LENGTH];
	UINT32		proof_len;
	UINT8		pid[HASH_STR_MAX_LEN];
	UINT32		pid_len;
	TIMESTAMP 	stamp;
}ALG_PROOF ,*P_ALG_PROOF; 

typedef struct _ALG_AVE
{
	UINT32  	interval;
	UINT32		samples;
	double		value;
	UINT8	  	hasher[HASH_STR_MAX_LEN];
	UINT32		hasher_len;
	TIMESTAMP 	stamp;
}ALG_AVE ,*P_ALG_AVE; 


typedef struct _ALG_INFO
{
	ALG_TYPE_E	type;
	void 		*alg;
	struct 		_ALG_INFO *next;	
}ALG_INFO ,*P_ALG_INFO;


typedef struct _RULE_INFO
{
	UINT8			rule_name[32];	/*规则名*/
	INT32 			timer;			/*规则执行定时器ID*/
	UINT32  		interval;		/*规则执行周期*/
	ALG_INFO		*alg_list;		/*规则算法列表*/
	struct 			_RULE_INFO *next;	
}RULE_INFO ,*P_RULE_INFO; 

UINT32 DevRuleParse(void *handle, UINT8 *data);
UINT32 FileRuleSave(void *handle);
UINT32 FileRuleDelete(void *handle);
UINT32 DevBulletProof(void *handle, ALG_PROOF *alg, double value);
UINT32 DevAveOnchain(void *handle, ALG_AVE *alg);
void *DevRuleAlgGet(RULE_INFO *rule, ALG_TYPE_E type);
UINT32 DevRuleStart(void *handle);
UINT32 DevRuleStop(void *handle);
ALG_TYPE_E ALG_TYPE_INT(UINT8 *type);
UINT8 *ALG_TYPE_STR(ALG_TYPE_E type);
UINT8 *RULE_STATUS_STR(RULE_STATUS status);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DEV_RULE_H__ */
