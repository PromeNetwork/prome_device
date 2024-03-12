#include <stdio.h>
#include <pthread.h>

#include "ecdsa.h"
#include "secp256k1.h"
#include "secp256k1_api.h"
#include "rand.h"

pthread_mutex_t ecdsa_lock = PTHREAD_MUTEX_INITIALIZER;


#define ECDSA_LOCK()		{\
								pthread_mutex_lock(&ecdsa_lock);\
							}

#define ECDSA_UNLOCK()		{\
								pthread_mutex_unlock(&ecdsa_lock);\
							}


/* Create an ECDSA signature. Always return low-S signature.
 *
 * Inputs: 32-byte message m, 32-byte scalar key d, 32-byte scalar nonce k.
 * 
 * Compute point R = k * G. Reject nonce if R's x coordinate is zero.
 * Compute 32-byte scalar r, the serialization of R's x coordinate.
 * Compose 32-byte scalar s = k^-1 * (r * d + m). Reject nonce if s is zero.
 * The signature is (r, s).
 * Option: Function noncefn
 * Nonce generator. By default it is rfc6979.
 * 
 * Function signature:
 * 
 * noncefn(Buffer message, Buffer privateKey, ?Buffer algo, ?Buffer data, Number attempt) -> Buffer
 * Option: Buffer data
 * Additional data for noncefn (RFC 6979 3.6) (32 bytes). By default is null.
 */
int secp256k1_sign(const uint8_t *priv_key, const uint8_t *msg, uint32_t msg_len, uint8_t *sig, uint8_t *recid)
{
	int ret = 0;
	
	ECDSA_LOCK();
	ret = ecdsa_sign(&secp256k1, HASHER_SHA3K, priv_key, msg, msg_len, sig, recid, NULL);
	ECDSA_UNLOCK();
	
	return ret;
}

/* Verify an ECDSA signature.
 * Note: return false for high signatures!
 * Inputs: 32-byte message m, public key point Q, signature: (32-byte r, scalar s).
 *	   Signature is invalid if r is zero.
 *	   Signature is invalid if s is zero.
 *	   Compute point R = (s^-1 * m * G + s^-1 * r * Q). Reject if R is infinity.
 *	   Signature is valid if R's x coordinate equals to r.
 */
int secp256k1_verify(const uint8_t *pub_key, const uint8_t *sig, const uint8_t *msg, uint32_t msg_len) 
{
	int ret = 0;
	
	ECDSA_LOCK();
	ret = ecdsa_verify(&secp256k1, HASHER_SHA3K, pub_key, sig, msg, msg_len);
	ECDSA_UNLOCK();
	
	return ret;
}

/** Recover an ECDSA public key from a signature.
 *  Returns: 0: successfully recovered
 *           1: Input secp256k1 sign struct was invalid 
 *	Out:	 poutput:	a 65-byte array computed from the signature. 
 *	In: 	 tosign:	a secp256k1 sign struct point.
 */
int secp256k1_recover(uint8_t *sig, uint8_t *msg, uint32_t msg_len, uint8_t recid, unsigned char *poutput)
{
	uint8_t hash[32];
	int ret = 0;

	hasher_Raw(HASHER_SHA3K, msg, msg_len, hash);

	ECDSA_LOCK();
	ret = ecdsa_recover_pub_from_sig(&secp256k1, poutput, sig, hash, recid);
	ECDSA_UNLOCK();
	
	return ret;
	
}

/** Compute an public key from a private key.
 *  Out:     pub_key:   a 65-byte array. 
 *  In:      privkey:   a 32-byte scalar with which to multiply the point
 */
void secp256k1_get_public_key65(const uint8_t *priv_key, uint8_t *pub_key)
{
	ECDSA_LOCK();
	ecdsa_get_public_key65(&secp256k1, priv_key, pub_key);
	ECDSA_UNLOCK();
}

/** Compute an EC Diffie-Hellman secret and return session key as result.
 *  Returns: 0: exponentiation was successful
 *           1: Input public key was invalid (zero or overflow)
 *  Out:     session_key:   a 65-byte array which will be populated by an ECDH
 *                       	secret computed from the priv_key and pub_key
 *  In:      pubkey:     	a pointer to a secp256k1_pubkey containing an
 *                       	initialized public key
 *           privkey:    	a 32-byte scalar with which to multiply the point
 */
int secp256k1_ecdh(const uint8_t *priv_key, const uint8_t *pub_key, uint8_t *session_key)
{
	int ret = 0;
	
	ECDSA_LOCK();
	ret = ecdh_multiply(&secp256k1, priv_key, pub_key, session_key);
	ECDSA_UNLOCK();
	
	return ret;
}

static int check_privkey(const uint8_t priv_key_array[32])
{
    bignum256 priv_key_bn256;
    bignum256 priv_key_max_bn256;

    // Valid private key value (as a UINT256) for Ethereum is [1, n-1], where n is
    // 0xFFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE BAAEDCE6 AF48A03B BFD25E8C D0364141
    const uint32_t priv_key_max_u256[8] = {0xD0364141, 0xBFD25E8C, 0xAF48A03B, 0xBAAEDCE6,
                                         0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

    if( priv_key_array == NULL )
    {
        return -1;
    }

    // Check whether private_key_array is in valid range
    
    // Convert private key from UINT256 to Bignum256 format
    bn_read_le(priv_key_array, &priv_key_bn256);

    if( bn_is_zero(&priv_key_bn256) != 0)
    {
        return -1;
    }

    // Convert priv_key_max_u256 from UINT256 to Bignum256 format
    bn_read_le((const uint8_t *)priv_key_max_u256, &priv_key_max_bn256);

    // Check whether priv_key_bn256 is less than priv_key_max_bn256
    if( bn_is_less(&priv_key_bn256, &priv_key_max_bn256) == 0 )
    {
        return -1;
    }

    return 0;

}

static int generate_privkey(uint8_t priv_key_array[32])
{
    uint32_t key_try_count;
    int result;

    if( priv_key_array == NULL )
    {
        return -1;
    }
    
    // Try at most 100 times to find a random number fits for Ethereum private key
    for( key_try_count = 0; key_try_count < 100; key_try_count++ )
    {
    
        random_buffer(priv_key_array, 32);

        result = check_privkey(priv_key_array);

        if( result == 0 )
        {
            break;
        }            
    }

    return result;
    
}

int ecdsa_keypair_create(uint8_t pri_key[32], uint8_t pub_key[65])
{
    int ret = 0;
    
	ECDSA_LOCK();
    ret = generate_privkey(pri_key);
    if( ret != 0 )
    {
		ECDSA_UNLOCK();
        return -1;
    }

    ecdsa_get_public_key65(&secp256k1, pri_key, pub_key);
    if (pub_key[0] != 0x04)
    {
		ECDSA_UNLOCK();
        return -1;
    }
	ECDSA_UNLOCK();
    return ret;
}


