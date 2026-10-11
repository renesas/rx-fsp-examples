/***********************************************************************************************************************
 * File Name    : exdmac_ep.h
 * Description  : Contains data structures and functions used in exdmac_ep.h.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef EXDMAC_EP_H_
#define EXDMAC_EP_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"

/*******************************************************************************************************************//**
 * Macro definitions
 **********************************************************************************************************************/
#define ZERO                        (0U)
#define ONE                         (1U)
#define TWO                         (2U)

/* Macros for used mode transfer */
#define NORMAL                      (1U)          /* Normal transfer mode */
#define REPEAT                      (2U)          /* Repeat transfer mode */
#define BLOCK                       (3U)          /* Block transfer mode */
#define REPEAT_BLOCK                (4U)          /* Repeat-Block transfer mode */

#define MAX_LENGTH                  (60U)
#define DATA_LENGTH                 (512U)

#define EXDMAC_WAIT_LOOP_DELAY      (1U)
#define CLEAR_FLAG                  (0U)
#define TIMEOUT                     (0xFFFFFFFFF) /* Maximum timeout value */
#define INITIAL_VALUE               (0)           /* Default initialization value for variables */

#define LED_ON_DURATION_MS          (2000U)
#define SRC_REPEAT_LENGTH           (10)
#define REPEAT_COUNT                (4)
#define BLOCK_BYTES                 (SRC_REPEAT_LENGTH * sizeof(uint32_t))
#define DST_REPEAT_LENGTH           (SRC_REPEAT_LENGTH * REPEAT_COUNT)

#define EP_INFO                     "\r\nThis example project demonstrates the standard usage of the Extended Direct Memory Access (EXDMAC)"\
                                    "\r\nHAL module APIs on an RX MCU. It employs the General PWM Timer (GPT) and Software Trigger as "\
                                    "\r\nactivation sources for EXDMAC transfer to control LEDs. The LEDs reflect the behavior of different "\
                                    "\r\nEXDMAC transfer modes, including Normal Mode, Repeat Mode, Block Mode, and Repeat-Block Mode."

#define MENU                        "\r\n===== EXDMAC Transfer Menu ====="\
                                    "\r\n1: Normal mode transfer"\
                                    "\r\n2: Repeat mode transfer"\
                                    "\r\n3: Block mode transfer"\
                                    "\r\n4: Repeat-Block mode transfer"\
                                    "\r\nPlease enter your choice (1-4): "

void exdmac_hal_entry(void);

#endif /* EXDMAC_EP_H_ */

