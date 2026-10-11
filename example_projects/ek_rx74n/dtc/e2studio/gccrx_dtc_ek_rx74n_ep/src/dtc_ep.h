/*******************************************************************************************************************//**
 * File Name    :  dtc_ep.h
 * Description  :  This file contains the function declarations and definitions for the DTC example project.
 *
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DTC_EP_H_
#define DTC_EP_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "common_utils.h"

/*******************************************************************************************************************//**
 * Macro definitions
 **********************************************************************************************************************/
#define ZERO                (0U)
#define ONE                 (1U)
/* Macros for used mode transfer */
#define NORMAL              (1U)   /* Normal transfer mode */
#define REPEAT              (2U)   /* Repeat transfer mode */
#define BLOCK               (3U)   /* Block transfer mode */
#define CHAIN               (4U)   /* Chain transfer mode */

#define BUF_SIZE            (16U)
#define DATA_LENGTH         (512U)
#define MAX_LENGTH          (60U)

#define TRANSFER_COUNT      (6U)
#define ONE_SECONDS         (0x7500000)

#define DTC_WAIT_LOOP_DELAY (1U)
#define CLEAR_FLAG          (0U)
#define NULL_CHAR           ('\0')

#define EP_INFO     "\r\nThis example project demonstrates the standard usage of the Data Transfer Controller (DTC)"\
                    "\r\nHAL module APIs on an RX MCU. It employs the General PWM Timer (GPT) as a trigger source "\
                    "\r\nfor DTC transfers to control LEDs efficiently. The LEDs reflect the behavior of different"\
                    "\r\nDTC transfer modes, including Normal Mode, Repeat Mode, Block Mode, and Chain Mode."
#define MENU        "\r\n===== DTC Transfer Menu ====="\
                    "\r\n1: Normal mode transfer"\
                    "\r\n2: Repeat mode transfer"\
                    "\r\n3: Block mode transfer"\
                    "\r\n4: Chain mode transfer"\
                    "\r\nPlease enter your choice (1-4): "

void dtc_hal_entry(void);

#endif /* DTC_EP_H_ */
