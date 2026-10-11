/***********************************************************************************************************************
 * File Name    : tmr_ep.h
 * Description  : This file contains data structures, macros, and functions commonly used in the EP.
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TMR_EP_H_
#define TMR_EP_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "common_utils.h"

/*******************************************************************************************************************//**
 * Macro definitions
 **********************************************************************************************************************/
#define BUF_SIZE                  (16U)          /* Size of buffer for Console input data */
#define INITIAL_VALUE             ('\0')

#define TIMER_UNITS_MILLISECONDS  (1000U)        /* Timer unit in millisecond */
#define TMR_MAX_PERIOD_COUNT      (0xFFFF)       /* Max Period Count for 8-bit Timer */
#define TMR_MAX_PERCENT           (100U)
#define PERIOD_RANGE              "(0 to 7158ms)" /* TMR timer period range */
#define TIMER_PIN                 (0)
#define PWM_MODE                  (1U)
#define ONE_SHOT_MODE             (2U)
#define PERIODIC_MODE             (3U)

#define MENU    "\r\nMenu Options"\
                "\r\n1: PWM mode"\
                "\r\n2: One Shot Mode"\
                "\r\n3: Periodic Mode"\
                "\r\nUser input: "

#define EP_INFO "\r\nThis example project showcases the standard usage of the 8-bit Timer (TMR) HAL module APIs on an"\
                "\r\nRX MCU. Users can interact with the system through a Virtual Console by selecting different"\
                "\r\ntimer-supported modes, including PWM, One-Shot, and Periodic. In PWM mode, the user enters"\
                "\r\na duty cycle (0-100%) to adjust the brightness of the LED. In One-Shot mode generates a single"\
                "\r\nuser-defined pulse and reports the event via the terminal interface. In Periodic mode the timer"\
                "\r\noperates with a 16-bit counter, and the user specifies a period in milliseconds."\
                "\r\nThe LED toggles at the given interval, creating a blinking pattern.\r\n"

void tmr_hal_entry(void);

#endif /* TMR_EP_H_ */
