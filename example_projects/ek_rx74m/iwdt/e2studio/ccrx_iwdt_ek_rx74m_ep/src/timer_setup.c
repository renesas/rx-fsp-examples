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
 * @addtogroup r_iwdt_ep
 * @{
 **********************************************************************************************************************/

/* Variable to capture timer overflow counts and print IWDT status */
volatile uint32_t g_timer_overflow_counter = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief       Initialize the GPT timer in Periodic mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS        Upon successful open
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t init_gpt_module(void)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT module */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Print error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_GPT_Open API failed ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Start the GPT HAL timer and return the error to the Application.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                 Timer started successfully.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful to start timer
 ***********************************************************************************************************************/
fsp_err_t timer_start(void)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Print error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_GPT_Start API failed ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Close the GPT HAL driver and handle the returned closing API error.
 * @param[IN]   None
 * @retval      None
 ***********************************************************************************************************************/
void deinit_gpt_module(void)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Close the GPT module */
    err=  R_GPT_Close(&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Print error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_GPT_Close API failed ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * This function is called when the GPT timer's counter has wrapped around.
 * @brief       Refresh IWDT counter and toggle LED state.
 * @param[IN]   p_args   Callback function parameter data
 * @retval      None
 **********************************************************************************************************************/
void gpt_callback(timer_callback_args_t *p_args)
{
    /* Variable to track error and return values */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to toggle LED */
    static bsp_io_level_t level_led = BSP_IO_LEVEL_HIGH;

    FSP_PARAMETER_NOT_USED(p_args);

    /* Refresh IWDT */
    err = R_IWDT_Refresh(&g_iwdt_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON LED to indicate error, along with output on RTT */
        TURN_ON_LED;
        /* Print error on RTT console */
        APP_ERR_PRINT ("\r\n ** R_IWDT_Refresh API failed ** \r\n");
    }
    else
    {
        /* Counter is used to count the number of times the GPT callback has triggered. */
        g_timer_overflow_counter++;

        /* Toggle LED */
        level_led ^= BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) GREEN, level_led);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_iwdt_ep)
 **********************************************************************************************************************/
