/***********************************************************************************************************************
 * File Name    : elc_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ELC_EP_H_
#define ELC_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define EP_INFO                 "\r\nThis example project demonstrates the basic usage of ELC driver."\
		                        "\r\nELC Software Event, GPT8 and GPT1 events are linked using ELC."\
                                "\r\nThe start source for GPT8 and GPT1 is ELC Software Event, and the"\
                                "\r\nstop source for GPT8 is GPT1 counter overflow."\
                                "\r\nGPT8 runs in PWM mode, and GPT1 runs in One-Shot mode.\r\n"\
                                "On giving valid Terminal Application input, an ELC Software Event is generated that"\
                                "\r\ntriggers LED blinking. LED stops blinking after 5 seconds when GPT1 expires.\r\n"

#define OPEN_TIMER_FAIL         "\r\n** GPT module Open for One-Shot mode failed **\r\n"
#define OPEN_TIMER_PWM_FAIL     "\r\n** GPT module Open for PWM mode failed **\r\n"
#define ENABLE_TIMER_PULSE_FAIL "\r\n** GPT Enable for PWM timer failed **\r\n"
#define STATUS_DISPLAY        "\r\nEnter any key on the Terminal Application input. LED blinks for 5 seconds and stops:"
#define ENABLE_TIMER_FAIL       "\r\n** GPT Enable for One-Shot mode failed **\r\n"

/* Macro for NULL character */
#define NULL_CHAR               ('\0')

/* Function declaration */
void elc_ep_entry(void);

#endif /* ELC_EP_H_ */
