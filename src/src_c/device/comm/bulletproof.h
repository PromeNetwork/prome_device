

#ifndef __BULLETPROOF_H__
#define __BULLETPROOF_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "cJSON.h"

#define PROOF_LENGTH		2431
#define BP_CT_CFG	cJSON	

#define VERIFYPROOF_URL				"/api/v1/enterprise/chain/verifyProof"
#define BULLETPROOF_TEMPERATURE_URL	"/bulletproof/temperature"
#define BULLETPROOF_HUMIDITY_URL	"/bulletproof/humidity"

#define BP_LOCAL_LIB_STR		"LocalLib"
#define BP_LOCAL_SERVER_STR		"LocalServer"
#define BP_REMOTE_SERVER_STR	"RemoteServer"



/*设备类型枚举*/
 typedef enum
 {
	 BP_NONE    		= 0,
	 BP_LOCAL_LIB 		= 1,
	 BP_LOCAL_SERVER 	= 2,
	 BP_REMOTE_SERVER 	= 3,
 }BP_TYPE_E;

typedef struct _BP_CT
{
	UINT32 bp_id;
	UINT32 bp_ptid;
	UINT8 bp_ptowner[32];
	UINT8 bp_ptuser[32];
	double bp_mstl;
	double bp_msth;
	UINT32 bp_mstp;
	UINT8 bp_cmgt[32];
}BP_CT ,*P_BP_CT; 

typedef struct _BP_CFG
{
	UINT8 flag;
	BP_TYPE_E type;
	UINT8 domain[DOMAIN_LEN];
	UINT8 token[TOKEN_LEN];				/*区块链存证HTTP token*/
	//BP_CT_CFG *ct_cfg;
	UINT8 *ct_cfg;
}BP_CFG, *P_BP_CFG;


UINT32 BulletProofGet(void *handle, double data, UINT8 *proof, void *para);
UINT32 BulletProofOnchain(void *handle, UINT8 *pid, UINT8 *proof, UINT8 *hasher);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BULLETPROOF_H__ */
