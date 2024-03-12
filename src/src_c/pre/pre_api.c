#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <mcl/bn_c256.h>
#include <openssl/sha.h>

#include "pre_api.h"
#include "hexutils.h"
#include "config.h"

#if 0
typedef unsigned char   	UINT8;
typedef unsigned int  		UINT32;
typedef signed char 		INT8;

void HexToStr(char *pszDest, unsigned char *pbSrc, int nLen)
{
	char	ddl, ddh;
	int i = 0;
	
	for (i = 0; i < nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pszDest[i * 2] = tolower(ddh);
		pszDest[i * 2 + 1] = tolower(ddl);		
	}

	pszDest[nLen * 2] = '\0';
}
#endif

typedef struct{
    mclBnG1 g1;
    mclBnG2 g2;
    mclBnGT e;
}curve_param;


typedef struct {
	mclBnFr a,b;
}secretKey;

typedef struct {
    mclBnGT pk1;
    mclBnG1 pk2;
}publicKey;

//keypair
// pk1 = gt^a, pk2 = g1^b
typedef struct {
    secretKey sk;
    publicKey pk;
}keyPair;

// ciphertext_1;
// c1 = g2^k, c2 = m*z^{ak}
typedef struct {
    mclBnG2 c1;
    mclBnGT c2;
}ciphertext_1;

// ciphertext_2; sender remain for self
// c1 = z^{bk}, c2 = m*z^k 
//reciphertex
// c1 = z^[bj*ai*(k+k')], c2 = m*z^[ai*(k+k')]
typedef struct {
    mclBnGT c1;
    mclBnGT c2;   
}ciphertext_2;

curve_param param;

