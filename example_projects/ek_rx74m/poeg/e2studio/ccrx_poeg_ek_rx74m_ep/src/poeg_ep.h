/***********************************************************************************************************************
 * File Name    : poeg_ep.h
 * Description  : Contains macros and function declarations.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef POEG_EP_H_
#define POEG_EP_H_

#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for commands to be received through RTT input */
#define POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL          (1U)
#define POEG_OUTPUT_DISABLE_USING_SOFTWARE_API          (2U)
#define POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL       (3U)
#define POEG_RESET                                      (4U)

#define POEG_CHANNEL_0                                  (1U)
#define POEG_CHANNEL_1                                  (2U)

#define BLUE_LED                (IOPORT_PORT_15_PIN_05)
#define LED_COUNT               (3U)

#define TURN_LED_ON             (R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) BLUE_LED, BSP_IO_LEVEL_HIGH))
#define TURN_LED_OFF            (R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) BLUE_LED, BSP_IO_LEVEL_LOW))

/* Macro for GTETRG pin */
#define GTETRG_PIN                                      "P310 (J1:36)"

#define SET_FLAG                                        (1)
#define CLEAR_FLAG                                      (0)

#define NULL_CHAR                                       ('\0')

#define EP_INFO   "\r\nThis project demonstrates the basic functionalities of the Port Output Enable for GPT (POEG)"\
                  "\r\ndriver on Renesas RX MCUs based on Renesas FSP. The user can disable the GPT output pins using"\
                  "\r\none of the following options:"\
                  "\r\n"EP_MENU""\
                  "\r\nInformation, guideline and error messages are displayed on the Terminal Application."\
                  "\r\nLED1 is turned ON when the GPT output pin is disabled successfully. LED1 is turned OFF"\
                  "\r\nwhen POEG resets successfully. Additionally, the user can verify the EP operation using a"\
                  "\r\nlogic analyzer.\r\n"

#define MENU_TITLE                                      "\r\nMain Menu\r\n"

#define EP_MENU                 "1: Disable GPT output pins based on the GTETRG input pin level\r\n"\
                                "2: Disable GPT output pins by software API\r\n"\
                                "3: Disable GPT output pins when GTIOC pins are at the same logic level\r\n"\
                                "4: Reset the POEG status\r\n"

#define OPTION_1_INFO           "\r\nRemove connection of GTETRG pin and reset of application are needed prior\r\n"\
                                "to repeatedly selecting menu option '1' or selecting other menu options "\
                                "(except option '4')\r\n"\
                                "User Input:  "

#define RESET_MENU              "\r\nReset Menu\r\n"\
                                "1: Reset POEG channel 0 (Required after running option 3)\r\n"\
                                "2: Reset POEG channel 1 (Required after running options 1 or 2)\r\n"\
                                "User Input:  "

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);
void deinit_poeg_module(void);
fsp_err_t init_poeg0_module(void);
fsp_err_t init_poeg1_module(void);
fsp_err_t init_gpt_timer_pwm1(void);
fsp_err_t init_gpt_timer_pwm2(void);
fsp_err_t deinit_poeg0_module(void);
fsp_err_t deinit_poeg1_module(void);
fsp_err_t poeg_output_disable_and_reset_options(void);
fsp_err_t reset_poeg_module(void);

#endif /* POEG_EP_H_ */
