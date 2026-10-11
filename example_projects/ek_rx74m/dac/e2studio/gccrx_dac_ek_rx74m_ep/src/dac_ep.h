/***********************************************************************************************************************
 * File Name    : dac_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DAC_EP_H_
#define DAC_EP_H_

#define EP_INFO             "\r\nThe project initializes and demonstrates DAC module on Renesas RX MCUs using ADC."\
                            "\r\nThe user can enter DAC value within permitted range (1-4095) and the"\
                            "\r\nconverted output is given as input to ADC channel 0. The EP information"\
                            "\r\nor any error messages will be displayed on the Terminal Application.\r\n"

#define DAC_MIN_VAL         (1)
#define DAC_MAX_VAL         (4095)

/* Macro to define the ADC channel to be used for board */
#define ADC_CHANNEL         (ADC_CHANNEL_0)

 /* DAC stabilization time (1ms) */
#define STABILIZATION_DELAY     (1)

#endif /* DAC_EP_H_ */
