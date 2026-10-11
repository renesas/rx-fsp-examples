/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : This file contains data structures and functions used in hal_entry.c.
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

/* External Global Variables */
extern volatile uint8_t g_timer_overflow_counter;

/* Function declarations */
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Private function declarations */
static fsp_err_t read_Input_from_RTT(void);
#if (DISABLE_UNUSED_FEATURES == 1)
static void enable_iwdt_count_in_debug_mode(void);
#endif
static void check_reset_status(void);
extern bsp_leds_t g_bsp_leds;

#define     EP_INFO     "\r\nThis example project demonstrates the typical use of the IWDT HAL module."\
		                "\r\nUser input initializes the IWDT and starts GPT timer. IWDT counter is refreshed"\
						"\r\nperiodically every 1 second when the GPT timer expires."\
						"\r\nOn entering 2 from Terminal Application, the IWDT counter stops refreshing and resets"\
						"\r\nthe MCU. \r\n"
#define     MENU        "\r\nEnter 1 to Enable IWDT\r\n"\
		                "User Input: "

/*******************************************************************************************************************//**
 * The RX Configuration tool generates main() and uses it to generate threads if an RTOS is used. This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                             (bsp_io_level_t) BSP_IO_LEVEL_LOW);
#if (USE_VIRTUAL_COM == 1)
    err = (fsp_err_t) TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON error LED to indicate uart_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                            (bsp_io_level_t) BSP_IO_LEVEL_HIGH);
        /* Error trap */
        ERROR_TRAP;
    }
#endif
    /* Example project information printed on the console */
    APP_PRINT (BANNER_INFO, EP_VERSION, version.version_id_b.major,
            version.version_id_b.minor, version.version_id_b.patch);

    APP_PRINT (EP_INFO);

    /* Check whether reset is caused by IWDT */
    check_reset_status();

    APP_PRINT("\r\nClock source for IWDT is IWDTCLK\r\n");

    /* Initialize GPT in Periodic Mode */
    err = init_gpt_module();
    if (FSP_SUCCESS != err)
    {
        /* Timer init failed */
        APP_ERR_PRINT ("\r\n ** GPT TIMER INIT FAILED ** \r\n");
        APP_ERR_TRAP (err);
    }

    /* Menu for User Selection */
    APP_PRINT (MENU);

    while(true)
    {
        /* Process input only when user has provided one */
        if (APP_CHECK_DATA)
        {
            err = read_Input_from_RTT();
            if (FSP_SUCCESS != err)
            {
                /* Close timer module */
                deinit_gpt_module();
                APP_ERR_TRAP (err);
            }
        }

        /* For every 2 seconds, Terminal Application prints an IWDT refresh message.
         * This is done to avoid continuous print messages on Terminal Application */
        if (IWDT_REFRESH_COUNTER_VALUE == g_timer_overflow_counter)
        {
            g_timer_overflow_counter = RESET_VALUE;
            APP_PRINT("IWDT counter refreshed.\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * This function is called to read user input from RTT using RTT API.
 * @brief       Read user input from RTT. Handle errors internally with proper messages. Application handles the rest.
 * @param[IN]   None
 * @retval      Any other error code apart from FSP_SUCCESS, unsuccessful to perform operation
 **********************************************************************************************************************/
static fsp_err_t read_Input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to store user input */
    char rByte[BUFFER_SIZE_DOWN] = { RESET_VALUE };
    uint8_t user_data = RESET_VALUE;
    static bool b_is_iwdt_enable = false;

    /* Read first byte of data provided by user */
    APP_READ(rByte);

    user_data = (uint8_t) atoi((char *)&rByte);

    /* Perform operation on user input */
    switch (user_data)
    {
        case ENABLE_IWDT:
        {
            /* Set the variable */
            b_is_iwdt_enable = true;
#if (DISABLE_UNUSED_FEATURES == 1)
            /* Enable IWDT to count and generate NMI or reset when the debugger (JLink) is connected. */
            enable_iwdt_count_in_debug_mode();
#endif
            /* Open IWDT. For every GPT timeout, IWDT will get refreshed. */
            err = R_IWDT_Open (&g_iwdt_ctrl, &g_iwdt_cfg);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** R_IWDT_Open API Failed ** \r\n");
                return err;
            }

            /* Start GPT timer in Periodic mode */
            err = timer_start();
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT ("\r\n ** GPT TIMER START FAILED ** \r\n");
                return err;
            }
            /* Print message to indicate application status to user. */
            APP_PRINT ("\r\nIWDT initialized, GPT Timer Started");
            APP_PRINT("\r\nTo stop IWDT counter from refreshing, Enter '2' from Terminal Application\r\n");
        }
        break;

        case STOP_IWDT_REFRESH:
        {
            if (true == b_is_iwdt_enable)
            {
                /* Reset the variable */
                b_is_iwdt_enable = false;
                /* GPT timer will stop running */
                err = R_GPT_Stop(&g_timer_ctrl);
                if (FSP_SUCCESS != err)
                {
                    /* Print error on RTT console */
                    APP_ERR_PRINT("\r\n ** R_GPT_Stop API failed ** \r\n");
                    return err;
                }
                APP_PRINT("\r\n\r\nGPT timer stopped.\r\n");
                APP_PRINT("\r\n*Independent Watchdog Timer will overflow in 17 seconds.*");
            }
            else
            {
                APP_PRINT (MENU);
            }
        }
        break;

        default:
        {
            /* Menu for user selection */
            APP_PRINT ("\r\nInvalid Input");
            APP_PRINT ("\r\nPlease provide input from below option");
            APP_PRINT (MENU);
        }
        break;
    }
    user_data = RESET_VALUE;
    return err;
}

#if (DISABLE_UNUSED_FEATURES == 1)
/*******************************************************************************************************************//**
 * This function is called to enable IWDT counter in debug mode.
 * @brief       Enables IWDT counter to count in debug mode.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void enable_iwdt_count_in_debug_mode(void)
{
    /* As per hardware manual's DBGREG module, section 2.6.4.2:- Clear this bit to enable IWDT Reset/NMI in debug mode */
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_IWDT = RESET_VALUE;
}
#endif

/*******************************************************************************************************************//**
 * This function is called to check whether a reset was caused by IWDT/NMI.
 * @brief       Check if reset was caused by IWDT or NMI.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void check_reset_status(void)
{
    /* Check if reset was caused by the IWDT/NMI  */
    if (SYSTEM_RSTSR1_IWDTRF_DETECT_RESET == R_SYSTEM->RSTSR1_b.IWDTRF)
    {
        /* Clear the flag once the value is read */
        R_SYSTEM->RSTSR1_b.IWDTRF = RESET_VALUE;
        APP_PRINT ("\r\n************************ IWDT Reset detected ************************\r\n");
    }
    else if (SYSTEM_RSTSR1_SWRF_DETECT_RESET == R_SYSTEM->RSTSR1_b.SWRF)
    {
        /* Clear the flag once the value is read */
        R_SYSTEM->RSTSR1_b.SWRF = RESET_VALUE;
        APP_PRINT ("\r\n******************* NMI Software Reset detected *********************\r\n");
    }
    else
    {
        /* None */
    }
}

/*******************************************************************************************************************//**
 * @brief       NMI_callback.
 * @param[IN]   p_args   Callback function parameter data
 * @retval      None
 **********************************************************************************************************************/
void NMI_callback(wdt_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
#if (DISABLE_UNUSED_FEATURES == 1)
    /* Issue a software reset to reset the MCU. */
    __NVIC_SystemReset();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process. This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_iwdt_ep)
 **********************************************************************************************************************/