UINT32 PreInit(void)
{
    int ret = 0;

	ret = mclBn_init(MCL_BN254, MCLBN_COMPILED_TIME_VAR);
    if (ret != 0)
    {
        printf("err ret=%d\n", ret);
		return 1;
    }
	memset(&param, 0, sizeof(param));
#if 1
	uint8_t g1[32*3]={	0x59,0xda,0x18,0x58,0x34,0x36,0x51,0x9c,0x1d,0xc2,0xab,0x45,0x39,0x33,0xa3,0x0c,
		 				0x90,0xd9,0x56,0x7f,0x22,0xde,0x96,0x25,0x83,0x67,0x26,0xa7,0xad,0x6c,0xeb,0x0b,
		 				0xbd,0x7e,0x08,0x5b,0x73,0x52,0x9c,0x38,0xb2,0x82,0x13,0x93,0xa6,0x0b,0x9a,0xb1,
		 				0x88,0xf5,0x84,0xfb,0x1d,0xbc,0x5b,0xa1,0x97,0xc9,0x56,0xaf,0x1b,0x10,0xdf,0x08,
		 				0x8e,0xff,0xff,0xff,0xff,0xff,0xff,0x15,0x8a,0xff,0xff,0xff,0xff,0xff,0x39,0xb9,
		 				0xcd,0xff,0xff,0xff,0xff,0x2e,0xc6,0xa2,0xf5,0xff,0xff,0x7f,0xf2,0xa4,0x2b,0x21
		};
	uint8_t g2[32*2*3]={0xc5,0x48,0xf1,0x9a,0xd1,0xb2,0xf5,0xf8,0x0f,0x7d,0xb6,0x27,0xb9,0x3a,0x0a,0xd5,
  						0x46,0xb1,0x54,0x3f,0x94,0xe6,0x09,0x03,0xff,0xd1,0xd6,0x7b,0x01,0x6b,0x7d,0x15,
						0xdb,0x1e,0xd8,0x20,0x9a,0x4d,0xf6,0x60,0x18,0xef,0x46,0x5d,0x81,0x04,0x6c,0x62,
						0xbb,0xf4,0xe7,0x8c,0xaf,0x9d,0xf8,0x9a,0x9f,0xad,0x80,0x75,0x88,0x52,0x85,0x06,
						0x94,0x60,0xfe,0x45,0x2b,0x6e,0xa1,0xa8,0xd8,0x08,0xe2,0x3d,0x9f,0x72,0x4a,0x7e,
						0xa1,0xdd,0xc8,0x88,0xf4,0x57,0xf5,0x30,0x67,0xb2,0x27,0xf8,0xd0,0x0b,0x7f,0x20,
						0x35,0xf0,0x86,0xa0,0x64,0xfa,0x92,0x2e,0xe5,0xb2,0xaa,0x76,0x41,0x89,0x7e,0xd9,
						0x61,0x14,0x7d,0x5d,0xc1,0xb7,0xef,0xd6,0xc0,0x9c,0x0b,0xb2,0x86,0x17,0xff,0x0e,
						0x49,0x86,0xd8,0xa4,0xeb,0x03,0x27,0xa7,0x54,0x34,0xb9,0x36,0x2c,0x28,0x10,0x93,
						0x21,0xe8,0xa2,0x71,0x9f,0x23,0xcc,0x04,0x7d,0x11,0x67,0xd9,0x14,0xa5,0xb0,0x0a,
						0x40,0x8e,0x27,0x7f,0xad,0xa6,0x35,0x28,0x79,0x27,0x9c,0xc5,0x98,0x2d,0xf6,0xb1,
						0xd8,0x15,0x37,0x40,0x88,0x1d,0xe4,0xb1,0xe9,0x56,0x00,0x04,0x57,0x74,0x13,0x0c,
		};
	uint8_t e[32*12]={	0xa8,0x67,0x47,0x4f,0x85,0x7c,0xcd,0xf3,0x91,0xa0,0xad,0x4c,0xa7,0x49,0x33,0xdc,
						0xce,0x39,0xc2,0xbc,0x19,0x06,0xfd,0xf0,0xff,0x37,0x76,0x83,0x51,0xb3,0x1b,0x10,
						0x5a,0x16,0xed,0xe9,0xb3,0xc3,0xe4,0x61,0xe8,0xf7,0xc8,0xf2,0xf0,0x0e,0xd6,0xd0,
						0x2a,0xce,0xde,0x29,0x28,0x58,0xb8,0xa6,0x6a,0x84,0x27,0x5a,0xf9,0xdf,0xd8,0x16,
						0xf0,0x7c,0x81,0x8a,0xb3,0xf5,0xfa,0xef,0xa8,0x1c,0xdd,0x76,0x43,0x67,0x02,0x1e,
						0x1b,0x40,0xd0,0x18,0x2b,0x7f,0x37,0x13,0x15,0xde,0x22,0xa5,0x93,0x7d,0xa7,0x0f,
						0x1c,0x90,0x4f,0x28,0x4f,0x28,0xc7,0xce,0x9d,0x83,0xe3,0xe9,0xab,0x26,0xa0,0xbe,
						0x3d,0xd6,0x99,0xe3,0x3b,0xe0,0xcf,0x87,0x94,0x78,0x59,0x8d,0x4c,0xca,0x98,0x07,
						0x33,0x5e,0x53,0xba,0x8a,0xaf,0x9a,0x0e,0x31,0x1c,0xd2,0x03,0x8c,0xdf,0x72,0xbd,
						0x27,0x64,0xc5,0x14,0xba,0x5e,0xb0,0x60,0x30,0xeb,0x3e,0x1b,0xb2,0xd0,0x84,0x20,
						0xad,0x43,0x13,0x6f,0x85,0xc9,0x48,0x8e,0xd8,0x04,0xf9,0x7a,0x6b,0x5b,0x81,0xe2,
						0x81,0xfc,0x2e,0x91,0x96,0x81,0xad,0xae,0x9d,0xfd,0x6b,0x7c,0xa7,0x01,0x47,0x07,
						0x7c,0xc9,0x84,0x35,0xd4,0x8f,0xd5,0x36,0x7a,0x71,0x4d,0x6e,0x97,0xb0,0x1c,0x54,
						0xe9,0x19,0xdf,0x59,0xd8,0xcf,0x16,0x75,0x10,0x68,0xb7,0x52,0x5f,0xfd,0x7d,0x0b,
						0x96,0x0e,0x2c,0xd7,0x8c,0x44,0xe8,0xbe,0xab,0x1c,0x89,0x22,0x00,0xcd,0xaf,0x05,
						0xa4,0x5e,0xaa,0x92,0x4c,0xa3,0xf9,0x13,0x48,0x34,0x84,0x35,0x83,0x94,0x39,0x0e,
						0x7f,0x59,0x6b,0xe0,0x40,0xd9,0xf4,0xc4,0xde,0x5d,0xe6,0xc8,0xf6,0xa3,0x26,0x9c,
						0x15,0x23,0x9c,0x81,0x5d,0xcb,0x45,0xaf,0x1a,0xd0,0xc1,0xe0,0x5c,0x65,0x7e,0x20,
						0x1f,0xe7,0xb1,0x37,0xaf,0x4f,0xcf,0xf6,0x57,0x50,0x8d,0x93,0x2f,0x6e,0x6a,0x94,
						0xf3,0xf2,0xfa,0x77,0x65,0x6c,0xeb,0xa6,0xad,0xe3,0x6c,0xe2,0xc3,0xfc,0x48,0x03,
						0x8f,0x3a,0x79,0xab,0x59,0x45,0x39,0x04,0xd3,0x76,0x91,0xc5,0x28,0x6b,0xa0,0xfb,
						0x1d,0x63,0x37,0x12,0x3b,0xf8,0x08,0x49,0xb3,0x33,0xfe,0x86,0x17,0xf9,0x63,0x04,
						0x0e,0xa4,0x28,0xde,0xdc,0x1b,0xd8,0x2a,0x52,0xf1,0x75,0x18,0xbc,0xa3,0x9d,0x34,
						0xb1,0xf9,0x57,0x74,0x64,0x95,0x71,0xe4,0xf5,0x15,0xef,0xd9,0x62,0x9b,0xd7,0x19
	};
	memcpy(&param.g1, g1, sizeof(param.g1));
	memcpy(&param.g2, g2, sizeof(param.g2));
	memcpy(&param.e,  e, sizeof(param.e));
#else	
	mclBnG1_hashAndMapTo(&param.g1, "g1", 2);
    mclBnG2_hashAndMapTo(&param.g2, "g2", 2);
    mclBn_pairing(&param.e, &param.g1, &param.g2);
    
	//PrintBuf("param.g1", (const uint8_t *)&param.g1, sizeof(param.g1));
	//PrintBuf("param.g2", (const uint8_t *)&param.g2, sizeof(param.g2));
	//PrintBuf("param.e", (const uint8_t *)&param.e, sizeof(param.e));
#endif
	return 0;
}

