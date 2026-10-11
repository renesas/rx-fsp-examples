/*******************************************************************************************************************//**
 * File Name    : common_utils.h
 * Description  : This file contains data structures, macros, and functions commonly used in the EP.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
/* Standard C libraries */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* HAL data */
#include "hal_data.h"

/* Configuration dependent includes */
#if (USE_VIRTUAL_COM == 1)
    #include "SERIAL_TERM/serial.h"
#else
    #include "lowsrc/lowlvl.h"
    #include "lowsrc/lowsrc.h"
#endif /* USE_VIRTUAL_COM == 1 */

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Configuration table */
/*
+------------------------+-----------------------------------------------+-----------------+
| Console Type           | Configuration Macros                          | Compiler        |
+------------------------+-----------------------------------------------+-----------------+
| Tera Term Console      | USE_VIRTUAL_COM=1                             | GCC-RX or CC-RX |
+------------------------+-----------------------------------------------+-----------------+
| Renesas Virtual        | USE_VIRTUAL_COM=0                             | GCC-RX or CC-RX |
| Debug Console          | BSP_CFG_IO_LIB_ENABLE=1 (For CC-RX)           |                 |
+------------------------+-----------------------------------------------+-----------------+
*/

/* Common macro definitions */
#define ERR_LED                         (2U)
#define BIT_SHIFT_8                     (8U)                /* Bit shift value for 8 bits */
#define SIZE_64                         (64U)               /* Size value of 64 bytes */
#define LVL_ERR                         (1U)                /* Error level condition flag */
#define RESET_VALUE                     (0x00)              /* Reset value for initialization */
#define NULL_CHAR                       ('\0')              /* Null character terminator */
#define MODULE_CLOSE                    (0U)                /* Module close state indicator */
#define CONSOLE_INDEX                   (0)                 /* Console index for virtual debug console */
#define BUFFER_SIZE_DOWN                (16U)               /* Buffer size for console read operations */

/* EP information macro */
#define EP_VERSION                      ("1.0")             /* Example project version */
/* Banner information string macro */
#define MODULE_NAME     "r_tmr"
#define BANNER_INFO     "\r\n*********************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                    *"\
                        "\r\n*   Example Project Version %s                                     *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                               *"\
                        "\r\n*********************************************************************"\
                        "\r\nRefer to .md file for more details on Example Project and" \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

/* Terminal interface function macros - configuration dependent */
#if (USE_VIRTUAL_COM == 1)
    /* Serial terminal (Tera Term) configuration */
    #define TERM_BUFFER_SIZE              (SERIAL_RX_MAX_SIZE)              /* Terminal buffer size */
    #define TERM_INIT()                   (serial_init())                   /* Initialize serial terminal */
    #define TERM_PRINTF(fmt, ...)         (serial_printf((fmt), ##__VA_ARGS__))/* Print formatted string to terminal */
    #define TERM_READ(buf)                (serial_read((buf), sizeof(buf))) /* Read data from terminal */
    #define TERM_HAS_DATA()               (serial_has_data())               /* Check if terminal has data available */
    #define TERM_HAS_KEY()                (serial_has_key())                /* Check if terminal has key press */
    #define TERM_DEINIT()                 (serial_deinit())                 /* De-initialize serial terminal */
#else
    /* Virtual debug console configuration */
    #define TERM_BUFFER_SIZE              (BUFFER_SIZE_DOWN)                /* Console buffer size */
    #define TERM_INIT()                   (0)                               /* Initialize terminal (no-op) */
    #define TERM_PRINTF(fmt, ...)         (printf((fmt), ##__VA_ARGS__))    /* Print formatted string to terminal */
    #define TERM_READ(buf)                (CONSOLE_Read(CONSOLE_INDEX, (buf), sizeof(buf)))/* Read data from console */
    #define TERM_HAS_DATA()               (CONSOLE_HasData())               /* Check if console has data available */
    #define TERM_HAS_KEY()                (CONSOLE_HasKey())                /* Check if console has key press */
    #define TERM_DEINIT()                 /* No de-initialization needed */ /* De-initialize terminal (no-op) */
#endif /* USE_VIRTUAL_COM == 1 */

/***********************************************************************************************************************
 * Application function macros - Compiler and configuration specific
 **********************************************************************************************************************/

/* Renesas CC-RX Toolchain with Virtual COM (Tera Term) */
#if defined(__CCRX__) && (USE_VIRTUAL_COM == 1)

/* Print formatted message to serial terminal */
#define APP_PRINT(...)                  \
    do                                  \
    {                                   \
        serial_printf(__VA_ARGS__);     \
    } while (0)

#define APP_ERR_HANDLE(err, fn_) \
                                    do {                                                \
                                        if (err) {                                      \
                                            handle_error((err), (uint8_t *)(fn_));      \
                                        }                                               \
                                    } while (0)

/* Print error message with function name and details to serial terminal */
#define APP_ERR_PRINT(...)                                         \
    do                                                             \
    {                                                              \
        if (LVL_ERR)                                               \
        {                                                          \
            serial_printf("\r\n[ERR] In Function: %s(), %s",       \
                          __func__, __VA_ARGS__);                  \
        }                                                          \
    } while (0)

