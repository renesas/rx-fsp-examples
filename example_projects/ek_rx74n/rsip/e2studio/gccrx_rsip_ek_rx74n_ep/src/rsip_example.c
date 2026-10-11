/***********************************************************************************************************************
 * File Name    : rsip_example.c
 * Description  : This file contains data structures and entry functions used in hal_entry().
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rsip_example.h"
#include "aes_gcm_ep.h"
#include "sha256_ep.h"
#include "ecdsa_ep.h"
#include "secure_session_ep.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
extern bsp_leds_t g_bsp_leds;

/* AES-GCM - Context, AAD and plain text message */
aes_gcm_ctx_t g_gcm_ctx;
static const uint8_t g_gcm_aad[GCM_AAD_LEN]=
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

/* Plaintext input for AES-GCM */
static const uint8_t g_gcm_plain[GCM_TEXT_LEN] =
{
    0x52, 0x65, 0x6E, 0x65, 0x73, 0x61, 0x73, 0x20,
    0x52, 0x53, 0x49, 0x50, 0x20, 0x53, 0x65, 0x73,
    0x73, 0x69, 0x6F, 0x6E, 0x20, 0x44, 0x65, 0x6D,
    0x6F, 0x20, 0x50, 0x6F, 0x43, 0x21, 0x21, 0x00
};

/* SHA-256 input test data */
static const uint8_t g_sha_message[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
};

/* Expected SHA-256 digests */
const uint8_t g_message_digest_single[] =
{
     0x4d, 0xbd, 0xc2, 0xb2, 0xb6, 0x2c, 0xb0, 0x07, 0x49, 0x78, 0x5b, 0xc8, 0x42, 0x02, 0x23, 0x6d,
     0xbc, 0x37, 0x77, 0xd7, 0x46, 0x60, 0x61, 0x1b, 0x8e, 0x58, 0x81, 0x2f, 0x0c, 0xfd, 0xe6, 0xc3
};

const uint8_t g_message_digest_multi[] =
{
     0xc3, 0x5f, 0x22, 0x9e, 0x7e, 0x93, 0x4c, 0x5e, 0xed, 0xf4, 0x00, 0x7e, 0x5d, 0x9c, 0x14, 0x66,
     0xd8, 0xf7, 0x52, 0xff, 0xf5, 0x07, 0x2a, 0x6b, 0x85, 0x97, 0xd1, 0xeb, 0x30, 0x26, 0x3e, 0xe7
};

/* ECDSA context */
ecdsa_ctx_t g_ecdsa_ctx;

/* Message for ECDSA signing */
static const uint8_t g_ecdsa_message[] = "sample";

/* Secure session context */
secure_session_ctx_t g_secure_session_ctx;

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
static void rsip_init(void);
static void rsip_deinit(void);
static void handle_error(fsp_err_t err, char *err_str);

/*******************************************************************************************************************//**
 *  @brief       Main entry function for RSIP example project. Initializes RSIP, displays menu,
 *               and handles user selection to execute different cryptographic use cases.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void rsip_operation_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Version information */
    fsp_pack_version_t version;

    /* Fetch FSP version to display on console */
    R_FSP_VersionGet(&version);

    /* User option */
    uint32_t user_option   = RESET_VALUE;

    /* Read buffer */
    char rByte[BUFF_SIZE]  = {RESET_VALUE};
    _Bool b_exit_demo = false;

#if (USE_VIRTUAL_COM == 1)
    err = (fsp_err_t) TERM_INIT();
        if (FSP_SUCCESS != err)
        {
            /* Turn ON error LED to indicate uart_init failed */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                              (bsp_io_level_t) BSP_IO_LEVEL_HIGH);
            /* Error trap */
            ERROR_TRAP;
        }
#endif

    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(RSIP_EP_INFO);

    /* Initialize the driver */
    rsip_init();

    APP_PRINT(EP_MENU_RSIP_MENU);
    APP_PRINT(ENTER_MENU_OPTION);

    while (!b_exit_demo)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte); /* Read user input from terminal */

            /* Convert ASCII input to integer option */
            user_option = (uint32_t)(atoi((char *)rByte));
            switch(user_option)
            {
                case AES_GCM_OPTION:
                    /* Execute AES-GCM demo */
                    aes_gcm_entry();
                    APP_PRINT(ENTER_MENU_OPTION);
                    break;

                case SHA_256_OPTION:
                    /* Execute SHA-256 demo */
                    sha256_entry();
                    APP_PRINT(ENTER_MENU_OPTION);
                    break;

                case ECDSA_OPTION:
                    /* Execute ECDSA demo */
                    ecdsa_entry();
                    APP_PRINT(ENTER_MENU_OPTION);
                    break;

                case SECURE_SESSION_OPTION:
                    /* Execute full secure session demo */
                    secure_session_entry();
                    APP_PRINT(ENTER_MENU_OPTION);
                    break;

                case EXIT:
                    APP_PRINT("\r\n");
                    APP_PRINT("Exiting the Example project...\r\n");
                    b_exit_demo = true; /* Exit loop */
                    break;

                default:
                    APP_PRINT("Invalid menu option!!\r\n");
                    APP_PRINT(EP_MENU_RSIP_MENU);
                    APP_PRINT(ENTER_MENU_OPTION);
                    break;
            }
        }
    }
    /* Close the driver */
    rsip_deinit();
    APP_PRINT("RSIP module has been closed!\r\n");
    APP_PRINT("Demo end here...\r\n");
    /*  */
    TERM_DEINIT(); /* De-initialize terminal */
}