UINT32 PreSymmetricKeyGenerate(UINT8 *seed, PRE_SYMMETRIC_KEY *s_key)
{
    mclBnGT m;
	mclBnFr r;
	UINT8 hasher[32];
	size_t len = 0;

	memset(&r, 0, sizeof(r));
	if(seed == NULL)
	{
		mclBnFr_setByCSPRNG(&r);
	}
	else
	{
		mclBnFr_setStr(&r, (const char *)seed, strlen((const char *)seed), 16);
	}
    mclBnGT_pow(&m, &param.e, &r);
	
    len = mclBnGT_getStr((char *)s_key->mGT, sizeof(s_key->mGT), &m, 16);
	memset(hasher, 0, sizeof(hasher));
	SHA256(s_key->mGT, len, hasher);
	HexToStr((char *)s_key->m, hasher, sizeof(hasher));
    
    return 0;
}

static void generate_key(curve_param *curve, keyPair *user_key)
{

    /*      
     how to generate const length  for user key     ?*/

    mclBnFr_setByCSPRNG(&user_key->sk.a);

    mclBnFr_setByCSPRNG(&user_key->sk.b);
    //pk1 = z^a
    mclBnGT_pow(&user_key->pk.pk1, &curve->e, &user_key->sk.a);
    //pk2 = g1^b
    mclBnG1_mul(&user_key->pk.pk2, &curve->g1, &user_key->sk.b);
}


UINT32 PreUserKeyGenerate(PRE_USER_KEY *user_key)
{
	keyPair key;
	size_t len1 = 0;
    size_t len2 = 0;
  
    while ((len1 != PRE_MAX_SK_SIZE) || (len2 != PRE_MAX_SK_SIZE)) // fixed sk's length
    {
		memset(&key, 0, sizeof(key));
        generate_key(&param, &key);

		memset(user_key->secret_key1, 0, sizeof(user_key->secret_key1));
        len1 = mclBnFr_getStr((char *)user_key->secret_key1, PRE_MAX_SK_SIZE, &key.sk.a, 16);
		memset(user_key->secret_key2, 0, sizeof(user_key->secret_key2));
        len2 = mclBnFr_getStr((char *)user_key->secret_key2, PRE_MAX_SK_SIZE, &key.sk.b, 16);
    }

	memset(user_key->public_key1, 0, sizeof(user_key->public_key1));
    mclBnGT_getStr((char *)user_key->public_key1, PRE_MAX_PK1_SIZE, &key.pk.pk1, 16);
	memset(user_key->public_key2, 0, sizeof(user_key->public_key2));
	mclBnG1_getStr((char *)user_key->public_key2, PRE_MAX_PK2_SIZE, &key.pk.pk2, 16);
    
    return 0;
}

