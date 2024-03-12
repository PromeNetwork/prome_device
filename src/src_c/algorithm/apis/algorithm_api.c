/***********************************************************************************
 * @file   			: algorithm_api.c

 * @date   			: 2020-9-15
 * @Description   	: This file provides the underlying algorithm interface.
                      Including the following algorithms: hash, signature encry-
                      ption, key pair generation, public key recovery, Base64 
                      encoding and decoding.
 * @Copyright   	: Copyright (c) 2008-2020
 * @other   		: 
 * @note History 	: 
***********************************************************************************/

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "basic_types.h"
#include "md5.h"
#include "hasher.h"
#include "base64.h"
#include "secp256k1_api.h"
#include "algorithm_api.h"
#include "average.h"
#include "aes.h"
#include "aes_cbc.h"
#include "aes_ofb.h"
#include "aes_ctr.h"
#include "rand.h"

/*****************************************************************************
 * @Function	: file_size

 * @date		: 2020-8-10
 * @brief       : get the file size
 * @param[in]  	: UINT8 *file  file name
 * @param[out]  : None
 * @return  	: the file size
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static UINT32 file_size(UINT8 *file) 
{
	INT32 fd = 0;
	INT32 filelen = 0;

	fd = open((char *)file, O_RDONLY);
	if (fd < 0) 
	{	 
		return ERR_FILE_OPEN_FAILED;
	}	

	filelen = lseek(fd, 0L, SEEK_END);
	lseek(fd,0L,SEEK_SET); 
	close(fd);

	return filelen;
}

/*****************************************************************************
 * @Function	: file_read

 * @date		: 2020-8-10
 * @brief       : Read data the of the file
 * @param[in]  	: UINT8 *file       the file to read 
               	  UINT32 read_size  read size
 * @param[out]  : UINT8 *filebuff   data read out
 * @return  	: success:0
 				  fail:ERR_CODE
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
static ERR_CODE file_read(UINT8 *file, UINT32 read_size, UINT8 *filebuff)
{
	INT32 fd = 0;
	INT32 ret = 0;


	
	if((read_size == 0) || (filebuff == NULL))
	{
		return ERR_PARAMETER_CHECK_FAILED;
	}

	fd = open((char *)file, O_RDONLY);
	if (fd < 0) 
	{	 
		return ERR_FILE_OPEN_FAILED;
	}	

	ret = read(fd, filebuff, read_size);
	if(ret < 0)
	{
		close(fd);
		return ERR_FILE_READ_FAILED; 
	}
	close(fd);

	return ERR_SUCCESS;
}



/*****************************************************************************
 * @Function	: DataHash

 * @date		: 2020-8-10
 * @brief       : Calculate the hash value of text data, using the specified
                  hash algorithm;
 * @param[in]  	: UINT8 *data  				Text data to be hashed
 				  UINT32 len  				Text data length
               	  HASH_TYPE hash_method  	hash algorithm
 * @param[out]  : UINT8 *hash  Hash value
 * @return		: success:length of hash value
 				  fail:0
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 DataHash( UINT8 *data, UINT32 len, HASH_TYPE hash_method, UINT8 *hash )
{
	UINT32 ret = 0;
	
    if((data == NULL) || (hash == NULL))
	{
		return 0;
	}

	switch(hash_method)
	{
		case HASH_MD5:
			Md5Calculate(data, len, hash);
			ret = MD5_HASH_SIZE;
			break;
		case HASH_SHA_256:
			//sha256_Raw(data, len, hash);
			hasher_Raw(HASHER_SHA2, data, len, hash);
			ret = HASHER_DIGEST_LENGTH;
			break;
		case HASH_SHA3K:
			//sha256_Raw(data, len, hash);
			hasher_Raw(HASHER_SHA3K, data, len, hash);
			ret = HASHER_DIGEST_LENGTH;
			break;
		default:
			ret = 0;
			break;
	}

	return ret;
}

/*****************************************************************************
 * @Function	: FileHash

 * @date		: 2020-8-10
 * @brief       : Calculate the hash value of the file, using the specified
                  hash algorithm;
 * @param[in]  	: UINT8 *file              The file to be hashed
               	  HASH_TYPE hash_method  hash algorithm
 * @param[out]  : UINT8 *hash              hash value
 * @return  	: success:0
 				  fail:ERR_CODE
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
UINT32 FileHash(UINT8 *file, HASH_TYPE hash_method, UINT8 *hash)
{
	UINT32 size = 0;
	ERR_CODE ret = ERR_SUCCESS;
	UINT8 *filebuff = NULL;
	
    if((file == NULL) || (hash == NULL))
	{
		return 0;
	}

	/*1. get the file size*/
	size = file_size(file);
	if(size == 0)
	{
		DEBUG_PRINT("file:%s is NULL\r\n", file);
		return 0;
	}

	/*2.malloc space to store the file data*/
	filebuff = (UINT8 *)malloc(size+1);
	if(filebuff == NULL)
	{
		return 0;
	}
	memset(filebuff, 0, size);

	/*3.read the file*/
	ret = file_read(file, size, filebuff);
	if(ret != ERR_SUCCESS)
	{
		DEBUG_PRINT("read file:%s fail\r\n", file);
		SAFE_FREE(filebuff);
		return 0;
	}

	/*4.Calculate the hash value*/
	ret = DataHash(filebuff, size, hash_method, hash);
	if(ret == 0)
	{
		DEBUG_PRINT("hash file:%s fail\r\n", file);
		SAFE_FREE(filebuff);
		return 0;
	}

	/*5.free the malloc space*/
	SAFE_FREE(filebuff);
	
	return ret;
}


