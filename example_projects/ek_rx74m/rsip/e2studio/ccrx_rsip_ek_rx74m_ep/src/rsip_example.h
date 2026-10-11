/***********************************************************************************************************************
 * File Name    : rsip_example.h
 * Description  : This file contains data structures and functions used in rsip_example.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef RSIP_EXAMPLE_H_
#define RSIP_EXAMPLE_H_

#define SECP256R1_KEY_SIZE   (32)
#define SHA256_HASH_SIZE     (32)
#define TRANSCRIPT_MAGIC_LEN (7U)
#define NONCE16_LEN          (16U)
#define TRANSCRIPT_LEN       (TRANSCRIPT_MAGIC_LEN + NONCE16_LEN + NONCE16_LEN)
#define GCM_NONCE_LEN        (12U)
#define GCM_AAD_LEN          (8U)
#define GCM_TAG_LEN          (16U)
#define GCM_TEXT_LEN         (32U)
#define KDF_HMAC_SHA256_DKM_BYTE_SIZE    (RSIP_BYTE_SIZE_WRAPPED_DKM_HEADER + RSIP_BYTE_SIZE_WRAPPED_DKM_BLOCK_SHA256)

#define PRINT_PASS(msg)      APP_PRINT("[PASS] " msg "\r\n")

/* This macro is used to indicate the buffer size */
#define BUFF_SIZE         (15U)
/* This macro is used to display the EP information */
#define RSIP_EP_INFO            "\r\nThis example project demonstrates the basic functionality of the RSIP driver on" \
                                "\r\nRenesas RX MCUs using the Renesas Flexible Software Package (FSP). The user can" \
                                "\r\ninteract with the application via terminal to execute cryptography operations. " \
                                "\r\nThe operation status and data content are displayed in the terminal.\r\n"

/* This macro is used to display the Main menu */
#define EP_MENU_RSIP_MENU       "\r\n-----------------------------------------------    "\
                                "\r\n MENU OPTIONS\r\n                                  "\
                                "\r\n   [1] AES-GCM encryption/decryption               "\
                                "\r\n   [2] Calculating the SHA-256 message digest      "\
                                "\r\n   [3] ECDSA signature/verify                      "\
                                "\r\n   [4] Secure Session                              "\
                                "\r\n   [5] Exit                                        "\
                                "\r\n-----------------------------------------------\r\n"
#define ENTER_MENU_OPTION       "\r\nPlease enter your option (Press any other key to print the Main menu):\r\n"
#define AES_GCM_OPTION          (1U)
#define SHA_256_OPTION          (2U)
#define ECDSA_OPTION            (3U)
#define SECURE_SESSION_OPTION   (4U)
#define EXIT                    (5U)

/* Function Declaration Section */
void rsip_operation_entry(void);
void aes_gcm_entry(void);
void sha256_entry(void);
void ecdsa_entry(void);
void secure_session_entry(void);

#endif /* RSIP_EXAMPLE_H_ */