//从指定文件读取user_key
UINT32 PreUserKey_Get(char *file ,PRE_USER_KEY *user_key)
{
	int ret=0;
	
	ret = ConfigGet("pre", "secret1", file,  (char *)user_key->secret_key1);
	if(ret || strlen(user_key->secret_key1) == 0)
	{
		printf("read secret1 from %s error \r\n", file);
		return -1;
	}
	ret = ConfigGet("pre", "secret2", file,  (char *)user_key->secret_key2);
	if(ret || strlen(user_key->secret_key1) == 0)
	{
		printf("read secret2 from %s error \r\n", file);
		return -1;
	}
	ret = ConfigGet("pre", "public1", file,  (char *)user_key->public_key1);
	if(ret || strlen(user_key->public_key1) == 0)
	{
		printf("read public1 from %s error \r\n", file);
		return -1;
	}
	ret = ConfigGet("pre", "public2", file,  (char *)user_key->public_key2);
	if(ret || strlen(user_key->public_key2) == 0)
	{
		printf("read public2 from %s error \r\n", file);
		return -1;
	}
	return 0;
}

//将user_key存到指定文件
UINT32 PreUserKey_Set(char *file ,PRE_USER_KEY *user_key)
{
	int ret=0;

	ret = ConfigPut("pre", "secret1",  (char *)user_key->secret_key1, file);
	if(ret)
	{
		printf("set secret1 to %s error \r\n", file);
		return -1;
	}
	ret = ConfigPut("pre", "secret2",  (char *)user_key->secret_key2, file);
	if(ret)
	{
		printf("set secret2 to %s error \r\n", file);
		return -1;
	}
	ret = ConfigPut("pre", "public1",  (char *)user_key->public_key1, file);
	if(ret)
	{
		printf("set public1 to %s error \r\n", file);
		return -1;
	}
	ret = ConfigPut("pre", "public2",  (char *)user_key->public_key2, file);
	if(ret)
	{
		printf("set public2 to %s error \r\n", file);
		return -1;
	}

	return 0;
}


UINT32 PreReEncryptKeyGenerate(PRE_REENCRYPT_KEY *re_key, 
							   PRE_RECEIVER_KEY *receiver_key,
							   PRE_SENDER_KEY *sender_key)
{
    mclBnG1 pkb_G1, rk_G1;
    mclBnFr ska_Fr;

	memset(&pkb_G1, 0, sizeof(pkb_G1));
	memset(&rk_G1, 0, sizeof(rk_G1));
	memset(&ska_Fr, 0, sizeof(ska_Fr));
    mclBnFr_setStr(&ska_Fr, (const char *)sender_key->secret_key1, strlen((const char *)sender_key->secret_key1), 16);
    mclBnG1_setStr(&pkb_G1, (const char *)receiver_key->public_key2, strlen((const char *)receiver_key->public_key2), 16);

	//generate reencrypt key rk = pkj^ai
	mclBnG1_mul(&rk_G1, &pkb_G1, &ska_Fr);

    mclBnG1_getStr((char *)re_key->reencrypt_key, PRE_MAX_RK_SIZE, &rk_G1, 16); //　todo
	return 0;
}

//encryption 1
// c1 = g2^k, c2 = m*z^{ak}
static void level1_cipher_generate(mclBnG2 *g2, mclBnGT *pk1, mclBnGT *m, ciphertext_1 *cipher1)
{
    mclBnGT zak;
    mclBnFr k;

	memset(&zak, 0, sizeof(zak));
	mclBnGT_clear(&zak);

	memset(&k, 0, sizeof(k));
    mclBnFr_setByCSPRNG(&k);

    // c1 = g2^k
    mclBnG2_mul(&cipher1->c1, g2, &k);

    // zak = pk1^k
    mclBnGT_pow(&zak, pk1, &k);

    //c2 = m * z^ak
    mclBnGT_mul(&cipher1->c2, m, &zak);
}