/*****************************************************************************
 * @Function	: DataSignature
 * @author		: Preh
 * @date		: 2020-8-12
 * @brief       : Calculate the signature value of the data, using the 
 				  spec-ified signature algorithm
 * @param[in]  	: UINT8 *data            data to be signed
               	  UINT32 data_len        data length
               	  SIGN_TYPE sign_method  signature algorithm
               	  UINT8 *pri_key         signature private key
 * @param[out]  : None
 * @return  	: the signature data
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
ERR_CODE DataSignature(UINT8 *data, UINT32 data_len, SIGN_TYPE sign_method, 
					   UINT8 *pri_key, UINT8 signed_msg[SIGN_MAX_LEN])
{
	ERR_CODE ret = ERR_SUCCESS;
	UINT8 recid = 0;
	
    if((data == NULL) || (pri_key == NULL))
	{
		return ERR_PARAMETER_CHECK_FAILED;
	}

	switch(sign_method)
	{
		case SIGN_ECDSA:
			/*3.ecdsa secp256k1 data signature*/
 			ret = secp256k1_sign(pri_key, data, data_len, 
								 signed_msg, &recid);
			if(ret != ERR_SUCCESS)
			{
				return ERR_SIGN_FAIL;
			}
			signed_msg[64] = recid;
			break;
		default:
			ret = ERR_SIGN_METHOD_NOT_SUPPORT;
			break;
	}

	return ret;
}

