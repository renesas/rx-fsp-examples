/***********************************************************************************************************************
 * File Name    : sha256_ep.h
 * Description  : This file contains data structures and functions used in sha256_ep.c
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SHA256_EP_H_
#define SHA256_EP_H_

#include "common_utils.h"
#include "rsip_example.h"

/* Function Declaration Section */
fsp_err_t hash_single(const uint8_t * msg, uint32_t msg_len, uint8_t * digest);
fsp_err_t hash_multi_init(void);
fsp_err_t hash_multi_update(const uint8_t * msg, uint32_t msg_len);
fsp_err_t hash_multi_finish(uint8_t * digest);
bool hash_compare(uint8_t * digest, const uint8_t * ref_hash);

#endif /* SHA256_EP_H_ */
