/***********************************************************************************************************************
* File Name    : serial.h
* Description  : Contains macros, data structures, and common functions used for serial.c
***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SERIAL_H_
#define SERIAL_H_

#if (USE_VIRTUAL_COM == 1)
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include <vector_data.h>
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Board-specific macros for UART hardware configuration */
#define SERIAL_CHANNEL                    (12U)
#define SERIAL_RX_PIN                     (BSP_IO_PORT_01_PIN_15)
#define SERIAL_TX_PIN                     (BSP_IO_PORT_01_PIN_14)
#define SERIAL_PIN_CFG                    ((uint32_t) IOPORT_CFG_DRIVE_1 | \
                                          (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | \
                                          (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8_10_12_0A)
#define SERIAL_PIN_RX_CFG                 (SERIAL_PIN_CFG)
#define SERIAL_PIN_TX_CFG                 (SERIAL_PIN_CFG)

/* Macros for the UART baud rate configuration */
#define SERIAL_BAUD_RATE                    (115200U)
#define SERIAL_ERR_X1000                    (4000U)
#define SERIAL_MODULATION                   (0U)

/* Macros for time-out operation */
#define SAFETY_FACTOR                       (2U)
#define SERIAL_BIT_FRAME                    (10U)
#define SECOND_TO_MICROSECOND               (1000000U)
#define SERIAL_TIME_US                      (SECOND_TO_MICROSECOND / (SERIAL_BAUD_RATE / SERIAL_BIT_FRAME))

/* Macros for the UART software configuration */
#define SERIAL_TX_MAX_SIZE                  (2048U)
#define SERIAL_RX_MAX_SIZE                  (64U)

/* Macros for special character definitions */
#define SERIAL_CHAR_BS                      (0x08)
#define SERIAL_CHAR_LF                      (0x0A)
#define SERIAL_CHAR_CR                      (0x0D)

/* Macros for special values */
#define SERIAL_TRUE                         (1U)
#define SERIAL_FALSE                        (0U)
#define SERIAL_DATA_ZERO                    (0x00)
#define SERIAL_DATA_ONE                     (0x01)

/* Macros for setting interrupt */
#if SERIAL_CHANNEL == 12
  #if defined(VECTOR_NUMBER_RSCI12_RXI)
    #define VECTOR_NUMBER_UARTX_RXI (VECTOR_NUMBER_RSCI12_RXI)
  #endif
#endif

#if SERIAL_CHANNEL == 12
  #if defined(VECTOR_NUMBER_RSCI12_TXI)
    #define VECTOR_NUMBER_UARTX_TXI (VECTOR_NUMBER_RSCI12_TXI)
  #endif
#endif

#if SERIAL_CHANNEL == 12
  #if defined(VECTOR_NUMBER_RSCI12_TEI)
    #define VECTOR_NUMBER_UARTX_TEI (VECTOR_NUMBER_RSCI12_TEI)
  #endif
#endif

#if SERIAL_CHANNEL == 12
  #if defined(VECTOR_NUMBER_RSCI12_ERI)
    #define VECTOR_NUMBER_UARTX_ERI (VECTOR_NUMBER_RSCI12_ERI)
  #endif
#endif

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
uint32_t serial_init(void);
void serial_deinit(void);
uint32_t serial_printf(char * p_format, ...);
uint32_t serial_read(void * const p_buffer, uint32_t buffer_size);
uint32_t serial_has_data(void);
uint32_t serial_has_key(void);

#endif /* USE_VIRTUAL_COM */
#endif /* SERIAL_H_ */
