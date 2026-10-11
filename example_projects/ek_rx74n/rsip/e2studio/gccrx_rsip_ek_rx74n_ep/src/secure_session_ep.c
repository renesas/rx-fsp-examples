/***********************************************************************************************************************
 * File Name    : secure_session_ep.c
 * Description  : This file contains data structures and functions used in secure_session_entry().
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rsip_example.h"
#include "secure_session_ep.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
static const uint8_t gcm_aad[GCM_AAD_LEN] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

static const uint8_t gcm_plain[GCM_TEXT_LEN] =
{
    0x52, 0x65, 0x6E, 0x65, 0x73, 0x61, 0x73, 0x20,
    0x52, 0x53, 0x49, 0x50, 0x20, 0x53, 0x65, 0x73,
    0x73, 0x69, 0x6F, 0x6E, 0x20, 0x44, 0x65, 0x6D,
    0x6F, 0x20, 0x50, 0x6F, 0x43, 0x21, 0x21, 0x00
};

/*******************************************************************************************************************//**
 *  @brief       Initialize secure session context and configure wrapped key structures for ECC, KDF and AES.
 *  @param[in]   ctx    Pointer to secure session context
 *  @retval      None
 **********************************************************************************************************************/
void secure_session_ctx_init(secure_session_ctx_t * ctx)
{
    memset(ctx, 0, sizeof(secure_session_ctx_t));

    ctx->client_pub.type    = RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC;
    ctx->client_pub.p_value = ctx->client_pub_buf;

    ctx->client_prv.type    = RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE;
    ctx->client_prv.p_value = ctx->client_prv_buf;

    ctx->server_pub.type     = RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC;
    ctx->server_pub.p_value  = ctx->server_pub_buf;

    ctx->server_prv.type     = RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE;
    ctx->server_prv.p_value  = ctx->server_prv_buf;

    ctx->wrapped_kdf_key.type    = RSIP_KEY_TYPE_KDF_HMAC_SHA256;
    ctx->wrapped_kdf_key.p_value = ctx->wrapped_kdf_key_buf;

    ctx->wrapped_session_key.type    = RSIP_KEY_TYPE_AES_256;
    ctx->wrapped_session_key.p_value = ctx->wrapped_session_key_buf;
}

