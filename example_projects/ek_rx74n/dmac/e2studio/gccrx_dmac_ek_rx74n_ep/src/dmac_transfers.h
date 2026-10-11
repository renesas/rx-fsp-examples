/***********************************************************************************************************************
 * File Name    : dmac_transfers.h
 * Description  : Contains data structures and functions used in dmac_transfers.h
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef DMAC_TRANSFERS_H_
#define DMAC_TRANSFERS_H_

#define NEW_LINE    ('\r')    /* New line character */
#define NULL_CHAR   ('\0')    /* End of string character */

#define EP_INFO "\r\nProject initializes 3 DMAC transfer instances. Upon successful initialization, a menu option is" \
                " displayed.\r\nOn selecting 1 from menu option, CMT generates 100ms interrupt to trigger a transfer" \
                " from the source buffer\r\nto port control register for 60 times indicated by LED blinking for 60 "  \
                "times.\r\nOn selecting 2 from menu option, 32-bit data is transferred from the GPT counter"          \
                " register\r\nto destination array which gets printed on the Terminal Application."                      \
                "\r\nOn selecting 3 from menu option, 32 bit data is transferred from source array\
                  \r\nto port control register and state of LED changes accordingly.\r\n"

#define MENU    "\r\nPress 1 to transfer source array to LED Port in normal mode. \
                \r\nPress 2 to transfer GPT current timer value.                  \
                \r\nPress 3 to transfer source array to LED Port in repeat mode.  \r\n"

/* Perform transfer led blink operation */
void transfer_led_blink_operation(void);

/* Perform transfer gpt timer operation  */
void transfer_gpt_timer_operation(void);

/* Perform transfer write ioport operation */
void transfer_write_ioport_operation(void);

#endif
