

#ifndef __PRE_JSON_H__
#define __PRE_JSON_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"
#include "package.h"


#define KEY_STR_REENCRYPT_KEY		"ReEncryptKey"
#define KEY_STR_CIPHER_TEXT1		"CipherText1"
#define KEY_STR_CIPHER_TEXT2		"CipherText2"
#define KEY_STR_PUBKEY_RECV			"ReceiverPubkey"
#define KEY_STR_PUBKEY_SEND			"SenderPubkey"
#define KEY_STR_PUBKEY_PROXY		"ProxyPubkey"
#define KEY_STR_ROLE				"Role"

INT32 PreCipherText1Add(data_package *pkg, UINT8 *pb_key);
UINT8 *PreCipherText1Get(UINT8 *pkg, UINT32 *len);
INT32 PreCipherText2Add(data_package *pkg, UINT8 *pb_key);
UINT8 *PreCipherText2Get(UINT8 *pkg, UINT32 *len);
UINT32 PreProxyPublicKeyAdd(data_package *pkg, UINT8 *pb_key);
UINT8 *PreProxyPublicKeyGet(UINT8 *pkg, UINT32 *len);
UINT32 PreRecvPublicKeyAdd(data_package *pkg, UINT8 *pb_key);
UINT8 *PreRecvPublicKeyGet(UINT8 *pkg, UINT32 *len);
UINT32 PreReencryptKeyAdd(data_package *pkg, UINT8 *pb_key);
UINT8 *PreReencryptKeyGet(UINT8 *pkg, UINT32 *len);
INT32 PreRoleAdd(data_package *pkg, UINT8 *pb_key);
UINT8 *PreRoleGet(UINT8 *pkg, UINT32 *len);
UINT32 PreSendPublicKeyAdd(data_package *pkg, UINT8 *pb_key);
UINT8 *PreSendPublicKeyGet(UINT8 *pkg, UINT32 *len);
void PrePackageFormattPrint(UINT8 *data);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PRE_JSON_H__ */