#if 1
/*****************************************************************************
 * @Function	: DataEncryption
 * @author 	 	:
 * @date		: 2020-8-10
 * @brief		: Encrypt the data, using the specified encryption algorithm
 * @param[in]	: UINT8 *data			  Data to be encrypted
 			   	  ENC_TYPE enc_method  encryption algorithm
 			   	  UINT8 *enc_key		  encryption key
 * @param[out]  : UINT8 *cipher		  cipher data
 * @return 	 	: success:0
 			   	  fail:ERR_CODE
 * @relation	: 
 * @other		: 

*****************************************************************************/
UINT8 *DataEncryption(UINT8 *data, UINT32 data_len, ENC_TYPE enc_method, 
					  UINT8 *key,  UINT32 key_len, 	UINT32 *err_code)
{
	UINT32 enc_out_len = 0;
	UINT8 *enc_out = NULL;
	UINT8 *base64_out = NULL;
	//UINT8 iv[16] = {0x53,0x61,0x6C,0x74,0x65,0x64,0x5F,0x5F,0x2C,0xBD,0x94,0x2B,0xB7,0xAE,0x60,0xB5};
	UINT8 iv[16] = {0x53,0x61,0x6C,0x74,0x65,0x64,0x5F,0x5F};
	
	if(err_code == NULL)
	{
		return NULL;
	}

	*err_code = ERR_SUCCESS;
	if((data == NULL) || (key == NULL))
	{
		if(err_code)
		{
			*err_code = ERR_PARAMETER_CHECK_FAILED;
		}
		return NULL;
	}

	switch(enc_method)
	{
		case ENC_TYPE_ECDSA:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			break;
		case ENC_TYPE_3DES:
			break;
		case ENC_TYPE_AES_ECB_PKCS5:
			//PrintBuf( "data", (const uint8_t *) data, data_len);
			//PrintBuf( "key", (const uint8_t *) key, key_len);
			enc_out = AES_ECB_Pkcs5Padding_Encrypt(data, data_len, key, key_len, &enc_out_len);
			if(enc_out == NULL && err_code)
			{
				*err_code = ERR_ENCRYPT_FAIL;
				return NULL;
			}
			break;
		case ENC_TYPE_AES_CBC_PKCS5:
			random_buffer(iv+8, 8);
			//PrintBuf( "data", (const uint8_t *) data, data_len);
			//PrintBuf( "key", (const uint8_t *) key, key_len);
			//PrintBuf( "iv", (const uint8_t *) iv, sizeof(iv));
			enc_out = AES_CBC_Pkcs5Padding_Encrypt(data, data_len, key, key_len, iv, sizeof(iv),&enc_out_len);
			if(enc_out == NULL && err_code)
			{
				*err_code = ERR_ENCRYPT_FAIL;
				return NULL;
			}
			break;
		case ENC_TYPE_AES_OFB:
			random_buffer(iv+8, 8);
			enc_out = (uint8_t *)malloc(data_len+sizeof(iv) + 1);
			if(enc_out == NULL)
			{
				*err_code = ERR_ENCRYPT_FAIL;
				return NULL;
			}
			*err_code = AesOfbXorWithKey(key, key_len, iv, data,enc_out+sizeof(iv), data_len);
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_FAIL;
				SAFE_FREE(enc_out);
				return NULL;
			}
			memcpy(enc_out, iv, sizeof(iv));
			break;
		case ENC_TYPE_AES_CTR:
			random_buffer(iv+8, 8);
			enc_out = (uint8_t *)malloc(data_len+ sizeof(iv) + 1);
			if(enc_out == NULL)
			{
				*err_code = ERR_ENCRYPT_FAIL;
				return NULL;
			}
			*err_code = AesCtrXorWithKey(key, key_len, iv, data,enc_out+sizeof(iv), data_len);
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_FAIL;
				SAFE_FREE(enc_out);
				return NULL;
			}
			memcpy(enc_out, iv, sizeof(iv));
			break;
		default:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			return NULL;
			break;
	}
	//PrintBuf( "out", (const uint8_t *) out, enc_out_len);
	
	base64_out = DataBase64Encode(enc_out, enc_out_len);
	if(base64_out == NULL)
	{
		if(err_code)
		{
			*err_code = ERR_BASE64_ENCODE_FAIL;
		}
		SAFE_FREE(enc_out);
		return NULL;
	}

	SAFE_FREE(enc_out);
	return base64_out;
}

