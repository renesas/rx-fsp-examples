/***********************************************************************************************************************
 * File Name    : ecdsa_ep.h
 * Description  : This file contains data structures and functions used in ecdsa_ep.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ECC_EP_H_
#define ECC_EP_H_

#include "common_utils.h"
#include "rsip_example.h"

/* ECC context structure used to store RSIP wrapped key pairs (secp256r1) and their aligned key buffers. */
typedef struct
{
    rsip_wrapped_key_t public_key;
    rsip_wrapped_key_t private_key;
    uint8_t public_key_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC)] BSP_ALIGN_VARIABLE(4);
    uint8_t private_key_buf[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE)] BSP_ALIGN_VARIABLE(4);
} ecdsa_ctx_t;

/* Function Declaration Section */
fsp_err_t ecc_keypair_generate(ecdsa_ctx_t * ctx);
fsp_err_t sha256_hash(const uint8_t * message, uint32_t msg_len, uint8_t * hash);
fsp_err_t ecdsa_sign(ecdsa_ctx_t * ctx, uint8_t * hash, uint8_t * signature);
fsp_err_t ecdsa_verify(ecdsa_ctx_t * ctx, uint8_t * hash, uint8_t * signature);

#endif /* ECC_EP_H_ */
