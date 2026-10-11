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

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"

/* Macro when using VIRTUAL_CONSOLE */
#if (VIRTUAL_CONSOLE == 1)
#include "lowsrc/lowlvl.h"
#include "lowsrc/lowsrc.h"
#else
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
#endif /* VIRTUAL_CONSOLE */

/* SEGGER RTT disabling macro */
#define DISABLE_APP_PRINT       (0)
#define DISABLE_APP_ERR_PRINT   (0)
#define DISABLE_APP_ERR_TRAP    (0)
#define DISABLE_APP_READ        (0)
#define DISABLE_APP_CHECK_DATA  (0)

#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define LVL_ERR      (1u)       /* error conditions   */

#define RESET_VALUE             (0x00)

#define EP_VERSION              ("1.0")
#define MODULE_NAME             "r_sci_b_uart"
#define BANNER_1                "\r\n******************************************************************"
#define BANNER_2                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module          *"
#define BANNER_3                "\r\n*   Example Project Version %s                                  *"
#define BANNER_4                "\r\n*   Flex Software Pack Version %d.%d.%d                             *"
#define BANNER_5                "\r\n******************************************************************"\
                                "\r\nRefer to .md file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

/* Macros when using Renesas Debug Virtual Console */
#if (VIRTUAL_CONSOLE == 1)
#define CONSOLE_INDEX           (0)

/* Size of the buffer for terminal input to target from host (Usually keyboard input) (Default: 16) */
#define BUFFER_SIZE_DOWN        (16U)

/* Used for Renesas CCRX Toolchain */
#if defined(__CCRX__)
#define APP_PRINT(...)              \
    do                              \
    {                               \
        CONSOLE_Print(__VA_ARGS__); \
    } while (0)

#define APP_ERR_PRINT(...)                               \
    do                                                   \
    {                                                    \
        if (LVL_ERR)                                     \
        {                                                \
            CONSOLE_Print("[ERR] In Function: %s(), %s", \
                          __func__, __VA_ARGS__);    \
        }                                                \
    } while (0)

#define APP_ERR_RETURN(err, ...)        \
    do                                  \
    {                                   \
        if (err)                        \
        {                               \
            APP_ERR_PRINT(__VA_ARGS__); \
            return (err);               \
        }                               \
    } while (0)

#define APP_ERR_TRAP(err)                                              \
    do                                                                 \
    {                                                                  \
        if ((err))                                                     \
        {                                                              \
            CONSOLE_Print("\r\nReturned Error Code: 0x%x\r\n", (err)); \
            __BKPT(0);                                                 \
        }                                                              \
    } while (0)

/* Used for Renesas GCC Toolchain */
#else
#define APP_PRINT(fn_, ...) (printf((fn_), ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...)                         \
    ({                                                  \
        if (LVL_ERR)                                    \
        {                                               \
            printf("[ERR] In Function: %s(), %s",       \
                   __FUNCTION__, (fn_), ##__VA_ARGS__); \
        }                                               \
    })

#define APP_ERR_RETURN(err, fn_, ...)          \
    ({                                         \
        if (err)                               \
        {                                      \
            APP_ERR_PRINT(fn_, ##__VA_ARGS__); \
            return (err);                      \
        }                                      \
    })

#define APP_ERR_TRAP(err)                                         \
    ({                                                            \
        if ((err))                                                \
        {                                                         \
            printf("\r\nReturned Error Code: 0x%x  \r\n", (err)); \
            __BKPT(0);                                            \
        } /* Trap upon the error */                              \
    })

#endif /* defined(__CCRX__) */

#define APP_READ(read_data) (CONSOLE_Read(CONSOLE_INDEX, read_data, sizeof(read_data)))

#define APP_CHECK_DATA      (CONSOLE_HasKey())

/* Macro when using RTT Viewer */
#else
#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)      SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);

#define APP_ERR_PRINT(fn_, ...)  if(LVL_ERR)\
        SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",__FUNCTION__,(fn_),##__VA_ARGS__);

#define APP_ERR_TRAP(err)        if(err) {\
        SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", err);\
        __BKPT(0);} /* trap upon the error  */

#define APP_READ(read_data)     SEGGER_RTT_Read (SEGGER_INDEX, read_data, sizeof(read_data));

#define APP_CHECK_DATA          SEGGER_RTT_HasKey()

/* Conditional disabling logic */
#if defined(APP_PRINT) && (DISABLE_APP_PRINT == 1)
#undef APP_PRINT
#define APP_PRINT(fn_, ...)
#endif

#if defined(APP_ERR_PRINT) && (DISABLE_APP_ERR_PRINT == 1)
#undef APP_ERR_PRINT
#define APP_ERR_PRINT(fn_, ...)
#endif

#if defined(APP_ERR_TRAP) && (DISABLE_APP_ERR_TRAP == 1)
#undef APP_ERR_TRAP
#define APP_ERR_TRAP(err)
#endif

#if defined(APP_READ) && (DISABLE_APP_READ == 1)
#undef APP_READ
#define APP_READ(read_data)
#endif

#if defined(APP_CHECK_DATA) && (DISABLE_APP_CHECK_DATA == 1)
#undef APP_CHECK_DATA
#define APP_CHECK_DATA (0)
#endif
#endif
#endif /* COMMON_UTILS_H_ */