UINT8 *DataDecryption(UINT8 *data, UINT32 data_len, ENC_TYPE enc_method, 
					  UINT8 *key,  UINT32 key_len, 	UINT32 *err_code, 
					  UINT32 *dec_len)
{
	UINT32 base64_out_len = 0;
	UINT8 *base64_out = NULL;
	UINT8 *dec_out = NULL;
	UINT8 iv[16] = {0};
	
	*err_code = ERR_SUCCESS;
	/*private base64 decode*/
	base64_out = DataBase64Decode(data, data_len, &base64_out_len);
	if(base64_out == NULL)
	{
		*err_code = ERR_BASE64_DECODE_FAIL;
		return NULL;
	}
	//PrintBuf( "base64_out", (const uint8_t *) base64_out, base64_out_len);
	
	switch(enc_method)
	{
		case ENC_TYPE_ECDSA:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			break;
		case ENC_TYPE_3DES:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			break;
		case ENC_TYPE_AES_ECB_PKCS5:
			dec_out = AES_ECB_Pkcs5Padding_Decrypt(base64_out, base64_out_len, 
												   key, key_len, dec_len);
			if(dec_out == NULL && err_code)
			{
				*err_code = ERR_DECRYPT_FAIL;
			}
			break;
		case ENC_TYPE_AES_CBC_PKCS5:
			memcpy(iv,base64_out,16);
			dec_out = AES_CBC_Pkcs5Padding_Decrypt(base64_out+16, base64_out_len-16, 
												   key, key_len, iv, sizeof(iv), dec_len);
			if(dec_out == NULL && err_code)
			{
				*err_code = ERR_DECRYPT_FAIL;
			}
			break;
		case ENC_TYPE_AES_OFB:
			memcpy(iv,base64_out,16);
			dec_out = (uint8_t *)malloc(base64_out_len-16 + 1);
			if(dec_out == NULL)
			{
				*err_code = ERR_DECRYPT_FAIL;
				return NULL;
			}
			*err_code = AesOfbXorWithKey(key, key_len, iv, base64_out+16, dec_out,base64_out_len-16);
			if(err_code)
			{
				*err_code = ERR_DECRYPT_FAIL;
				SAFE_FREE(dec_out);
			}
			break;
		case ENC_TYPE_AES_CTR:
			memcpy(iv,base64_out,16);
			dec_out = (uint8_t *)malloc(base64_out_len-16 + 1);
			if(dec_out == NULL)
			{
				*err_code = ERR_DECRYPT_FAIL;
				return NULL;
			}
			*err_code = AesCtrXorWithKey(key, key_len, iv, base64_out+16, dec_out,base64_out_len-16);
			if(err_code)
			{
				*err_code = ERR_DECRYPT_FAIL;
				SAFE_FREE(dec_out);
			}
			break;
		default:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			break;
	}

	SAFE_FREE(base64_out);
	
	return dec_out;
}
#else
/*****************************************************************************
 * @Function	: DataEncryption
 * @author 	 	:
 * @date		: 2020-8-10
 * @brief		: Encrypt the data, using the specified encryption algorithm
 * @param[in]	: UINT8 *data			  Data to be encrypted
 			   	  ENC_TYPE enc_method  encryption algorithm
 			   	  UINT8 *enc_key		  encryption key
 * @param[out]  : UINT8 *cipher		  cipher data
 * @return 	 	: success:0
 			   	  fail:ERR_CODE
 * @relation	: 
 * @other		: 

*****************************************************************************/
UINT8 *DataEncryption(UINT8 *data, UINT32 data_len, ENC_TYPE enc_method, 
					  UINT8 *key,  UINT32 key_len, 	UINT32 *err_code)
{
	UINT32 enc_out_len = 0;
	UINT8 *enc_out = NULL;
	//UINT8 iv[16] = {0x53,0x61,0x6C,0x74,0x65,0x64,0x5F,0x5F,0x2C,0xBD,0x94,0x2B,0xB7,0xAE,0x60,0xB5};
	UINT8 iv[16] = {0x53,0x61,0x6C,0x74,0x65,0x64,0x5F,0x5F};
	
	if(err_code == NULL)
	{
		return NULL;
	}

	*err_code = ERR_SUCCESS;
	if((data == NULL) || (key == NULL))
	{
		if(err_code)
		{
			*err_code = ERR_PARAMETER_CHECK_FAILED;
		}
		return NULL;
	}

	switch(enc_method)
	{
		case ENC_TYPE_ECDSA:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			break;
		case ENC_TYPE_3DES:
			break;
		case ENC_TYPE_AES_ECB_PKCS5:
			//PrintBuf( "data", (const uint8_t *) data, data_len);
			//PrintBuf( "key", (const uint8_t *) key, key_len);
			enc_out = AES_ECB_Pkcs5Padding_Encrypt(data, data_len, key, key_len, &enc_out_len);
			if(enc_out == NULL && err_code)
			{
				*err_code = ERR_ENCRYPT_FAIL;
				return NULL;
			}
			break;
		case ENC_TYPE_AES_CBC_PKCS5:
			random_buffer(iv+8, 8);
			//PrintBuf( "data", (const uint8_t *) data, data_len);
			//PrintBuf( "key", (const uint8_t *) key, key_len);
			//PrintBuf( "iv", (const uint8_t *) iv, sizeof(iv));
			enc_out = AES_CBC_Pkcs5Padding_Encrypt(data, data_len, key, key_len, iv, sizeof(iv),&enc_out_len);
			if(enc_out == NULL && err_code)
			{
				*err_code = ERR_ENCRYPT_FAIL;
				return NULL;
			}
			break;
		default:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			return NULL;
			break;
	}
	//PrintBuf( "out", (const uint8_t *) out, enc_out_len);
	
	return enc_out;
}

