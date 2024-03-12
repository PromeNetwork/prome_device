

#ifndef __BLOCKCHAIN_H__
#define __BLOCKCHAIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define CHAIN_ON_CHAIN_URL		"/api/v1/enterprise/chain/setEvidenceData"
#define CHAIN_SAMURAI_STR		"samurai"
#define CHAIN_BCOP_STR			"bcop"
#define CHAIN_NONE_STR			"none"

typedef enum
{
	ONCHAIN_NO	= 0,
	ONCHAIN_YES = 1,
}ON_CHAIN;


/*设备类型枚举*/
typedef enum
{
	CHAIN_NONE	  = 0,
	CHAIN_SAMURAI = 1,
	CHAIN_BCOP	  = 2,
}CHAIN_TYPE_E;

typedef struct _CHAIN_CFG
{
	UINT8 flag;
	ON_CHAIN data_onchain;
	CHAIN_TYPE_E type;
	UINT8 domain[DOMAIN_LEN];					/*区块链存证服务地址*/
	UINT8 access_id[ACCESS_KEY_LEN];			/*samurai存证接入身份ID*/
	UINT8 access_key[ACCESS_KEY_LEN];			/*samurai存证接入秘钥*/
	UINT8 token[TOKEN_LEN];					/*区块链存证HTTP token*/
}CHAIN_CFG, *P_CHAIN_CFG;


UINT32 DataOnchain(void *handle, UINT8 *data, UINT32 data_len, UINT8 *hasher);
CHAIN_TYPE_E CHAIN_TYPE_INT(UINT8 *type);
UINT8 *CHAIN_TYPE_STR(CHAIN_TYPE_E type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BLOCKCHAIN_H__ */