UINT32 PreLevel1CipherGenerate(	PRE_CIPHER_TEXT *level1_cipher, 
								PRE_SENDER_KEY *sender_key, 
								PRE_SYMMETRIC_KEY *msg)
{
    ciphertext_1 cipher1;
	mclBnGT pk1;
	mclBnGT m;
	
	memset(&pk1, 0, sizeof(pk1));
    mclBnGT_setStr(&pk1, (const char *)sender_key->public_key1, strlen((const char *)sender_key->public_key1), 16);

	memset(&m, 0, sizeof(m));
    mclBnGT_setStr(&m, (const char *)msg->mGT, strlen((const char *)msg->mGT), 16);

	
    level1_cipher_generate(&param.g2, &pk1, &m, &cipher1);
    mclBnG2_getStr((char *)level1_cipher->cipher_text1, PRE_MAX_CIPHER_LEVEL1_1, &cipher1.c1, 16);
    mclBnGT_getStr((char *)level1_cipher->cipher_text2, PRE_MAX_CIPHER_LEVEL1_2, &cipher1.c2, 16);
    
    return 0;
}

//c1 = e(pk2,g2)^k= e^bk, c2 = m e^k
static void local_cipher_generate(mclBnG1 *pk2, curve_param *curve, mclBnGT *m, ciphertext_2 *c2)
{
	mclBnGT zk;
	mclBnFr k;
	
	mclBn_pairing(&c2->c1, pk2, &curve->g2);
	mclBnGT_pow(&c2->c1, &c2->c1, &k);
	//c2 = m * z^ak
	mclBnGT_pow(&zk, &curve->e, &k);
	mclBnGT_mul(&c2->c2, m, &zk);
}


UINT32 PreLocalCipherGenerate(PRE_CIPHER_TEXT *local_cipher, 
							  PRE_SENDER_KEY *sender_key, 
							  PRE_SYMMETRIC_KEY *msg)
{
	ciphertext_2 cipher2;
	mclBnG1 pk2;
	mclBnGT m;

	memset(&pk2, 0, sizeof(pk2));
	mclBnG1_setStr(&pk2, (const char *)sender_key->public_key2, strlen((const char *)sender_key->public_key2), 16);

	memset(&m, 0, sizeof(m));
	mclBnGT_setStr(&m, (const char *)msg->mGT, strlen((const char *)msg->mGT), 16);
	local_cipher_generate(&pk2, &param, &m, &cipher2);

	mclBnGT_getStr((char *)local_cipher->cipher_text1, PRE_MAX_CIPHER_LEVEL2, &cipher2.c1, 16);
	mclBnGT_getStr((char *)local_cipher->cipher_text2, PRE_MAX_CIPHER_LEVEL2, &cipher2.c2, 16);
	
	
	return 0;
}

static void reencrypt_cipher_generate(mclBnG1 *rk, ciphertext_1 *cipher1, mclBnGT *pk1, mclBnG2 *g2, ciphertext_2 *rec)
{
    mclBnFr k_prime;
    mclBnG2 g2_kprime, g2kk;
    mclBnGT pk1_kprime;
    /*     const char *test = "123";
    mclBnFr_setStr(&k_prime, test, strlen(test), 16); */
    mclBnFr_setByCSPRNG(&k_prime);

    //g2_kprime = g2^k'
    mclBnG2_mul(&g2_kprime, g2, &k_prime);
    //g1kk = g2^k * g2^k'
    mclBnG2_add(&g2kk, &g2_kprime, &cipher1->c1);

    // c1 = e(rk,g2kk)
    mclBn_pairing(&rec->c1, rk, &g2kk);

    // c2 =  c2 * pk1^k'
    mclBnGT_pow(&pk1_kprime, pk1, &k_prime);
    mclBnGT_mul(&rec->c2, &cipher1->c2, &pk1_kprime);
}

