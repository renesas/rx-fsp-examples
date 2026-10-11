/***********************************************************************************************************************
 * File Name    : icu_ep.h
 * Description  : Contains Macros and function declarations.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef ICU_EP_H_
#define ICU_EP_H_

/* External IRQ channel for specific boards*/
#define USER_SW_IRQ_NUMBER        (0x1E)      /* EK_RX74m board */

#define GREEN                     (IOPORT_PORT_08_PIN_07)

#define EP_INFO    "\r\nThis Example Project demonstrates the functionality of ICU driver.\r\n" \
    "On pressing the user push button, an external IRQ is triggered, which toggles on-board LED.\r\n"

/* Function declaration */
fsp_err_t icu_init(void);
fsp_err_t icu_enable(void);
void icu_deinit(void);

#endif /* ICU_EP_H_ */
