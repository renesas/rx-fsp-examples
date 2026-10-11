/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.h
 * Description  : Contains macros and function declarations.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SCI_I2C_EP_H_
#define SCI_I2C_EP_H_

/* For on board LED */
#define LED_ON             ((bool)BSP_IO_LEVEL_HIGH)
#define LED_OFF            ((bool)BSP_IO_LEVEL_LOW)

/* MACRO for checking if two buffers are equal */
#define BUFF_EQUAL         (0U)

/* Buffer size for slave and master data */
#define BUF_LEN            (0x06)

/* Led toggle delay */
#define TOGGLE_DELAY       (0x15E)

/* Delay added to recognise LED toggling after write/read operation */
#define DELAY_OPERATION    (1U)

#define EP_INFO  "\r\n  This Example Project demonstrates SCI_I2C Master operation through"\
                  "\r\n loop-back with IIC Slave driver. 6 bytes of data will be transmitted"\
                  "\r\n and received continuously on successful initialization."\
                  "\r\n The transmitted data is compared with the received data. If the data matches, on-board LED"\
                  "\r\n starts blinking. On a data mismatch, LED stays ON."\
                  "\r\n Failure messages and status is displayed on Terminal Application.\r\n\n"

/* Define LED */
#define LED_RED         (IOPORT_PORT_06_PIN_04)
#define LED_GREEN       (IOPORT_PORT_08_PIN_07)
#define LED_BLUE        (IOPORT_PORT_15_PIN_05)

/* Enumerators to identify Master event to be processed */
typedef enum e_master
{
    MASTER_READ  = 1U,
    MASTER_WRITE = 2U
}master_transfer_mode_t;

/*
 *  Global functions
 */
fsp_err_t init_i2c_driver(void);
fsp_err_t process_master_WriteRead(void);
void deinit_i2c_driver(void);
void set_led(bool b_value);

#endif /* SCI_I2C_EP_H_ */