UINT32 PreReEncryptCipherGenerate(PRE_CIPHER_TEXT *reencrypt_cipher, 
							   	  PRE_REENCRYPT_KEY *reencrypt_key, 
							      PRE_CIPHER_TEXT *level1_cipher, 
							      PRE_SENDER_KEY *sender_key)
{
	mclBnG1 rk;
	mclBnGT pk1;
	ciphertext_1 cipher1;
	ciphertext_2 cipher2;

	memset(&rk, 0, sizeof(rk));
	mclBnG1_setStr(&rk, (const char *)reencrypt_key->reencrypt_key, strlen((const char *)reencrypt_key->reencrypt_key), 16);

	memset(&pk1, 0, sizeof(pk1));
	mclBnGT_setStr(&pk1, (const char *)sender_key->public_key1, strlen((const char *)sender_key->public_key1), 16);

	memset(&cipher1, 0, sizeof(cipher1));
	mclBnG2_setStr(&cipher1.c1, (const char *)level1_cipher->cipher_text1, strlen((const char *)level1_cipher->cipher_text1), 16);
    mclBnGT_setStr(&cipher1.c2, (const char *)level1_cipher->cipher_text2, strlen((const char *)level1_cipher->cipher_text2), 16);

	memset(&cipher2, 0, sizeof(cipher2));
	reencrypt_cipher_generate(&rk, &cipher1, &pk1, &param.g2, &cipher2);

	mclBnGT_getStr((char *)reencrypt_cipher->cipher_text1, PRE_MAX_CIPHER_LEVEL2, &cipher2.c1, 16);
    mclBnGT_getStr((char *)reencrypt_cipher->cipher_text2, PRE_MAX_CIPHER_LEVEL2, &cipher2.c2, 16);
	
    
    return 0;
}

static void pre_decryption(ciphertext_2 *recipher, mclBnFr *b, mclBnGT *dec2_m)
{
	mclBnFr b_neg;
	mclBnGT c1_negb;
	mclBnFr_inv(&b_neg, b);
	//c1^(1/b)
	mclBnGT_pow(&c1_negb, &recipher->c1, &b_neg);
	mclBnGT_div(dec2_m, &recipher->c2, &c1_negb);
}


UINT32 PreDecryption(PRE_SYMMETRIC_KEY *msg, 
				  	 PRE_CIPHER_TEXT *reencrypt_cipher, 
				  	 PRE_RECEIVER_KEY *receiver_key)
{
	ciphertext_2 cipher2;
	mclBnFr skb_Fr;
	mclBnGT m;
	size_t len = 0;
	UINT8 hasher[32];

	memset(&cipher2, 0, sizeof(cipher2));
	mclBnGT_setStr(&cipher2.c1, (const char *)reencrypt_cipher->cipher_text1, strlen((const char *)reencrypt_cipher->cipher_text1), 16);
	mclBnGT_setStr(&cipher2.c2, (const char *)reencrypt_cipher->cipher_text2, strlen((const char *)reencrypt_cipher->cipher_text2), 16);
	
	memset(&skb_Fr, 0, sizeof(skb_Fr));
	mclBnFr_setStr(&skb_Fr, (const char *)receiver_key->secret_key2, strlen((const char *)receiver_key->secret_key2), 16);

	memset(&m, 0, sizeof(m));
	pre_decryption(&cipher2, &skb_Fr, &m);

	len = mclBnGT_getStr((char *)msg->mGT, sizeof(msg->mGT), &m, 16);
	memset(hasher, 0, sizeof(hasher));
	SHA256(msg->mGT, len, hasher);
	HexToStr((char *)msg->m, hasher, sizeof(hasher));
	
	return 0;
}