/*******************************************************************************************************************//**
 *  @brief       Generate client and server nonces, and initialize GCM nonce using RSIP RNG.
 *  @param[in]   ctx    Pointer to secure session context
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t secure_session_generate_nonces(secure_session_ctx_t * ctx)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t tmp16[16] = {0};

    err = R_RSIP_RandomNumberGenerate(&g_rsip_ctrl, ctx->client_nonce);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_RandomNumberGenerate(&g_rsip_ctrl, ctx->server_nonce);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_RandomNumberGenerate(&g_rsip_ctrl, tmp16);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    memcpy(ctx->gcm_nonce, tmp16, GCM_NONCE_LEN);
    memset(tmp16, 0, sizeof(tmp16));

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Build handshake transcript using magic value, client nonce, and server nonce,
 *               then compute SHA-256 hash of the transcript.
 *  @param[in]   ctx    Pointer to secure session context
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t secure_session_build_transcript(secure_session_ctx_t * ctx)
{
    static const uint8_t magic[TRANSCRIPT_MAGIC_LEN] = { 'R','E','N','E','S','A','S' };

    memcpy(ctx->transcript, magic, TRANSCRIPT_MAGIC_LEN);
    memcpy(ctx->transcript + TRANSCRIPT_MAGIC_LEN, ctx->client_nonce, NONCE16_LEN);
    memcpy(ctx->transcript + TRANSCRIPT_MAGIC_LEN + NONCE16_LEN, ctx->server_nonce, NONCE16_LEN);

    return R_RSIP_SHA_Compute(&g_rsip_ctrl,
                              RSIP_HASH_TYPE_SHA256,
                              ctx->transcript,
                              sizeof(ctx->transcript),
                              ctx->transcript_hash);
}

/*******************************************************************************************************************//**
 *  @brief       Perform ECDSA authentication by generating server key pair, signing transcript hash,
 *               and verifying the signature using the generated public key.
 *  @param[in]   ctx    Pointer to secure session context
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t secure_session_authenticate(secure_session_ctx_t * ctx)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Server side */
    err = R_RSIP_KeyPairGenerate(&g_rsip_ctrl, &ctx->server_pub, &ctx->server_prv); /* Server side */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Server side */
    err = R_RSIP_ECDSA_Sign(&g_rsip_ctrl,
                            &ctx->server_prv,
                            ctx->transcript_hash,
                            ctx->signature);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Client uses server_pub to verify the authentication */
    err = R_RSIP_ECDSA_Verify(&g_rsip_ctrl,
                              &ctx->server_pub,
                              ctx->transcript_hash,
                              ctx->signature);
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Perform ECDH key agreement by generating client key pair and deriving shared secret
 *               using server private key and client public key.
 *  @param[in]   ctx    Pointer to secure session context
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t secure_session_key_agreement(secure_session_ctx_t * ctx)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_RSIP_KeyPairGenerate(&g_rsip_ctrl, &ctx->client_pub, &ctx->client_prv); /* Client side */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Server side create a shared secret from server_prv and client_pub */
    err = R_RSIP_ECDH_KeyAgree(&g_rsip_ctrl,
                               &ctx->server_prv,
                               &ctx->client_pub,
                               &ctx->wrapped_secret);
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Derive AES session key from ECDH shared secret using HMAC-based KDF and transcript data.
 *  @param[in]   ctx    Pointer to secure session context
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t secure_session_derive_key(secure_session_ctx_t * ctx)
{
    fsp_err_t err = FSP_SUCCESS;
    rsip_wrapped_dkm_t * p_wrapped_dkm = (rsip_wrapped_dkm_t *) ctx->wrapped_dkm_buf;

    err = R_RSIP_KDF_HMAC_ECDHSecretKeyImport(&g_rsip_ctrl,
                                              &ctx->wrapped_secret,
                                              &ctx->wrapped_kdf_key);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_KDF_HMAC_Init(&g_rsip_ctrl, &ctx->wrapped_kdf_key);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_KDF_HMAC_Update(&g_rsip_ctrl,
                                 ctx->transcript,
                                 sizeof(ctx->transcript));
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_KDF_HMAC_SignFinish(&g_rsip_ctrl, p_wrapped_dkm);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_KDF_DerivedKeyImport(&g_rsip_ctrl,
                                      p_wrapped_dkm,
                                      0U,
                                      &ctx->wrapped_session_key);
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Encrypt application data using derived AES-GCM session key, including AAD processing
 *               and authentication tag generation.
 *  @param[in]   ctx          Pointer to secure session context
 *  @param[out]  cipher_len   Output length of ciphertext
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t secure_session_encrypt(secure_session_ctx_t * ctx, uint32_t * cipher_len)
{
    fsp_err_t err;
    uint32_t out_len_tmp = {RESET_VALUE};

    *cipher_len = 0U;

    err = R_RSIP_AES_AEAD_Init(&g_rsip_ctrl,
                               RSIP_AES_AEAD_MODE_GCM_ENC,
                               &ctx->wrapped_session_key,
                               ctx->gcm_nonce,
                               GCM_NONCE_LEN);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_AES_AEAD_AADUpdate(&g_rsip_ctrl, gcm_aad, GCM_AAD_LEN);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_AES_AEAD_Update(&g_rsip_ctrl,
                                 gcm_plain,
                                 GCM_TEXT_LEN,
                                 ctx->cipher,
                                 &out_len_tmp);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    *cipher_len += out_len_tmp;

    err = R_RSIP_AES_AEAD_Finish(&g_rsip_ctrl,
                                 ctx->cipher + *cipher_len,
                                 &out_len_tmp,
                                 ctx->tag);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    *cipher_len += out_len_tmp;

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Decrypt ciphertext and verify AES-GCM authentication tag to ensure data integrity
 *               and authenticity of received data.
 *  @param[in]   ctx          Pointer to secure session context
 *  @param[in]   cipher_len   Length of ciphertext
 *  @retval      FSP_SUCCESS if verification succeeds, otherwise error code
 *  @note        Decrypted data must not be trusted until verification is successful
 **********************************************************************************************************************/
fsp_err_t secure_session_decrypt_verify(secure_session_ctx_t * ctx, uint32_t cipher_len)
{
    fsp_err_t err;
    uint32_t plain_len = {RESET_VALUE};
    uint32_t plain_len_tmp = {RESET_VALUE};

    err = R_RSIP_AES_AEAD_Init(&g_rsip_ctrl,
                               RSIP_AES_AEAD_MODE_GCM_DEC,
                               &ctx->wrapped_session_key,
                               ctx->gcm_nonce,
                               GCM_NONCE_LEN);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_AES_AEAD_AADUpdate(&g_rsip_ctrl, gcm_aad, GCM_AAD_LEN);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_RSIP_AES_AEAD_Update(&g_rsip_ctrl,
                                 ctx->cipher,
                                 cipher_len,
                                 ctx->plain_out,
                                 &plain_len_tmp);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    plain_len += plain_len_tmp;

    err = R_RSIP_AES_AEAD_Verify(&g_rsip_ctrl,
                                 ctx->plain_out + plain_len,
                                 &plain_len_tmp,
                                 ctx->tag,
                                 GCM_TAG_LEN);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    plain_len += plain_len_tmp;

    if (0 != memcmp(gcm_plain, ctx->plain_out, GCM_TEXT_LEN))
    {
        return FSP_ERR_INVALID_DATA;
    }

    return FSP_SUCCESS;
}
