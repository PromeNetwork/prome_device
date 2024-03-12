

#ifndef __PRE_API_H__
#define __PRE_API_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "basic_types.h"


#define PRE_MAX_G1_SIZE 		(64 * 3 + 2)		//194
#define PRE_MAX_GT_SIZE 		(64 * 12 + 11)		//779
#define PRE_MAX_SK_SIZE 		(64 * 1)			//64
#define PRE_MAX_PK1_SIZE 		(64 * 12 + 11)		//779
#define PRE_MAX_PK2_SIZE 		(64 * 3 + 2)		//194
#define PRE_MAX_RK_SIZE 		(64 * 3 + 2)		//194
#define PRE_MAX_CIPHER_LEVEL1_1 (64 * 3 * 2 + 5)	//389
#define PRE_MAX_CIPHER_LEVEL1_2 (64 * 12 + 11)		//779
#define PRE_MAX_CIPHER_LEVEL2 	(64 * 12 + 11)		//779


typedef struct _PRE_SYMMETRIC_KEY
{
	UINT8 		m[PRE_MAX_SK_SIZE + 1];
	UINT8 		mGT[PRE_MAX_GT_SIZE + 1];
}PRE_SYMMETRIC_KEY;

typedef struct _PRE_USER_KEY
{
	UINT8 		secret_key1[PRE_MAX_SK_SIZE + 1];
	UINT8 		secret_key2[PRE_MAX_SK_SIZE + 1];
	UINT8 		public_key1[PRE_MAX_PK1_SIZE + 1];
	UINT8 		public_key2[PRE_MAX_PK2_SIZE + 1];
}PRE_USER_KEY;

typedef struct _PRE_REENCRYPT_KEY
{
	UINT8 		reencrypt_key[PRE_MAX_RK_SIZE + 1];
}PRE_REENCRYPT_KEY;

typedef struct _PRE_CIPHER_TEXT
{
	UINT8 		cipher_text1[PRE_MAX_CIPHER_LEVEL2 + 1];
	UINT8 		cipher_text2[PRE_MAX_CIPHER_LEVEL2 + 1];
}PRE_CIPHER_TEXT;



typedef PRE_USER_KEY	PRE_SENDER_KEY;
typedef PRE_USER_KEY	PRE_RECEIVER_KEY;

UINT32 PreInit(void);

UINT32 PreDecryption( PRE_SYMMETRIC_KEY *msg, 
 				  	  PRE_CIPHER_TEXT *reencrypt_cipher, 
				  	  PRE_RECEIVER_KEY *receiver_key);
UINT32 PreLevel1CipherGenerate(	PRE_CIPHER_TEXT *level1_cipher, 
								PRE_SENDER_KEY *sender_key, 
								PRE_SYMMETRIC_KEY *msg);
UINT32 PreLocalCipherGenerate(PRE_CIPHER_TEXT *local_cipher, 
 							  PRE_SENDER_KEY *sender_key, 
							  PRE_SYMMETRIC_KEY *msg);
UINT32 PreReEncryptCipherGenerate(PRE_CIPHER_TEXT *reencrypt_cipher, 
 							   	  PRE_REENCRYPT_KEY *reencrypt_key, 
 							      PRE_CIPHER_TEXT *level1_cipher, 
							      PRE_SENDER_KEY *sender_key);
UINT32 PreReEncryptKeyGenerate(PRE_REENCRYPT_KEY *re_key, 
 							   PRE_RECEIVER_KEY *receiver_key,
							   PRE_SENDER_KEY *sender_key);
UINT32 PreSymmetricKeyGenerate(UINT8 *seed, PRE_SYMMETRIC_KEY *s_key);
UINT32 PreUserKeyGenerate(PRE_USER_KEY *user_key);
UINT32 PreUserKey_Get(char *file ,PRE_USER_KEY *user_key);
UINT32 PreUserKey_Set(char *file ,PRE_USER_KEY *user_key);

void *PreProxyInit(void *para);
void *PreReceiverInit(void *para);
void *PreSenderInit(void *para);
void PreReceiverSendReq(UINT8 *sender_pbkey);
void *Key_Update(void *para);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PRE_API_H__ */
