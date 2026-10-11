/*******************************************************************************************************************//**
 * File Name    : cmtw_ep.h
 * Description  : This file contains data structures, macros, and function declarations used in cmtw_ep.c
 *                It implements the CMTW functionality for the example project. It handles CMTW in One-Shot mode
 *                and Periodic mode.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CMTW_EP_H_
#define CMTW_EP_H_

/*******************************************************************************************************************//**
 * @defgroup CMTW_EP
 * @{
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * Macro definition Section
 **********************************************************************************************************************/
/* EP information banner displayed at startup */
#define CMTW_INFO      "\r\nThis example demonstrates the typical usage of the CMTW HAL module APIs. "                    \
                       "\r\nUsers can select one of four demo modes via the terminal menu: "                          \
                       "\r\nDemo 1: One-Shot mode - Compare Match: LED1 turns on for a user-defined period\r\n"       \
                       "         (or frequency), then turns off automatically. "                                      \
                       "\r\nDemo 2: Periodic mode - Compare Match: LED2 blinks continuously at a user-defined\r\n"    \
                       "         period or frequency. "                                                               \
                       "\r\nDemo 3: Periodic mode - Input Capture: GPT1 generates a waveform, and CMTW0\r\n"          \
                       "         captures the signal to measure the actual pulse width and toggles LED3. "            \
                       "\r\nDemo 4: Periodic mode - Output Compare: CMTW0 generates a waveform, and GPT1\r\n"         \
                       "         captures the signal to measure the actual pulse width and toggles LED3. \r\n"        

/* Main menu displayed to user for selecting demo mode */
#define EP_MENU        "\r\n******************************************************************"                      \
                       "\r\n              Main Menu (Mode Selection) "                                               \
                       "\r\n******************************************************************"                      \
                       "\r\n    [1] One-Shot - Compare Match"                                                        \
                       "\r\n    [2] Periodic - Compare Match"                                                        \
                       "\r\n    [3] Periodic - Input Capture"                                                        \
                       "\r\n    [4] Periodic - Output Compare"                                                       \
                       "\r\n------------------------------------------------"                                        \
                       "\r\nPlease enter your selection [1, 2, 3, 4]: \r\n> "

/* Submenu for selecting timer input type (millisecond or frequency) */
#define INPUT_OPTION   "\r\n******************************************************************"                     \
                       "\r\n           Main Menu > Sub Menu (Input Type Selection)"                                 \
                       "\r\n******************************************************************"                     \
                       "\r\n    [1] Input type is: millisecond "                                                    \
                       "\r\n    [2] Input type is: Hz "                                                             \
                       "\r\n    [3] Go back to Main Menu "                                                          \
                       "\r\n------------------------------------------------"                                       \
                       "\r\nPlease enter your selection [1, 2, 3]: \r\n> "

/* Menu option definitions */
#define ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION                    (1U)
#define PERIODIC_MODE_COMPARE_MATCH_FUNCTION                    (2U)
#define PERIODIC_MODE_INPUT_CAPTURE_FUNCTION                    (3U)
#define PERIODIC_MODE_OUTPUT_COMPARE_FUNCTION                   (4U)

/* Common macros */
#define BUF_SIZE                           (15U)                /* Size of character buffer for data conversion */
#define MODULE_CLOSE                       (0U)                 /* Timer module closed state indicator */
#define LED_COUNT_ZERO                     (0U)                 /* Value indicating no LEDs available on board */
#define LED_ONE                            (1U)                 /* Index for first LED (used in One-Shot demo) */
#define LED_TWO                            (2U)                 /* Index for second LED (used in Periodic demo) */
#define LED_THREE                          (3U)                 /* Index for third LED (used in Capture demos) */
#define LED_ON                             (BSP_IO_LEVEL_HIGH)  /* GPIO level to illuminate LED */
#define LED_OFF                            (BSP_IO_LEVEL_LOW)   /* GPIO level to turn off LED */
#define NULL_CHAR                          ('\0')               /* String terminator character */
#define DISABLE                            (0)                  /* Flag state: disabled/inactive */
#define ENABLE                             (1)                  /* Flag state: enabled/active */
#define MAX_RAW_COUNT_16BIT                (65535U)             /* Maximum timer count value for 16-bit mode */
#define MAX_RAW_COUNT_32BIT                (4294967295U)        /* Maximum timer count value for 32-bit mode */
#define MIN_RAW_COUNT                      (1U)                 /* Minimum valid timer count value */
#define CONVERT_SECOND_TO_MILLISECOND      (1000U)              /* Conversion factor: seconds to milliseconds */
#define CONVERT_MILLISECOND_TO_SECOND      (1000U)              /* Conversion factor: milliseconds to seconds */
#define INITIAL_VALUE                      (0)                  /* Default initialization value for variables */
#define CAPTURE_EVENT_COUNT_THRESHOLD      (7U)                 /* Number of capture events before measurement ends */
#define SKIP_FIRST_READING_THRESHOLD       (2U)                 /* Number of initial readings to skip (unstable) */