/* Print error message and return error code if error occurs */
#define APP_ERR_RETURN(err, ...)                                 \
    do                                                           \
    {                                                            \
        if (err)                                                 \
        {                                                        \
            if (LVL_ERR)                                         \
            {                                                    \
                serial_printf("\r\n[ERR] In Function: %s(), %s", \
                              __func__, __VA_ARGS__);            \
            }                                                    \
            return (err);                                        \
        }                                                        \
    } while (0)

#define ERROR_TRAP   do { __BKPT(0); } while(0)
/* Trap on error by printing error code, de-initializing serial, and triggering breakpoint */
#define APP_ERR_TRAP(err)                                              \
    do                                                                 \
    {                                                                  \
        if ((err))                                                     \
        {                                                              \
            serial_printf("\r\nReturned Error Code: 0x%x\r\n", (err)); \
            serial_deinit();                                           \
            ERROR_TRAP;                                                 \
        }                                                              \
    } while (0)

/* Read data from serial terminal into buffer */
#define APP_READ(buf)                   (serial_read((buf), sizeof(buf)))

/* Check if serial terminal has data available to read */
#define APP_CHECK_DATA                  (serial_has_data())

/* Check if serial terminal has key press available */
#define APP_CHECK_KEY                   (serial_has_key())

/*
 * Renesas CC-RX Toolchain with Renesas Debug Virtual Console
 */
#elif defined(__CCRX__) && (USE_VIRTUAL_COM == 0)

/* Print formatted message to virtual debug console */
#define APP_PRINT(...)                  \
    do                                  \
    {                                   \
        CONSOLE_Print(__VA_ARGS__);     \
    } while (0)

#define APP_ERR_HANDLE(err, fn_) \
                                    do {                                                \
                                        if (err) {                                      \
                                            handle_error((err), (uint8_t *)(fn_));      \
                                        }                                               \
                                    } while (0)

/* Print error message with function name and details to virtual debug console */
#define APP_ERR_PRINT(...)                                   \
    do                                                       \
    {                                                        \
        if (LVL_ERR)                                         \
        {                                                    \
            CONSOLE_Print("[ERR] In Function: %s(), %s",     \
                          __func__, __VA_ARGS__);            \
        }                                                    \
    } while (0)

/* Print error message and return error code if error occurs */
#define APP_ERR_RETURN(err, ...)        \
    do                                  \
    {                                   \
        if (err)                        \
        {                               \
            APP_ERR_PRINT(__VA_ARGS__); \
            return (err);               \
        }                               \
    } while (0)

#define ERROR_TRAP   do { __BKPT(0); } while(0)

/* Trap on error by printing error code and triggering breakpoint */
#define APP_ERR_TRAP(err)                                              \
    do                                                                 \
    {                                                                  \
        if ((err))                                                     \
        {                                                              \
            CONSOLE_Print("\r\nReturned Error Code: 0x%x\r\n", (err));  \
            ERROR_TRAP;                                                \
        }                                                              \
    } while (0)

/* Read data from virtual debug console into buffer */
#define APP_READ(buf)                   (CONSOLE_Read(CONSOLE_INDEX, (buf), sizeof(buf)))

/* Check if virtual debug console has data available to read */
#define APP_CHECK_DATA                  (CONSOLE_HasData())

/* Check if virtual debug console has key press available */
#define APP_CHECK_KEY                   (CONSOLE_HasKey())

/*
 * Renesas GCC-RX Toolchain (both Virtual COM and Debug Console)
 */
#elif !defined(__CCRX__)

/* Print formatted message to terminal */
#define APP_PRINT(fn_, ...)             (TERM_PRINTF((fn_), ##__VA_ARGS__))

#define APP_ERR_HANDLE(err, fn_) \
                                    do {                                                \
                                        if (err) {                                      \
                                            handle_error((err), (uint8_t *)(fn_));      \
                                        }                                               \
                                    } while (0)

/* Print error message with function name and details to terminal */
#define APP_ERR_PRINT(fn_, ...)         ({                                                    \
                                          if (LVL_ERR)                                        \
                                          {                                                   \
                                            TERM_PRINTF("\r\n[ERR] In Function: %s(), %s",    \
                                                        __FUNCTION__, (fn_), ##__VA_ARGS__);  \
                                          }                                                   \
                                        })

/* Print error message and return error code if error occurs */
#define APP_ERR_RETURN(err, fn_, ...)   ({                                     \
                                          if (err)                             \
                                          {                                    \
                                            APP_ERR_PRINT(fn_, ##__VA_ARGS__); \
                                            return (err);                      \
                                          }                                    \
                                        })

#define ERROR_TRAP          ({ __BKPT(0); })

/* Trap on error by printing error code, de-initializing terminal, and triggering breakpoint */
#define APP_ERR_TRAP(err)   ({                                                             \
                              if ((err))                                                   \
                              {                                                            \
                                TERM_PRINTF("\r\nReturned Error Code: 0x%x  \r\n", (err)); \
                                TERM_DEINIT();                                             \
                                ERROR_TRAP;                                                \
                              }                                                            \
                            })

/* Read data from terminal into buffer */
#define APP_READ(buf)                   (TERM_READ(buf))

/* Check if terminal has data available to read */
#define APP_CHECK_DATA                  (TERM_HAS_DATA())

/* Check if terminal has key press available */
#define APP_CHECK_KEY                   (TERM_HAS_KEY())

#endif /* defined(__CCRX__) */

#endif /* COMMON_UTILS_H_ */
