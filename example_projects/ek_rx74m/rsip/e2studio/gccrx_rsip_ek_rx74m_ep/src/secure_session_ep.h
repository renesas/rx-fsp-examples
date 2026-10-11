/***********************************************************************************************************************
 * File Name    : secure_session_ep.h
 * Description  : This file contains data structures and functions used in secure_session_ep.c
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SECURE_SESSION_EP_H_
#define SECURE_SESSION_EP_H_

#include "common_utils.h"
#include "rsip_example.h"

/* Secure session context structure used to store nonces, transcript data, wrapped ECC keys, 
 * shared secret, derived session key, and AES-GCM buffers for secure communication. */
typedef struct
{
    uint8_t client_nonce[NONCE16_LEN] BSP_ALIGN_VARIABLE(4);
    uint8_t server_nonce[NONCE16_LEN] BSP_ALIGN_VARIABLE(4);
    uint8_t gcm_nonce[GCM_NONCE_LEN]  BSP_ALIGN_VARIABLE(4);

    uint8_t transcript[TRANSCRIPT_LEN] BSP_ALIGN_VARIABLE(4);
    uint8_t transcript_hash[SHA256_HASH_SIZE] BSP_ALIGN_VARIABLE(4);

    uint8_t signature[SECP256R1_KEY_SIZE * 2] BSP_ALIGN_VARIABLE(4);

    uint8_t client_pub_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC)] BSP_ALIGN_VARIABLE(4);
    uint8_t client_prv_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE)] BSP_ALIGN_VARIABLE(4);
    uint8_t server_pub_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC)] BSP_ALIGN_VARIABLE(4);
    uint8_t server_prv_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE)] BSP_ALIGN_VARIABLE(4);

    rsip_wrapped_key_t client_pub;
    rsip_wrapped_key_t client_prv;
    rsip_wrapped_key_t server_pub;
    rsip_wrapped_key_t server_prv;

    rsip_wrapped_secret_t wrapped_secret;

    uint8_t wrapped_kdf_key_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_KDF_HMAC_SHA256)] BSP_ALIGN_VARIABLE(4);
    rsip_wrapped_key_t wrapped_kdf_key;

    uint8_t wrapped_dkm_buf[KDF_HMAC_SHA256_DKM_BYTE_SIZE] BSP_ALIGN_VARIABLE(4);

    uint8_t wrapped_session_key_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_AES_256)] BSP_ALIGN_VARIABLE(4);
    rsip_wrapped_key_t wrapped_session_key;

    uint8_t cipher[GCM_TEXT_LEN * 2] BSP_ALIGN_VARIABLE(4);
    uint8_t plain_out[GCM_TEXT_LEN * 2] BSP_ALIGN_VARIABLE(4);
    uint8_t tag[GCM_TAG_LEN] BSP_ALIGN_VARIABLE(4);

} secure_session_ctx_t;

/* Function Declaration Section */
void secure_session_ctx_init(secure_session_ctx_t * ctx);
fsp_err_t secure_session_generate_nonces(secure_session_ctx_t * ctx);
fsp_err_t secure_session_build_transcript(secure_session_ctx_t * ctx);
fsp_err_t secure_session_authenticate(secure_session_ctx_t * ctx);
fsp_err_t secure_session_key_agreement(secure_session_ctx_t * ctx);
fsp_err_t secure_session_derive_key(secure_session_ctx_t * ctx);
fsp_err_t secure_session_encrypt(secure_session_ctx_t * ctx, uint32_t * cipher_len);
fsp_err_t secure_session_decrypt_verify(secure_session_ctx_t * ctx, uint32_t cipher_len);

#endif /* SECURE_SESSION_EP_H_ */
