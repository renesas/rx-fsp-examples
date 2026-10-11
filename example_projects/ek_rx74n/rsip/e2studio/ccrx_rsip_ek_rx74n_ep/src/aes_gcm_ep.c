/***********************************************************************************************************************
 * File Name    : aes_gcm_ep.c
 * Description  : This file contains data structures and functions used in aes_gcm_entry().
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rsip_example.h"
#include "aes_gcm_ep.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/

 
/*******************************************************************************************************************//**
 *  @brief       Initialize AES-GCM context, generate wrapped AES-256 key and nonce using RSIP.
 *  @param[in]   ctx    Pointer to AES-GCM context structure
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t aes_gcm_init(aes_gcm_ctx_t * ctx)
{
    fsp_err_t err;

    ctx->key.type    = RSIP_KEY_TYPE_AES_256;
    ctx->key.p_value = ctx->key_buf;

    /* Generate AES key */
    err = R_RSIP_KeyGenerate(&g_rsip_ctrl, &ctx->key);
    if (FSP_SUCCESS != err) return err;

    /* Generate nonce */
    err = R_RSIP_RandomNumberGenerate(&g_rsip_ctrl, ctx->nonce);
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Perform AES-GCM encryption using RSIP, including AAD processing and authentication tag generation.
 *  @param[in]   ctx          Pointer to AES-GCM context
 *  @param[in]   plain        Pointer to input plaintext
 *  @param[in]   plain_len    Length of plaintext
 *  @param[in]   aad          Pointer to additional authenticated data
 *  @param[in]   aad_len      Length of AAD
 *  @param[out]  cipher       Output buffer for ciphertext
 *  @param[out]  cipher_len   Output length of ciphertext
 *  @param[out]  tag          Output authentication tag
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t aes_gcm_encrypt(aes_gcm_ctx_t * ctx,
                          const uint8_t * plain,
                          uint32_t        plain_len,
                          const uint8_t * aad,
                          uint32_t        aad_len,
                          uint8_t *       cipher,
                          uint32_t *      cipher_len,
                          uint8_t *       tag)
{
    fsp_err_t err;
    uint32_t out_len_tmp = 0;

    *cipher_len = 0;

    err = R_RSIP_AES_AEAD_Init(&g_rsip_ctrl,
                               RSIP_AES_AEAD_MODE_GCM_ENC,
                               &ctx->key,
                               ctx->nonce,
                               GCM_NONCE_LEN);
    if (err) return err;

    err = R_RSIP_AES_AEAD_AADUpdate(&g_rsip_ctrl, aad, aad_len);
    if (err) return err;

    err = R_RSIP_AES_AEAD_Update(&g_rsip_ctrl,
                                 plain,
                                 plain_len,
                                 cipher,
                                 &out_len_tmp);
    if (err) return err;

    *cipher_len += out_len_tmp;

    err = R_RSIP_AES_AEAD_Finish(&g_rsip_ctrl,
                                 cipher + *cipher_len,
                                 &out_len_tmp,
                                 tag);
    if (err) return err;

    *cipher_len += out_len_tmp;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 *  @brief       Perform AES-GCM decryption using RSIP and output plaintext without verification.
 *  @param[in]   ctx          Pointer to AES-GCM context
 *  @param[in]   cipher       Pointer to input ciphertext
 *  @param[in]   cipher_len   Length of ciphertext
 *  @param[in]   aad          Pointer to additional authenticated data (must match encryption)
 *  @param[in]   aad_len      Length of AAD
 *  @param[out]  plain        Output buffer for plaintext
 *  @param[out]  plain_len    Output length of plaintext
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t aes_gcm_decrypt(aes_gcm_ctx_t * ctx,
                          const uint8_t * cipher,
                          uint32_t        cipher_len,
                          const uint8_t * aad,
                          uint32_t        aad_len,
                          uint8_t *       plain,
                          uint32_t *      plain_len)
{
    fsp_err_t err;
    uint32_t out_len_tmp = 0;

    *plain_len = 0;

    err = R_RSIP_AES_AEAD_Init(&g_rsip_ctrl,
                               RSIP_AES_AEAD_MODE_GCM_DEC,
                               &ctx->key,
                               ctx->nonce,
                               GCM_NONCE_LEN);
    if (err) return err;

    err = R_RSIP_AES_AEAD_AADUpdate(&g_rsip_ctrl, aad, aad_len);
    if (err) return err;

    err = R_RSIP_AES_AEAD_Update(&g_rsip_ctrl,
                                 cipher,
                                 cipher_len,
                                 plain,
                                 &out_len_tmp);
    if (err) return err;

    *plain_len += out_len_tmp;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 *  @brief       Verify AES-GCM authentication tag to ensure data integrity and authenticity.
 *  @param[in]   ctx          Pointer to AES-GCM context
 *  @param[in,out] plain_len  Current plaintext length (may be updated after verify)
 *  @param[in]   plain        Pointer to decrypted plaintext buffer
 *  @param[in]   tag          Pointer to authentication tag
 *  @param[in]   tag_len      Length of authentication tag
 *  @retval      FSP_SUCCESS if verification succeeds, otherwise error code
 **********************************************************************************************************************/
fsp_err_t aes_gcm_verify(aes_gcm_ctx_t * ctx,
                         uint8_t *       plain,
                         uint32_t *      plain_len,
                         const uint8_t * tag,
                         uint32_t        tag_len)
{
    fsp_err_t err;
    uint32_t out_len_tmp = 0;

    err = R_RSIP_AES_AEAD_Verify(&g_rsip_ctrl,
                                 plain + *plain_len,
                                 &out_len_tmp,
                                 tag,
                                 tag_len);
    if (err) return err;

    *plain_len += out_len_tmp;

    return FSP_SUCCESS;
}
