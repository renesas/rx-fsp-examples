/***********************************************************************************************************************
 * File Name    : lpm_ep.c
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "lpm_ep.h"
#include "source_cancel_lpm.h"

/*******************************************************************************************************************//**
 * @addtogroup r_lpm_ep
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************
* External variables
**********************************************************************************************************************/
extern bsp_leds_t g_bsp_leds;

/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/
/* LPM instances */
static lpm_instance_t const * g_lpm_instance [APP_LPM_MAX_STATE] =
{
     [APP_LPM_SLEEP_STATE] = &g_lpm_sleep,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
     [APP_LPM_DEEP_SLEEP_STATE] = &g_lpm_deep_sleep,
#endif
     [APP_LPM_SW_STANDBY_STATE] = &g_lpm_sw_standby,
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
     [APP_LPM_DEEP_SW_STANDBY_STATE] = &g_lpm_deep_sw_standby,
#endif
};

/* LPM Mode name */
static const uint8_t * g_lpm_mode_name [APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE]                   = (uint8_t *)NAME_LPM_SLEEP_MODE,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    [APP_LPM_DEEP_SLEEP_STATE]              = (uint8_t *)NAME_LPM_DEEP_SLEEP_MODE,
#endif
    [APP_LPM_SW_STANDBY_STATE]              = (uint8_t *)NAME_LPM_SW_STANDBY_MODE,
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    [APP_LPM_DEEP_SW_STANDBY_STATE]         = (uint8_t *)NAME_LPM_DEEP_SW_STANDBY_MODE,
#endif
};

/**********************************************************************************************************************
* Private function declarations
**********************************************************************************************************************/
static fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode);
static fsp_err_t lpm_exit_procedure(app_lpm_states_t lpm_mode);
static fsp_err_t lpm_wait_start(void);
static fsp_err_t lpm_operation(app_lpm_states_t lpm_mode);
static void led_lpm_state_set(led_power_t state);
static void led_error_state_set(led_power_t state);
//static void handle_error(fsp_err_t err_value, uint8_t * err_func, uint8_t * err_str);

/**********************************************************************************************************************
* Function implementations
**********************************************************************************************************************/
fsp_err_t lpm_ep_startup(void){
    fsp_err_t err = FSP_SUCCESS;

    /* Turn off LEDs state at start up */
    led_lpm_state_set(LED_POWER_OFF);
    led_error_state_set(LED_POWER_OFF);


    /* In case of internal reset */
    if (true == g_source_flag)
    {
        /* Print internal reset banner */
        APP_PRINT(BANNER_RESET);

        /* Detect source generates internal reset */
        source_cancel_lpm_detect();

        /* Clear all source detection flags */
        source_cancel_lpm_flag_clear();

        /* Prints a new cycle start message */
        APP_PRINT("\r\nStart a new LPM operating cycle\r\n");
    }

    /* Initialize all modules used to cancel LPM */
    err = source_cancel_lpm_init();
   APP_ERR_RETURN(err, "source_cancel_lpm_init Failed\r\n");

    /* Print instructions to start the LPM operating sequence */
    APP_PRINT("\r\nPress SW1 button or wait 2 seconds to start LPM operating sequence\r\n");

    /* Wait until start-enable event occurs */
   err = lpm_wait_start();
   APP_ERR_RETURN(err,"lpm_wait_start Failed\r\n");

    /* Turn on the blue LED to indicate a successful startup */
    led_lpm_state_set(LED_POWER_ON);

    return err;
}

fsp_err_t lpm_ep_main_process(void)
{
    fsp_err_t err = FSP_SUCCESS;
    app_lpm_states_t lpm_state = APP_LPM_SLEEP_STATE;

    /* Prints LPM operation start message */
    APP_PRINT("\r\nLPM operating is started\r\n");

    /* Example project loop*/
    while (true)
    {
        /* Software delay to next LPM mode */
        R_BSP_SoftwareDelay(LPM_TIME_TRANSITION_VALUE, LPM_TIME_TRANSITION_UNIT);

        /* Perform a LPM operating sequence */
        err = lpm_operation(lpm_state);
       APP_ERR_RETURN(err,"lpm_operation Failed\r\n");

        /* Go to next LPM mode */
        lpm_state ++;

        /* Check end of LPM mode list */
        if (APP_LPM_MAX_STATE <= lpm_state)
        {
            lpm_state = APP_LPM_SLEEP_STATE;
            APP_PRINT("\r\nStart a new LPM operating sequence cycle\r\n");
        }
    }
}



