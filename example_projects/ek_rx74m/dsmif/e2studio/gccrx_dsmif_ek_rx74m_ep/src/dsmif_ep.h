/*******************************************************************************************************************//**
 * File Name    :  dsmif_ep.h
 * Description  :  This file contains the function declarations and definitions for the DSMIF example project.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DSMIF_EP_H_
#define DSMIF_EP_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "common_utils.h"

/*******************************************************************************************************************//**
 * Macro definitions
 **********************************************************************************************************************/
#define SET_DUTY_CYCLE               (1U)
#define SET_FILTER_AND_DECIMATION    (2U)
#define START_MEASUREMENT            (3U)

#define BUF_SIZE                     (16U)     /* Size of buffer for Console input data */
#define MODULE_CLOSE                 (0U)
#define GPT_MAX_PERCENT              (100U)    /* Max Duty Cycle percentage */
#define ELC_OPEN                     (0U)

#define DSMIF_MAXIMUM_FILTER_SETTLING_TIME_US (58U)

#define TIMER_PIN                    (GPT_IO_PIN_GTIOCB) /* GPT Timer Pin for boards */

#define INITIAL_VALUE                ('\0')

/* Macros for DSMIF filter configuration options:
 * Format: SINCx_y means:
 *   - x: Sinc filter order (1st, 2nd, or 3rd)
 *   - y: Decimation rate (64 or 256)
 */
#define SINC1_64                     (1U)
#define SINC1_256                    (2U)
#define SINC2_64                     (3U)
#define SINC2_256                    (4U)
#define SINC3_64                     (5U)
#define SINC3_256                    (6U)

#define FILTER_SETTING  "\r\nFilter Options"\
                        "\rFilter                Decimation Rate"\
                        "\r\n1: Sinc1                 64"\
                        "\r\n2: Sinc1                 256"\
                        "\r\n3: Sinc2                 64"\
                        "\r\n4: Sinc2                 256"\
                        "\r\n5: Sinc3                 64"\
                        "\r\n6: Sinc3                 256"\
                        "\r\nInput: "

#define MENU            "\r\nMenu"\
                        "\r\n1: Set Duty Cycle"\
                        "\r\n2: Set Filter"\
                        "\r\n3: Start Measurement"\
                        "\r\nInput: "
#define EP_INFO "\r\nThis example demonstrates the use of DSMIF on an RX MCU by simulating the output of a"\
                "\r\nDelta-Sigma Modulator. Users enter a duty cycle via the Terminal Application, which "\
                "\r\ncontrols the LED and drives a 1-bit PWM signal using GPT. DSMIF filters this signal "\
                "\r\nand produces a 16-bit output value. If it exceeds a defined overcurrent threshold, "\
                "\r\nthe LED turns OFF; otherwise, the measured value is printed to the Terminal Application.\r\n"

void dsmif_hal_entry(void);

#endif /* DSMIF_EP_H_ */
