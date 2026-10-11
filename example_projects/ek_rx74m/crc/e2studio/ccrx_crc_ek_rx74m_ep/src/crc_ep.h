/***********************************************************************************************************************
 * File Name    : crc_ep.h
 * Description  : This file contains macros and function declarations used in hal_entry.c
 **********************************************************************************************************************/


/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CRC_EP_H_
#define CRC_EP_H_

/* Related headers */
#include "hal_data.h"

/* For onboard LED */
#define LED_ON                  (BSP_IO_LEVEL_HIGH)
#define LED_OFF                 (BSP_IO_LEVEL_LOW)

#define GREEN                   (IOPORT_PORT_08_PIN_07)
#define LED_COUNT               (3U)

#define TURN_GREEN_ON             (R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) GREEN, BSP_IO_LEVEL_HIGH))
#define TURN_GREEN_OFF            (R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) GREEN, BSP_IO_LEVEL_LOW))

/* Length of input buffer to calculate CRC in normal mode */
#define NUM_BYTES               (4U)

/* Size of input buffer */
#define BUF_LEN                 (8U)

/* LED toggle delay */
#define TOGGLE_DELAY            (0x15E)

/* 8-bit and 16-bit seed value and data length */
#define SEED_VALUE              (0x00000000)
#define EIGHT_BIT_DATA_LEN      (5U)
#define SIXTEEN_BIT_DATA_LEN    (6U)

#define EP_INFO "\r\nThe example project demonstrates the typical use of the CRC HAL module APIs.\r\n"\
                "It demonstrates CRC operation for data transmission in normal mode and reception in snoop\r\n"\
                "mode through SCI interface. If the board does not support snoop mode, reception is performed in\r\n"\
                "normal mode through SCI interface. If the board does not support snoop mode, reception\r\n"\
                "is performed in normal mode through SAU interface. On pressing any key through Terminal Application,"\
                "\r\nCRC value in normal mode is calculated for 4 bytes of data. The calculated CRC value and input"\
                "\r\ndata are transmitted and received through loopback on SCI_UART. \r\n"\
                "\r\nOnce the transfer is complete, if CRC value for snoop mode is zero and the transmit\r\n"\
                "and receive buffers are equal, the on-board LED blinks as a sign of successful CRC operation.\r\n"\
              "On data mismatch, LED stays ON. Failure and status messages are displayed on the Terminal Application.\r\n"\

/* Check IO-port API return and trap error (if any error occurs), clean up and display failure details
 * on Terminal Application */
#define VALIDATE_IO_PORT_API(API)   ({\
                                    if (FSP_SUCCESS != (API))\
                                    {   APP_PRINT("%s API failed at line number %d", \
                                                    #API, __LINE__);\
                                        cleanup();\
                                        APP_ERR_TRAP(true);\
                                    }\
                                    })

/* Function declarations */
void toggle_led(void);
void cleanup(void);
void deinit_crc(void);
void deinit_uart(void);

#endif /* CRC_EP_H_ */
