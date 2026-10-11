/***********************************************************************************************************************
 * File Name    : lpm_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef LPM_EP_H_
#define LPM_EP_H_

#include "common_utils.h"

/**********************************************************************************************************************
* Macro definitions
**********************************************************************************************************************/
#define EP_INFO         "\r\nThe Example Project demonstrates the basic functionality of LPM driver on Renesas \r\n"\
                        "RX MCUs based on FSP. On successful initialization of necessary hardware modules,\r\n"\
                        "the MCU will automatically enter each LPM mode and then wait for a wake-up source\r\n"\
                        "trigger, either manually by an external interrupt or automatically by timer.\r\n\n"

#define BANNER_RESET    "\r\n********************************************************************************"\
                        "\r\n*   MCU has been reset from Deep SW Standby mode by internal reset             *"\
                        "\r\n********************************************************************************\r\n"

/* Macro for define LED on board */
#define LED_LPM_STATE                           (0U)
#define LED_ERROR_STATE                         (2U)

/* Macro for delay time in application */
#define LPM_TIME_TRANSITION_VALUE               (500U)
#define LPM_TIME_TRANSITION_UNIT                (BSP_DELAY_UNITS_MILLISECONDS)

/* Macro for LPM cancel source name */
#define NAME_LPM_CANCEL_SOURCE_TIMER            "\r\nLPM is canceled by RTC0 module\r\n"


#define NAME_LPM_CANCEL_SOURCE_IRQ              "\r\nLPM is canceled by ICU External IRQ module\r\n"
#define NAME_LPM_CANCEL_SOURCE_NOT_DETECTED     "\r\nLPM is canceled, but source has not been detected\r\n"

/* Macro for LPM mode name */
#define NAME_LPM_SLEEP_MODE                     "\r\nMCU enters Sleep mode\r\n"
#define NAME_LPM_DEEP_SLEEP_MODE                "\r\nMCU enters Deep Sleep mode\r\n"
#define NAME_LPM_SW_STANDBY_MODE                "\r\nMCU enters SW Standby mode\r\n"
#define NAME_LPM_DEEP_SW_STANDBY_MODE           "\r\nMCU enters Deep SW Standby mode\r\n"

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Enumeration for low power mode use in application */
typedef enum e_app_lpm_state
{
    APP_LPM_SLEEP_STATE = 0,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    APP_LPM_DEEP_SLEEP_STATE,
#endif
    APP_LPM_SW_STANDBY_STATE,

#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    APP_LPM_DEEP_SW_STANDBY_STATE,
#endif
    APP_LPM_MAX_STATE
} app_lpm_states_t;

/* Enumeration for led state use in application */
typedef enum e_led_power
{
    LED_POWER_ON = BSP_IO_LEVEL_HIGH,
    LED_POWER_OFF = BSP_IO_LEVEL_LOW,
}led_power_t;

/**********************************************************************************************************************
* Public functions declarations
**********************************************************************************************************************/
fsp_err_t lpm_ep_startup(void);
fsp_err_t lpm_ep_main_process(void);


#endif /* LPM_EP_H_ */
