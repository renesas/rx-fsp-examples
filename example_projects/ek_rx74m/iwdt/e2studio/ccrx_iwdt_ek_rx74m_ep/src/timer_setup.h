/***********************************************************************************************************************
 * File Name    : timer_setup.h
 * Description  : This file contains data structures and functions used in timer_setup.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TIMER_SETUP_H_
#define TIMER_SETUP_H_

#define GREEN                   (IOPORT_PORT_08_PIN_07)
#define LED_COUNT               (3U)

#define TURN_ON_LED             (R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) GREEN, BSP_IO_LEVEL_HIGH))

/* IWDT/NMI detect reset value */
#define SYSTEM_RSTSR1_IWDTRF_DETECT_RESET       (1u)
#define SYSTEM_RSTSR1_SWRF_DETECT_RESET         (1u)

/* Number of counts for printing IWDT refresh status */
#define IWDT_REFRESH_COUNTER_VALUE              (2u)
/* User command input values */
#define ENABLE_IWDT                             (1u)
#define STOP_IWDT_REFRESH                       (2u)

/* Function declarations */
fsp_err_t init_gpt_module(void);
fsp_err_t timer_start(void);
void deinit_gpt_module(void);

#endif /* TIMER_SETUP_H_ */