UINT8 *DataDecryption(UINT8 *data, UINT32 data_len, ENC_TYPE enc_method, 
					  UINT8 *key,  UINT32 key_len, 	UINT32 *err_code, 
					  UINT32 *dec_len)
{
	UINT32 base64_out_len = 0;
	UINT8 *dec_out = NULL;
	UINT8 iv[16] = {0};
	
	*err_code = ERR_SUCCESS;
	//PrintBuf( "base64_out", (const uint8_t *) base64_out, base64_out_len);
	
	switch(enc_method)
	{
		case ENC_TYPE_ECDSA:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			break;
		case ENC_TYPE_3DES:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			break;
		case ENC_TYPE_AES_ECB_PKCS5:
			dec_out = AES_ECB_Pkcs5Padding_Decrypt(data, data_len, 
												   key, key_len, dec_len);
			if(dec_out == NULL && err_code)
			{
				*err_code = ERR_DECRYPT_FAIL;
			}
			break;
		case ENC_TYPE_AES_CBC_PKCS5:
			memcpy(iv,data,16);
			dec_out = AES_CBC_Pkcs5Padding_Decrypt(data+16, data_len-16, 
												   key, key_len, iv, sizeof(iv), dec_len);
			if(dec_out == NULL && err_code)
			{
				*err_code = ERR_DECRYPT_FAIL;
			}
			break;
		default:
			if(err_code)
			{
				*err_code = ERR_ENCRYPT_METHOD_NOT_SUPPORT;
			}
			break;
	}

	return dec_out;
}

#endif

/*****************************************************************************
 * @Function	: ShareKeyCalc

 * @date		: 2020-8-10
 * @brief		: Calculate the shared key, using the private key of the
 				  data sending and the public key of the data receiver
 * @param[in]   : UINT8 *r_pub_key	   the public key of the data receiver
 				  UINT8 *s_pri_key	   the private key of the data sending
 				  ENC_TYPE enc_method  encryption algorithm
 * @param[out]  : UINT8 *share_key	   the shared key
 * @return	  	: success:0
 				  fail:ERR_CODE
 * @relation	: 
 * @other		: 

*****************************************************************************/
ERR_CODE ShareKeyCalc(UINT8 *r_pub_key, UINT8 *s_pri_key, 
					  ENC_TYPE enc_method, UINT8 *share_key)
{
	ERR_CODE ret = ERR_SUCCESS;

	if((r_pub_key == NULL) || (s_pri_key == NULL) || (share_key == NULL))
	{
		return ERR_PARAMETER_CHECK_FAILED;
	}

	switch(enc_method)
	{
		case ENC_TYPE_ECDSA:
			secp256k1_ecdh(s_pri_key, r_pub_key, share_key);
			break;
		default:
			ret = ERR_HASH_METHOD_NOT_SUPPORT;
			break;
	}

	return ret;
}

