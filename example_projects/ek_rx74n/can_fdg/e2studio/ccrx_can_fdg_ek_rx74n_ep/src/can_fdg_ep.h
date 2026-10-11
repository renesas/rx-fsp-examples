/***********************************************************************************************************************
 * File Name    : can_fdg_ep.h
 * Description  : This file contains the function declarations and definitions for the CANFDG example project.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAN_FDG_EP_H_
#define CAN_FDG_EP_H_

#define CAN_MAILBOX_NUMBER_0            (0U)               /* Mail box number */
#define CAN_CLASSIC_FRAME_DATA_BYTES    (8U)               /* Data Length code for classic frame */
#define CAN_FDG_DATA_LENGTH_CODE         (64)               /* Data Length code for FD frame */
#define CAN_ID                          (0x1100)           /* ID to be updated in transmit frame */

/* Acceptance filter array parameters */
#define CANFDG_FILTER_ID                 (0x00001000)
#define MASK_ID                         (0x1FFFF000)
#define MASK_ID_MODE                    (1)

#define ZERO                            (0U)               /* Array Index value */
#define NULL_CHAR                       ('\0')
#define WAIT_TIME                       (1500U)            /* Wait time value */

#define EP_INFO        "\r\n This Example Project demonstrates CAN FDG operations on Renesas RX MCUs using 2 RX boards."\
                       "\r\nOn pressing any key on the Terminal Application, data is transmitted from one"\
                       "\r\nboard to other. On data reception, Board2 displays the received data on the "\
                       "\r\nTerminal Application and transmits updated data to Board1."\
                       "\r\nBoard1 on reception of data, changes CAN frame to CANFDG frame and sends data to board2."\
                       "\r\nOn reception, board2 transmits updated data."\
                       "\r\nOn successful transmission, Board1 prints the data on to the Terminal Application.\r\n\n"

/* Enum to select LD that is to be made high */
typedef enum
{
   red = 1,
   green = 2,
   blue = 3,
} led_state_t;

#define LED_RED         (IOPORT_PORT_06_PIN_04)
#define LED_GREEN       (IOPORT_PORT_08_PIN_07)
#define LED_BLUE        (IOPORT_PORT_15_PIN_05)

void canfdg_operation(void);
void can_read_operation(void);
void canfdg_deinit(void);

#endif /* CAN_FDG_EP_H_ */