/* Timeout macros */
#define TIMEOUT                            (0xFFFFFFFFF)        /* Maximum timeout value */

/* Recommended value macros for Compare Match demos (Demo 1 & 2) */
#define PERIOD_REC_MIN_COMPARE_MS          (1U)                 /* Min recommended period for Compare Match (ms) */
#define PERIOD_REC_MAX_MS                  (10000U)             /* Max recommended period for all demos (ms) */
#define FREQ_REC_MIN_HZ                    (1U)                 /* Min recommended frequency for all demos (Hz) */
#define FREQ_REC_MAX_HZ                    (50U)                /* Max recommended frequency for all demos (Hz) */

/* Recommended value macros for Capture demos (Demo 3 & 4) */
#define PERIOD_REC_MIN_CAPTURE_MS          (50U)                /* Min recommended period for Capture demos (ms) */

/*******************************************************************************************************************//**
 * Enum definitions
 **********************************************************************************************************************/
/* Timer type: CMTW or GPT */
typedef enum
{
    TIMER_TYPE_CMTW = 0,                    /* CMTW timer type */
    TIMER_TYPE_GPT                          /* GPT timer type */
} e_timer_type;

/* Timer mode: One-Shot or Periodic */
typedef enum
{
    ONE_SHOT_MODE = 0,                      /* One-Shot mode */
    PERIODIC_MODE                           /* Periodic mode */
} e_timer_mode;

/* Timer function: Compare Match, Output Compare, or Input Capture */
typedef enum
{
    COMPARE_MATCH_FUNCTION = 0,             /* Timer triggers when counter matches compare value */
    OUTPUT_COMPARE_FUNCTION,                /* Timer generates output waveform at compare match */
    INPUT_CAPTURE_FUNCTION                  /* Timer captures counter value on external signal edge */
} e_timer_function;

/* Timer input type: Period (ms) or Frequency (Hz) */
typedef enum
{
    TIMER_INPUT_TYPE_MILLISECOND = 1,       /* User inputs timer period in milliseconds */
    TIMER_INPUT_TYPE_FREQUENCY,             /* User inputs timer frequency in Hertz */
    TIMER_INPUT_TYPE_GO_BACK                /* User wants to go back to main menu */
} e_timer_input_type;

/* Timer size: 16-bit or 32-bit */
typedef enum
{
    TIMER_16_BIT = 0,                       /* Timer operates with 16-bit counter register */
    TIMER_32_BIT                            /* Timer operates with 32-bit counter register */
} e_timer_size;

/*******************************************************************************************************************//**
 * Structure definitions
 **********************************************************************************************************************/
/* Buffer struct to safely pass capture data between ISR and main loop */
typedef struct
{
    uint64_t capture_count;                 /* Timer counter value captured on signal edge */
    uint32_t overflow_count;                /* Overflow counter for GPT timer */
    uint32_t event_count;                   /* Total number of capture events detected */
    bool     buffer_ready;                  /* Flag indicating new capture data is available */
} capture_buffer_t;

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
/* Public function declarations */
void cmtw_ep_entry(void);

 /*******************************************************************************************************************//**
 * @} (end defgroup CMTW_EP)
 **********************************************************************************************************************/

#endif /* CMTW_EP_H_ */
