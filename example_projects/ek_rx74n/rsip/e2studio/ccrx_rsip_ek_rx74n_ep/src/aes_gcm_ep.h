/***********************************************************************************************************************
 * File Name    : aes_gcm_ep.h
 * Description  : This file contains data structures and functions used in aes_gcm_ep.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef AES_GCM_EP_H_
#define AES_GCM_EP_H_

#include "common_utils.h"
#include "rsip_example.h"


 /* AES-GCM context structure used to store wrapped key, key buffer, and nonce. */
typedef struct
{
    rsip_wrapped_key_t key;
    uint8_t            key_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_AES_256)];
    uint8_t            nonce[GCM_NONCE_LEN];
} aes_gcm_ctx_t;

/* Function Declaration Section */
fsp_err_t aes_gcm_init(aes_gcm_ctx_t * ctx);
fsp_err_t aes_gcm_encrypt(aes_gcm_ctx_t * ctx,
                          const uint8_t * plain,
                          uint32_t        plain_len,
                          const uint8_t * aad,
                          uint32_t        aad_len,
                          uint8_t *       cipher,
                          uint32_t *      cipher_len,
                          uint8_t *       tag);
fsp_err_t aes_gcm_decrypt(aes_gcm_ctx_t * ctx,
                          const uint8_t * cipher,
                          uint32_t        cipher_len,
                          const uint8_t * aad,
                          uint32_t        aad_len,
                          uint8_t *       plain,
                          uint32_t *      plain_len);
fsp_err_t aes_gcm_verify(aes_gcm_ctx_t * ctx,
                         uint8_t *       plain,
                         uint32_t *      plain_len,
                         const uint8_t * tag,
                         uint32_t        tag_len);
#endif /* AES_GCM_EP_H_ */
