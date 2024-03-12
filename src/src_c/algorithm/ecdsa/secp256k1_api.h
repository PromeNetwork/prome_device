#ifndef SECP256K1_API_H
#define SECP256K1_API_H

#include "hasher.h"


int secp256k1_sign(const uint8_t *priv_key, const uint8_t *msg, uint32_t msg_len, uint8_t *sig, uint8_t *recid);
int secp256k1_verify(const uint8_t *pub_key, const uint8_t *sig, const uint8_t *msg, uint32_t msg_len);
int secp256k1_recover(uint8_t *sig, uint8_t *msg, uint32_t msg_len, uint8_t recid, unsigned char *poutput);
void secp256k1_get_public_key65(const uint8_t *priv_key, uint8_t *pub_key);
int secp256k1_ecdh(const uint8_t *priv_key, const uint8_t *pub_key, uint8_t *session_key);
int ecdsa_keypair_create(uint8_t pri_key[32], uint8_t pub_key[65]);

#endif