#if 0
int main(int argc, char **argv)
{
	PRE_SYMMETRIC_KEY 	data_key;
	PRE_SENDER_KEY		sender_key;	
	PRE_RECEIVER_KEY	receiver_key;	
	PRE_REENCRYPT_KEY 	re_key;
	PRE_CIPHER_TEXT 	level1_cipher;
	PRE_CIPHER_TEXT 	local_cipher;
	PRE_CIPHER_TEXT 	reencrypt_cipher;
	PRE_SYMMETRIC_KEY 	msg;
	PRE_SYMMETRIC_KEY 	msg_local;

	//1.生成用于实际数据加密使用的对称密钥m，为GT曲线上的一个点
	printf("1.生成用于实际数据加密使用的对称密钥m，为GT曲线上的一个点\r\n");
	memset(&data_key, 0, sizeof(data_key));
	PreSymmetricKeyGenerate(&data_key);
	printf("data_key m:%s\r\n", data_key.m);
	printf("data_key mGT:%s\r\n\r\n", data_key.mGT);

	//2.生成发送者Alice的公私钥
	printf("2.生成发送者Alice的公私钥\r\n");
	memset(&sender_key, 0, sizeof(sender_key));
	PreUserKeyGenerate(&sender_key);
	printf("sender secret key1:%s\r\n", sender_key.secret_key1);
	printf("sender secret key2:%s\r\n", sender_key.secret_key2);
	printf("sender public key1:%s\r\n", sender_key.public_key1);
	printf("sender public key2:%s\r\n\r\n", sender_key.public_key2);

	//3.生成接收者(Bob)的公私钥
	printf("3.生成接收者(Bob)的公私钥\r\n");
	memset(&receiver_key, 0, sizeof(receiver_key));
	PreUserKeyGenerate(&receiver_key);
	printf("receiver secret key1:%s\r\n", receiver_key.secret_key1);
	printf("receiver secret key2:%s\r\n", receiver_key.secret_key2);
	printf("receiver public key1:%s\r\n", receiver_key.public_key1);
	printf("receiver public key2:%s\r\n\r\n", receiver_key.public_key2);

	//4.利用接收者(Bob)的公钥与发送者Alice的私钥，一起生成重加密密钥
	printf("4.利用接收者(Bob)的公钥与发送者Alice的私钥，一起生成重加密密钥\r\n");
	memset(&re_key, 0, sizeof(re_key));
	PreReEncryptKeyGenerate(&re_key, &receiver_key, &sender_key);
	printf("reencrypt key:%s\r\n\r\n", re_key.reencrypt_key);

	//5.利用发送者Alice的公钥加密数据，生成第一重数据密文
	printf("5.利用发送者Alice的公钥加密数据，生成第一重数据密文\r\n");
	memset(&level1_cipher, 0, sizeof(level1_cipher));
	PreLevel1CipherGenerate(&level1_cipher, &sender_key, &data_key);
	printf("level1 cipher1= %s\r\n", level1_cipher.cipher_text1);
	printf("level1 cipher2= %s\r\n\r\n", level1_cipher.cipher_text2);

	//6.利用发送者Alice的公钥加密数据，生成本地存储数据密文
	printf("6.利用发送者Alice的公钥加密数据，生成本地存储数据密文\r\n");
	memset(&local_cipher, 0, sizeof(local_cipher));
	PreLocalCipherGenerate(&local_cipher, &sender_key, &data_key);
	printf("local cipher1= %s\r\n", local_cipher.cipher_text1);
	printf("local cipher2= %s\r\n\r\n", local_cipher.cipher_text2);

	//7.利用重加密密钥与发送者Alice的公钥一起加密第一重数据密文，生成第二重数据密文
	printf("7.利用重加密密钥与发送者Alice的公钥一起加密第一重数据密文，生成第二重数据密文\r\n");
	memset(&reencrypt_cipher, 0, sizeof(reencrypt_cipher));
	PreReEncryptCipherGenerate(&reencrypt_cipher, &re_key, &level1_cipher, &sender_key);
	printf("reencrypt cipher1= %s\r\n", reencrypt_cipher.cipher_text1);
	printf("reencrypt cipher2= %s\r\n\r\n", reencrypt_cipher.cipher_text2);

	//8.接收者(Bob)利用自己的私钥解密第二重数据密文，得到数据原文
	printf("8.接收者(Bob)利用自己的私钥解密第二重数据密文，得到数据原文\r\n");
	memset(&msg, 0, sizeof(msg));
	PreDecryption(&msg, &reencrypt_cipher, &receiver_key);
	printf("msg m:%s\r\n", msg.m);
	printf("msg mGT:%s\r\n\r\n", msg.mGT);

	//9.发送者Alice利用自己的私钥解密本地数据密文，得到数据原文
	printf("9.发送者Alice利用自己的私钥解密本地数据密文，得到数据原文\r\n");
	memset(&msg_local, 0, sizeof(msg_local));
	PreDecryption(&msg_local, &local_cipher, &sender_key);
	printf("msg local m:%s\r\n", msg_local.m);
	printf("msg local mGT:%s\r\n\r\n", msg_local.mGT);

	
	return 0;
}
#endif



