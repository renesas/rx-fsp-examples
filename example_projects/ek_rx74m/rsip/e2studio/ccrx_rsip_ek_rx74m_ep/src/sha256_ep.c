/***********************************************************************************************************************
 * File Name    : sha256_ep.c
 * Description  : This file contains data structures and functions used in sha256_entry().
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rsip_example.h"
#include "sha256_ep.h"

/*******************************************************************************************************************//**
 *  @brief       Compute SHA-256 hash of input message using single-part (one-shot) API.
 *  @param[in]   msg       Pointer to input message
 *  @param[in]   msg_len   Length of the message
 *  @param[out]  digest    Output buffer for SHA-256 digest
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t hash_single(const uint8_t * msg, uint32_t msg_len, uint8_t * digest)
{
    return R_RSIP_SHA_Compute(&g_rsip_ctrl, RSIP_HASH_TYPE_SHA256, msg, msg_len, digest);
}

/*******************************************************************************************************************//**
 *  @brief       Initialize SHA-256 hashing operation for multi-part (streaming) API.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t hash_multi_init(void)
{
    return R_RSIP_SHA_Init(&g_rsip_ctrl, RSIP_HASH_TYPE_SHA256);
}

/*******************************************************************************************************************//**
 *  @brief       Update SHA-256 hashing operation with input message chunk (streaming mode).
 *  @param[in]   msg       Pointer to input message chunk
 *  @param[in]   msg_len   Length of the message chunk
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t hash_multi_update(const uint8_t * msg, uint32_t msg_len)
{
    return R_RSIP_SHA_Update(&g_rsip_ctrl, msg, msg_len);
}

/*******************************************************************************************************************//**
 *  @brief       Finalize SHA-256 hashing operation and output message digest.
 *  @param[out]  digest    Output buffer for SHA-256 digest
 *  @retval      FSP_SUCCESS on success, otherwise error code
 **********************************************************************************************************************/
fsp_err_t hash_multi_finish(uint8_t * digest)
{
    return R_RSIP_SHA_Finish(&g_rsip_ctrl, digest);
}

/*******************************************************************************************************************//**
 *  @brief       Compare computed hash with reference hash value.
 *  @param[in]   digest     Pointer to computed hash
 *  @param[in]   ref_hash   Pointer to reference hash
 *  @retval      true if hashes match, otherwise false
 **********************************************************************************************************************/
bool hash_compare(uint8_t * digest, const uint8_t * ref_hash)
{
    return (0 == memcmp(digest, ref_hash, SHA256_HASH_SIZE));
}
