/***********************************************************************************************************************
 * File Name    : ecdsa_ep.c
 * Description  : This file contains data structures and functions used in ecdsa_entry().
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rsip_example.h"
#include "ecdsa_ep.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/

 
/*******************************************************************************************************************//**
 *  @brief       Generate ECC key pair (public and private key) using RSIP.
 *  @param[in]   ctx    Pointer to ECC context structure
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t ecc_keypair_generate(ecdsa_ctx_t * ctx)
{
    fsp_err_t err;

    ctx->public_key.type    = RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC;
    ctx->public_key.p_value = ctx->public_key_buf;

    ctx->private_key.type    = RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE;
    ctx->private_key.p_value = ctx->private_key_buf;

    err = R_RSIP_KeyPairGenerate(&g_rsip_ctrl, &ctx->public_key, &ctx->private_key);

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Compute SHA-256 hash for the given message using RSIP.
 *  @param[in]   message    Pointer to input message
 *  @param[in]   msg_len    Length of the message
 *  @param[out]  hash       Output buffer for SHA-256 digest
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t sha256_hash(const uint8_t * message, uint32_t msg_len, uint8_t * hash)
{
    return R_RSIP_SHA_Compute(&g_rsip_ctrl, RSIP_HASH_TYPE_SHA256, message, msg_len, hash);
}

/*******************************************************************************************************************//**
 *  @brief       Generate ECDSA signature from input hash using ECC private key.
 *  @param[in]   ctx         Pointer to ECC context containing private key
 *  @param[in]   hash        Pointer to input hash (e.g., SHA-256 digest)
 *  @param[out]  signature   Output buffer for signature
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t ecdsa_sign(ecdsa_ctx_t * ctx, uint8_t * hash, uint8_t * signature)
{
    return R_RSIP_ECDSA_Sign(&g_rsip_ctrl, &ctx->private_key, hash, signature);
}

/*******************************************************************************************************************//**
 *  @brief       Verify ECDSA signature using ECC public key.
 *  @param[in]   ctx         Pointer to ECC context containing public key
 *  @param[in]   hash        Pointer to input hash (must match signed data)
 *  @param[in]   signature   Pointer to signature to verify
 *  @retval      FSP_SUCCESS if verification succeeds, otherwise error code
 **********************************************************************************************************************/
fsp_err_t ecdsa_verify(ecdsa_ctx_t * ctx, uint8_t * hash, uint8_t * signature)
{
    return R_RSIP_ECDSA_Verify(&g_rsip_ctrl, &ctx->public_key, hash, signature);
}