/*****************************************************************************
 * @Function	: PublicKeyRecover

 * @date		: 2020-8-27
 * @brief       : Recover an ECDSA public key from a signature
 * @param[in]  	: UINT8 *sig      data signture
               	  UINT8 *msg      original data
               	  UINT32 msg_len  data length
 * @param[out]  : UINT8 *pub_key  a 65-byte array computed from the signature
 * @return  	: ERR_CODE
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
ERR_CODE PublicKeyRecover(ENC_TYPE enc_method, UINT8 *sig, UINT8 *msg, 
						  UINT32 msg_len, UINT8 *pub_key)
{
	ERR_CODE ret = ERR_SUCCESS;
	
	switch(enc_method)
	{
		case ENC_TYPE_ECDSA:
			secp256k1_recover(sig, msg, msg_len, sig[64], pub_key);
			break;
		default:
			ret = ERR_HASH_METHOD_NOT_SUPPORT;
			break;
	}
	
	return ret;
}


/*****************************************************************************
 * @Function	: KeyPairCreate

 * @date		: 2020-9-14
 * @brief       : Key pair generation function
 				  The function automatically generates a random number and uses 
 				  the number to generate a pair of public and private keys.
 * @param[in]  	: ENC_TYPE enc_method  Key pair generation algorithm
 * @param[out]  : UINT8 pri_key[32]    32 bytes private key
               	  UINT8 pub_key[65]    65 bytes public key
 * @return  	: ERR_CODE
 * @relation  	: 
 * @other  		: 

*****************************************************************************/
ERR_CODE KeyPairCreate(ENC_TYPE enc_method, 
					   UINT8 pri_key[PRI_KEY_MAX_LEN], 
					   UINT8 pub_key[PUB_KEY_MAX_LEN])
{
    ERR_CODE ret = ERR_SUCCESS;
	
	switch(enc_method)
	{
		case ENC_TYPE_ECDSA:
			ecdsa_keypair_create(pri_key, pub_key);
			break;
		default:
			ret = ERR_HASH_METHOD_NOT_SUPPORT;
			break;
	}
	
	return ret;
}


/*****************************************************************************
 * @Function	: DataBase64Decode
 * @author		: Preh
 * @date		: 2020-8-10
 * @brief		: decode private key coding by base64
 * @param[in]   : UINT8 *data 			data encoded by base64
 				  UINT32 data_len		data length				
 * @return		: success:	decoded data, the space must be freed manually. 
 				  fail:		NULL
 * @relation	: 
 * @other 	  	: 

*****************************************************************************/
UINT8 *DataBase64Decode(UINT8 *data, UINT32 data_len, UINT32 *out_len)
{
	UINT32 ret = 0;
	UINT8 *out = NULL;

	if(data == NULL)
	{
		return NULL;
	}
	/*1.Calculate the private key length after decoding */
	*out_len = BASE64_DECODE_OUT_SIZE(data_len);
	out = (UINT8 *)malloc(*out_len);
	if(out == NULL)
	{
		return NULL;
	}

	/*2.decode the private key*/
	ret = base64_decode((char *)data, data_len, out);
	if(ret == 0)
	{
		printf("Data:%s base64 decode fail, ret:%d, *out_len:%d \r\n", data, ret, *out_len);
		return NULL;
	}
	*out_len = ret;

	return out;
}

 /*****************************************************************************
  * @Function	 : DataBase64Encode
  * @author 	 : Preh
  * @date		 : 2020-8-10
  * @brief		 : encoding data by base64
  * @param[in]	 : UINT8* data	  	data to encode
				   UINT16 data_len  data length 
  * @param[out]  : None
  * @return 	 : encoded data by base64
  * @relation	 : 
  * @other		 : 
 
 *****************************************************************************/
 UINT8 *DataBase64Encode(UINT8* data, UINT32 data_len)
 {
	INT16 out_len;
	UINT8* encode_data = NULL;
	INT16 ret = 0; 


	/*1.Calculate the data length after encoding */
	out_len = BASE64_ENCODE_OUT_SIZE(data_len);


	/*2.malloc space to store the after encoding*/
	encode_data = (UINT8 *)malloc(out_len);
	if(encode_data == NULL)
	{
		return NULL;
	}
	memset(encode_data, 0U, out_len);

	/*3.encoding data by Base64*/
	ret = base64_encode(data, data_len, (char *)encode_data);
	if(ret != (out_len - 1))
	{
		DEBUG_PRINT("Data base64 encode fail, ret:%d\r\n", ret);
		return NULL;
	}
	DEBUG_PRINT("encode_data:%s\r\n", encode_data);

	return encode_data;
 }
 
 double AveragecALC(double *array, UINT32 len)
 {
 	return average(array, len);
 }

 