/**********************************************************************************************************************
* Private Function implementations
**********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief      This function will perform a procedure before entering LPM mode
 *  @param[in]  lpm_mode    LPM mode prepares to enter
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Print LPM mode name */
    APP_PRINT("%s\r\n", g_lpm_mode_name[lpm_mode]);

    /* Turn off blue LED in case of LPM mode */
    led_lpm_state_set(LED_POWER_OFF);

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Start/enable hardware modules to cancel LPM mode */
    err = source_cancel_lpm_enable();
    APP_ERR_RETURN(err,"source_cancel_lpm_enable Failed\r\n");


    /* SCI UART module Stop with Undefined in SW Standby and Deep SW Standby mode */
    if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        TERM_DEINIT();
    }

    /* Disable unused IO ports to reduce power */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
   APP_ERR_RETURN(err,"R_IOPORT_PinsCfg API Failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will perform a procedure after exiting LPM mode
 *  @param[in]  lpm_mode    LPM mode just exited
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t lpm_exit_procedure(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Put IO port configuration back to user's selections */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
   APP_ERR_RETURN(err,"R_IOPORT_PinsCfg API Failed\r\n");

    /* Re-initialize SCI UART after SW Standby Mode */
    if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        TERM_INIT();
    }

    (void) lpm_mode;

    /* Stop/disable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_disable();
   APP_ERR_RETURN(err,"source_cancel_lpm_disable Failed\r\n");

    /* Detect and print LPM cancellation source */
    source_cancel_lpm_detect();

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Turn on blue LED in case of normal mode */
    led_lpm_state_set(LED_POWER_ON);

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will perform a sample LPM operation
 *  @param[in]  lpm_mode    LPM mode to operate
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t lpm_operation(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check valid LPM mode */
    if (APP_LPM_MAX_STATE <= lpm_mode)
    {
        APP_ERR_RETURN(FSP_ERR_INVALID_MODE, "This MCU does not support this LPM mode\r\n");
    }

    /* Open this LPM mode */
    err = R_LPM_Open(g_lpm_instance[lpm_mode]->p_ctrl, g_lpm_instance[lpm_mode]->p_cfg);
   APP_ERR_RETURN(err,"R_LPM_Open API Failed\r\n");

    /* Procedure before enter to LPM  */
    err = lpm_enter_procedure(lpm_mode);
   APP_ERR_RETURN(err,"lpm_enter_procedure Failed\r\n");

    /* Enter to LPM */
    err = R_LPM_LowPowerModeEnter(g_lpm_instance[lpm_mode]->p_ctrl);
   APP_ERR_RETURN(err,"R_LPM_LowPowerModeEnter API Failed\r\n");

    /* Procedure after exit LPM */
    err = lpm_exit_procedure(lpm_mode);
   APP_ERR_RETURN(err,"lpm_exit_procedure Failed\r\n");

    /* Close LPM */
    err = R_LPM_Close(g_lpm_instance[lpm_mode]->p_ctrl);
   APP_ERR_RETURN(err,"R_LPM_Close API Failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will wait for trigger to start LPM operation sequence
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t lpm_wait_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable sources used to cancel LPM */
    err = source_cancel_lpm_disable();
   APP_ERR_RETURN(err,"source_cancel_lpm_disable Failed\r\n");

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Start/enable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_enable();
   APP_ERR_RETURN(err,"source_cancel_lpm_enable Failed\r\n");

    /* Wait for an event */
    while (false == g_source_flag);

    /* Stop/disable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_disable();
   APP_ERR_RETURN(err,"source_cancel_lpm_disable Failed\r\n");

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will set the state of the Blue LED
 *  @param[in]  state   LED state want to set
 *  @retval     None
 **********************************************************************************************************************/
static void led_lpm_state_set(led_power_t state)
{
    /* Check board has this LED */
    if (LED_LPM_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t)state);
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function will set the state of the Red LED
 *  @param[in]  state   LED state want to set
 *  @retval     None
 **********************************************************************************************************************/
static void led_error_state_set(led_power_t state)
{
    /* Check board has this LED */
    if (LED_ERROR_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_ERROR_STATE], (bsp_io_level_t)state);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_lpm_ep)
 **********************************************************************************************************************/
