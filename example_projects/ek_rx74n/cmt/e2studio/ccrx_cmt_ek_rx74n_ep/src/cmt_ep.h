/***********************************************************************************************************************
 * File Name    : cmt_ep.h
 * Description  : This file contains data structures, macros and functions used in cmt_ep.c.
 *                It implements the CMT functionality for the example project. It handles CMT in One Shot modes 
 *                and Periodic mode
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CMT_EP_H_
#define CMT_EP_H_

/*******************************************************************************************************************//**
 * @ingroup cmt_ep
 * @{
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * Include Section
 **********************************************************************************************************************/
 #include "common_utils.h"
 
/*******************************************************************************************************************//**
 * Macro definition Section
 **********************************************************************************************************************/
#define CMT_INFO        "\r\nThis example demonstrates typical usage of the CMT HAL module APIs. "            \
                        "\r\nUsers can select one of two demo modes via the terminal menu: "                  \
                        "\r\nDemo 1: One-Shot mode - Compare Match: LED1 turns on for a user-defined period " \
                        "or frequency, then turns off automatically. "                                        \
                        "\r\nDemo 2: Periodic mode - Compare Match: LED2 blinks based on the user-defined "   \
                        "period or frequency. \r\n"

#define EP_MENU         "\r\n******* EP Menu Options *******" \
                        "\r\n1. One Shot - Compare Match"     \
                        "\r\n2. Periodic - Compare Match"     \
                        "\r\nPlease enter your option:  "

#define INPUT_OPTION    "\r\n******* Input Options *******" \
                        "\r\n1. Input is: millisecond "     \
                        "\r\n2. Input is: Hz "              \
                        "\r\nPlease enter your option:  "

/* Buffer size */
#define BUF_SIZE                                                     (30U)
/* Status of timer  */
#define MODULE_CLOSE                                                 (0U) 
/* MACRO to check if the LED count is zero */
#define LED_COUNT_ZERO                                               (0U)
/* LED 0 */
#define LED_ZERO                                                     (0U)
/* LED 1 */
#define LED_ONE                                                      (1U)
/* LED 2 */
#define LED_TWO                                                      (2U)
/* Turn on LED */
#define LED_ON                                                       (BSP_IO_LEVEL_HIGH)
/* Turn off LED */
#define LED_OFF                                                      (BSP_IO_LEVEL_LOW)

/* Macro used for demo case */
#define ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION                         (1U)
#define PERIODIC_MODE_COMPARE_MATCH_FUNCTION                         (2U)

/* MACRO for null character */
#define NULL_CHAR                                                    ('\0')

/* Macro use for flag */
#define DISABLE                                                      (0)
#define ENABLE                                                       (1)

/* Macro for the raw count  */
#define MAX_RAW_COUNT_16BIT                                          (65535U)
#define MAX_RAW_COUNT_32BIT                                          (4294967295U)
#define MIN_RAW_COUNT                                                (1U)

/* Macro for the conversion */
#define CONVERT_SECOND_TO_MILLISECOND                                (1000U)
#define CONVERT_MILLISECOND_TO_SECOND                                (1000U)

/*******************************************************************************************************************//**
 * Enum definition Section
***********************************************************************************************************************/

/* Enum of Timer mode */
typedef enum
{
    ONE_SHOT_MODE = 0,
    PERIODIC_MODE
} e_timer_mode;

/* Enum of Timer function */
typedef enum
{
    COMPARE_MATCH_FUNCTION,
    OUTPUT_COMPARE_FUNCTION,
    INPUT_CAPTURE_FUNCTION
} e_timer_function;

/* Enum of Timer input type */
typedef enum
{
    PERIOD_INPUT_TYPE = 1,     /* Input is: millisecond */
    FREQUENCY_INPUT_TYPE       /* Input is: Hz */
} e_timer_input_type;

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
/* Public function declarations */
/* Main function of EP */
void cmt_ep_entry(void);

/*******************************************************************************************************************//**
 * @} (end defgroup cmt_ep)
 **********************************************************************************************************************/

#endif /* CMT_EP_H_ */