/*******************************************************************************************************************//**
 *  @brief       Demonstrate AES-GCM encryption, decryption and authentication tag verification using RSIP.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void aes_gcm_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    static uint8_t cipher[64] = {RESET_VALUE};   /* Output buffer for ciphertext */
    static uint8_t plain[64] = {RESET_VALUE};    /* Output buffer for decrypted data */
    static uint8_t tag[16] = {RESET_VALUE};      /* Authentication tag */

    static uint32_t cipher_len = {RESET_VALUE};  /* Actual ciphertext length */
    static uint32_t plain_len = {RESET_VALUE};   /* Actual plaintext length */

    APP_PRINT("\r\n  === AES-GCM encryption/decryption ===\r\n");

    /* Init key + nonce */
    err = aes_gcm_init(&g_gcm_ctx);
    handle_error(err, "[FAIL] aes_gcm_init()\r\n");
    PRINT_PASS("Generate AES-256 key + nonce ");

    /* Encrypt + generate tag */
    err = aes_gcm_encrypt(&g_gcm_ctx,
                          g_gcm_plain,
                          GCM_TEXT_LEN,
                          g_gcm_aad,
                          GCM_AAD_LEN,
                          cipher,
                          &cipher_len,
                          tag);
    handle_error(err, "[FAIL] aes_gcm_encrypt()\r\n");
    PRINT_PASS("AES-GCM encrypt");

    /* Decrypt ciphertext */
    err = aes_gcm_decrypt(&g_gcm_ctx,
                          cipher,
                          cipher_len,
                          g_gcm_aad,
                          GCM_AAD_LEN,
                          plain,
                          &plain_len);
    handle_error(err, "[FAIL] aes_gcm_decrypt()\r\n");
    PRINT_PASS("AES-GCM decrypt");

    /* Verify authentication tag */
    err = aes_gcm_verify(plain,
                         &plain_len,
                         tag,
                         GCM_TAG_LEN);
    handle_error(err, "[FAIL] aes_gcm_verify()\r\n");
    PRINT_PASS("AES-GCM verify");

    /* Verify output with reference plain */
    if (memcmp(g_gcm_plain, plain, GCM_TEXT_LEN) == 0)
    {
        APP_PRINT("[RESULT] AES-GCM SUCCESS\r\n");
    }
    else
    {
        APP_PRINT("[RESULT] AES-GCM MISMATCH\r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       Demonstrate SHA-256 hash calculation using both single-part and multi-part (streaming) APIs.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void sha256_entry(void)
{
    fsp_err_t err;

    /* Output buffers for SHA-256 results */
    static uint8_t digest_single[SHA256_HASH_SIZE] = {RESET_VALUE};
    static uint8_t digest_multi[SHA256_HASH_SIZE] = {RESET_VALUE};

    APP_PRINT("\r\n  === Calculating the SHA-256 message digest ===\r\n");

    /* Single-part API */
    /* Compute SHA-256 in one call */
    err = hash_single(g_sha_message, sizeof(g_sha_message), digest_single);
    handle_error(err, "[FAIL] hash_single()\r\n");
    PRINT_PASS("Generate SHA-256 message digest using single-part API");

    /* Verify output with reference digest */
    if (hash_compare(digest_single, g_message_digest_single))
    {
        APP_PRINT("[RESULT] Single-part hash SUCCESS\r\n");
    }
    else
    {
        APP_PRINT("[RESULT] Single-part hash mismatch\r\n");
    }

    /* Multi-part API section */
    /* Initialize streaming hash context */
    err = hash_multi_init();
    handle_error(err, "[FAIL] hash_multi_init()\r\n");
    PRINT_PASS("Start SHA-256 operation to use multi-part API");

    /* Demonstrate data streaming */
    /* Feed data in chunks (simulate streaming input) */
    err = hash_multi_update(g_sha_message, sizeof(g_sha_message));
    handle_error(err, "[FAIL] hash_multi_update()\r\n");
    PRINT_PASS("Input the first SHA message");

    err = hash_multi_update(g_sha_message, sizeof(g_sha_message));
    handle_error(err, "[FAIL] hash_multi_update()\r\n");
    PRINT_PASS("Input the second SHA message");

    /* Finalize hashing and output digest */
    err = hash_multi_finish(digest_multi);
    handle_error(err, "[FAIL] hash_multi_finish()\r\n");
    PRINT_PASS("Output SHA message digest");

    /* Verify output with reference digest */
    if (hash_compare(digest_multi, g_message_digest_multi))
    {
        APP_PRINT("[RESULT] Multi-part hash SUCCESS\r\n");
    }
    else
    {
        APP_PRINT("[RESULT] Multi-part hash mismatch\r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       Demonstrate ECDSA key generation, message hashing, signing and signature verification.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void ecdsa_entry(void)
{
    fsp_err_t err;

    /* Buffer for SHA-256 hash (input to ECDSA signing) */
    static uint8_t hash[SECP256R1_KEY_SIZE] = {RESET_VALUE};
    /* Buffer for ECDSA signature */
    static uint8_t signature[SECP256R1_KEY_SIZE * 2] = {RESET_VALUE};

    APP_PRINT("\r\n  === ECDSA Signature and Verification ===\r\n");

    /* Generate ECC key pair (private + public key stored in context) */
    err = ecc_keypair_generate(&g_ecdsa_ctx);
    handle_error(err, "[FAIL] ecc_keypair_generate()\r\n");
    PRINT_PASS("Generate ECC key pair");

    /* Hash the input message before signing (ECDSA signs hash, not raw data) */
    err = sha256_hash(g_ecdsa_message, sizeof(g_ecdsa_message), hash);
    handle_error(err, "[FAIL] sha256_hash()\r\n");
    PRINT_PASS("Generate SHA-256 message digest");

    /* Generate ECDSA signature using private key */
    err = ecdsa_sign(&g_ecdsa_ctx, hash, signature);
    handle_error(err, "[FAIL] ecdsa_sign()\r\n");
    PRINT_PASS("Generate an ECDSA signature");

    /* Verify signature using public key */
    err = ecdsa_verify(&g_ecdsa_ctx, hash, signature);
    handle_error(err, "[FAIL] ecdsa_verify()\r\n");
    PRINT_PASS("Verify an ECDSA signature");

    APP_PRINT("[RESULT] ECDSA operation SUCCESS\r\n");
}


/*******************************************************************************************************************//**
 *  @brief       Demonstrate a complete secure session flow including nonce generation, transcript hashing,
 *               authentication, key agreement, key derivation, and AES-GCM secure communication.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void secure_session_entry(void)
{
    fsp_err_t err;

    /* Output length of encrypted data (ciphertext length) */
    static uint32_t cipher_len = {RESET_VALUE};

    /* Initialize secure session context */
    secure_session_ctx_init(&g_secure_session_ctx);

    APP_PRINT("\r\n  === RSIP Secure Session ===\r\n");

    /* Generate random nonces */
    err = secure_session_generate_nonces(&g_secure_session_ctx);
    handle_error(err, "[FAIL] secure_session_generate_nonces()\r\n");
    PRINT_PASS("Generate nonces");

    /* Build transcript and compute hash */
    err = secure_session_build_transcript(&g_secure_session_ctx);
    handle_error(err, "[FAIL] secure_session_build_transcript()\r\n");
    PRINT_PASS("Build transcript + hash");

    /* Authenticate using ECDSA */
    err = secure_session_authenticate(&g_secure_session_ctx);
    handle_error(err, "[FAIL] secure_session_authenticate()\r\n");
    PRINT_PASS("ECDSA authenticate");

    /* Perform ECDH key agreement (derive shared secret) */
    err = secure_session_key_agreement(&g_secure_session_ctx);
    handle_error(err, "[FAIL] secure_session_key_agreement()\r\n");
    PRINT_PASS("ECDH key agreement");

    /* Derive symmetric key using KDF (e.g. derive AES key from shared secret) */
    err = secure_session_derive_key(&g_secure_session_ctx);
    handle_error(err, "[FAIL] secure_session_derive_key()\r\n");
    PRINT_PASS("Derive AES key (KDF)");

    /* Encrypt sample data using derived AES key (AES-GCM) */
    err = secure_session_encrypt(&g_secure_session_ctx, &cipher_len);
    handle_error(err, "[FAIL] secure_session_encrypt()\r\n");
    PRINT_PASS("AES-GCM encrypt");

    /* Decrypt and verify tag (ensure confidentiality + integrity + authenticity) */
    err = secure_session_decrypt_verify(&g_secure_session_ctx, cipher_len);
    handle_error(err, "[FAIL] secure_session_decrypt_verify()\r\n");
    PRINT_PASS("AES-GCM decrypt + verify");

    APP_PRINT("[RESULT] Secure session SUCCESS\r\n");

    /* Cleanup sensitive buffers */
    memset(&g_secure_session_ctx, 0, sizeof(g_secure_session_ctx));
}


/*******************************************************************************************************************//**
 *  @brief       Initialize RSIP module and open the RSIP driver.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
static void rsip_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_RSIP_Open(&g_rsip_ctrl, &g_rsip_cfg);
    handle_error(err, "R_RSIP_Open() failed\r\n");
}

/*******************************************************************************************************************//**
 *  @brief       Deinitialize RSIP module and close the RSIP driver.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
static void rsip_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_RSIP_Close(&g_rsip_ctrl);
    handle_error(err, "R_RSIP_Close() failed\r\n");
}
/*******************************************************************************************************************//**
 *  @brief       This function handle error if error occurred, closes all opened module, print and traps error.
 *  @param[in]   err       error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
