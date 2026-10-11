/***********************************************************************************************************************
 * File Name    : timer_setup.c
 * Description  : This file contains data structures and functions used in timer_setup.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#include "timer_setup.h"

/*******************************************************************************************************************//**
 * @addtogroup r_wdt_ep
 * @{
 **********************************************************************************************************************/

/*
 * External Global Variables
 */
/* LED Structure used to blink on board LED */
extern bsp_leds_t g_bsp_leds;

/*
 * Global Variables
 */
/* Variable to capture timer overflow counts and print WDT status */
volatile uint32_t g_timer_overflow_counter = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief       Initialize the GPT timer in Periodic mode.
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful open
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t init_gpt_module(void)
{
    /* variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT module */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT("\r\n ** R_GPT_TimerOpen API failed ** \r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       Start the GPT HAL timer and return the error to the Application.
 * @param[in]   None
 * @retval      FSP_SUCCESS                 Timer started successfully.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful to start timer
 ***********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    /* variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT("\r\n ** R_GPT_Start API failed ** \r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       Close the GPT HAL driver and Handle the return closing API error, to the Application.
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void deinit_gpt_module(void)
{
    /* variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Close the GPT module */
    err = R_GPT_Close(&g_timer_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* Print Error on RTT console */
        APP_ERR_PRINT("\r\n ** R_GPT_Close API failed ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * This function is called when GPT timer's counter wrapped around.
 * @brief       Refresh WDT counter and toggle LED state.
 * @param[in]   p_args   Callback function parameter data
 * @retval      None
 **********************************************************************************************************************/
void gpt_callback(timer_callback_args_t *p_args)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to toggle LED */
    static bsp_io_level_t level_led = BSP_IO_LEVEL_HIGH;

    FSP_PARAMETER_NOT_USED(p_args);

    /* Refresh WDT, if user has not pressed the push button */
    err = R_WDT_Refresh(&g_wdt_ctrl);

    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error, along with output on Terminal Application */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[BSP_LED_LED3], BSP_IO_LEVEL_HIGH);

        /* Print Error on Terminal Application */
        APP_ERR_PRINT("\r\n ** R_WDT_Refresh API failed ** \r\n");
    }
    else
    {
        /* Counter is used to count the number of times GPT callback triggered. */
        g_timer_overflow_counter++;

        /* Toggle LED */
        level_led ^= BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[BSP_LED_LED1], level_led);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_wdt_ep)
 **********************************************************************************************************************/
