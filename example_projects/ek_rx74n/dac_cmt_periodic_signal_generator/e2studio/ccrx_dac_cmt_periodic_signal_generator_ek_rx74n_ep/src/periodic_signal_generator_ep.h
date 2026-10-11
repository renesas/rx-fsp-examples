/***********************************************************************************************************************
 * File Name    : periodic_signal_generator_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PERIODIC_SIGNAL_GENERATOR_EP_H_
#define PERIODIC_SIGNAL_GENERATOR_EP_H_

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define EP_INFO       "\r\nThis example project demonstrates the configuration of the DAC, CMT, and DTC"\
		              "\r\nto generate a continuous sinusoidal signal. The user will define the samples"\
                      "\r\nper period and frequency in Hz of the desired output wave via terminal."\
                      "\r\nThe samples per period are used to compute and store one period of the sinusoidal"\
                      "\r\nin a lookup table (LUT), and the frequency is used to set the CMT period to"\
                      "\r\nthe appropriate sampling rate. The CMT generates an interrupt that triggers"\
                      "\r\nthe DTC to transfer the next value of the LUT to the DADR1 register. The output"\
					  "\r\nsinusoidal wave will oscillate between the minimum and maximum DAC values at the"\
                      "\r\nuser-specified samples per period and frequency.\r\n\r\n"

#define SPP_MIN_VAL                 (2)
#define SPP_MAX_VAL                 (256)

#define DAC_MID_VAL                 (2047.5)

#define DADR1                       (0x87833100)
#define DAC_MAX_CONVERSION_PERIOD   (3)

#endif /* PERIODIC_SIGNAL_GENERATOR_EP_H_ */
