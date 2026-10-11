/***********************************************************************************************************************
 * File Name    : tmr_ep.c
 * Description  : Implements Timer example project with PWM, One-Shot, and Periodic modes.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup TMR_EP
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "tmr_ep.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
bool volatile g_one_shot_expired = false;
/* Store timer open state */
uint8_t g_timer_open_state = RESET_VALUE;

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
static fsp_err_t tmr_pwm_mode(void);
static fsp_err_t tmr_oneshot_mode(void);
static fsp_err_t tmr_periodic_mode(void);
static fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent);
static fsp_err_t init_tmr_timer(timer_ctrl_t *const p_timer_ctl, timer_cfg_t const *const p_timer_cfg,\
                                uint8_t timer_mode);
static fsp_err_t start_tmr_timer(timer_ctrl_t *const p_timer_ctl);
static void handle_error(fsp_err_t err, timer_ctrl_t *const p_timer_ctl , char *err_str);

/* Public function declarations */
uint32_t process_input_data(void);
void deinit_tmr_timer(timer_ctrl_t *const p_timer_ctl);
extern bsp_leds_t g_bsp_leds;

/***********************************************************************************************************************
*  Function Name: tmr_hal_entry
*  Description  : This function is used to demonstrate the TMR example project.
*  Arguments    : None
*  Return Value : None
***********************************************************************************************************************/
void tmr_hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Turn OFF error LED */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                         (bsp_io_level_t)BSP_IO_LEVEL_LOW);
#if (USE_VIRTUAL_COM == 1)

    err= (fsp_err_t) TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON error LED to indicate uart_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                          (bsp_io_level_t) BSP_IO_LEVEL_HIGH);
        /* Error trap */
        ERROR_TRAP;
    }
#endif /* USE_VIRTUAL_COM */

    /* Print EP information */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major,

            version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);
    APP_PRINT(MENU);
    while (true)
    {
        if (APP_CHECK_DATA)
        {
            /* Read first byte of data provided by user */
            char rByte[SIZE_64] = {RESET_VALUE};
            APP_READ(rByte);

            /* Conversion from input string to integer value */
            uint32_t read_data = (uint32_t) (atoi((char*) rByte));

            /* Handle user selection from menu */
            switch (read_data)
            {
                case PWM_MODE:
                {
                    /* Handle PWM mode */
                    err = tmr_pwm_mode();
                    handle_error(err, &g_pwm_timer_ctrl , "TMR Timer PWM Mode Failed");
                    break;
                }
                case ONE_SHOT_MODE:
                {
                    /* Handle One-Shot mode */
                    err = tmr_oneshot_mode();
                    handle_error(err, &g_one_shot_timer_ctrl , "TMR Timer One-Shot Mode Failed");
                    break;
                }
                case PERIODIC_MODE:
                {
                    /* Handle Periodic mode */
                    err = tmr_periodic_mode();
                    handle_error(err, &g_periodic_timer_ctrl , "TMR Timer Periodic Mode Failed");
                    break;
                }
                default:
                {
                    APP_PRINT("\r\nInvalid Menu Option Selected\r\n");
                    break;
                }
            }
            /* Re-print the user menu after handling input */
            APP_PRINT(MENU);
        }
    }
}

/***********************************************************************************************************************
* End of function tmr_hal_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: tmr_pwm_mode
*  Description  : This function is used to demonstrates TMR operation in PWM mode
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
***********************************************************************************************************************/
static fsp_err_t tmr_pwm_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t tmr_desired_duty_cycle_percent = RESET_VALUE;

    APP_PRINT("\r\nEnter The Desired Duty Cycle in Percentage of Range 0 - 100: ");
    tmr_desired_duty_cycle_percent = process_input_data();

    /* Validate duty cycle percentage */
    if (TMR_MAX_PERCENT < tmr_desired_duty_cycle_percent)
    {
        APP_PRINT("\r\n INVALID INPUT, DESIRED DUTY CYCLE IS OUT OF RANGE. \r\n");
    }
    else
    {
        /* Check the status of TMR timer in PWM mode */
        if (PWM_MODE != g_timer_open_state)
        {
            /* We got valid input, initialize PWM timer */
            err = init_tmr_timer (&g_pwm_timer_ctrl, &g_pwm_timer_cfg, PWM_MODE);
            /* Handle error */
            APP_ERR_RETURN(err, "** TMR TIMER INIT FAILED ** \r\n");

            APP_PRINT("\r\nOpened TMR timer in PWM Mode\r\n");

            /* Start PWM timer */
            err = start_tmr_timer (&g_pwm_timer_ctrl);
            if (FSP_SUCCESS != err)
                {
                    APP_ERR_RETURN(err, "** TMR TIMER START FAILED ** \r\n");
                    /* Close PWM timer instance */
                    deinit_tmr_timer (&g_pwm_timer_ctrl);
                    APP_ERR_TRAP(err);
                }
            APP_PRINT("Started TMR timer in PWM Mode\r\n");
        }
        else
        {
            /* Print duty cycle on Virtual console, when PWM Timer already started */
            APP_PRINT("PWM Timer Already started, Update Duty Cycle to: %d\r\n", tmr_desired_duty_cycle_percent);
        }

        /* Set duty cycle of PWM timer */
        err = set_timer_duty_cycle ((uint8_t) (tmr_desired_duty_cycle_percent));

        /* Handle error */
        APP_ERR_RETURN(err, "** TMR TIMER DUTYCYCLE SET FAILED ** \r\n");
    }
    return err;
}

/***********************************************************************************************************************
* End of function tmr_pwm_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: tmr_oneshot_mode
*  Description  : This function is used to demonstrates TMR operation in One-Shot mode.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t tmr_oneshot_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t one_shot_timeout = INT32_MAX;

    /* Check the status of TMR timer in PWM mode */
    if (PWM_MODE == g_timer_open_state)
    {
        /* Close PWM timer instance */
        deinit_tmr_timer (&g_pwm_timer_ctrl);
    }
    /* Check the status of TMR timer in One-Shot mode */
    if (ONE_SHOT_MODE == g_timer_open_state)
    {
        /* Close One-Shot timer instance */
        deinit_tmr_timer (&g_one_shot_timer_ctrl);
    }
    /* Initialize One-Shot timer */
    err = init_tmr_timer (&g_one_shot_timer_ctrl, &g_one_shot_timer_cfg, ONE_SHOT_MODE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** TMR TIMER INIT FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nOpened TMR timer in One-Shot Mode\r\n");
    /* Start One-Shot timer */
    err = start_tmr_timer (&g_one_shot_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** TMR TIMER START FAILED ** \r\n");
        /* Close One-Shot timer instance */
        deinit_tmr_timer (&g_one_shot_timer_ctrl);
        APP_ERR_TRAP(err);
    }
    APP_PRINT("Started TMR timer in One-Shot Mode\r\n");
    /* Wait for One-Shot mode timer to expire. */
    while (true != g_one_shot_expired)
    {
        /* Start checking for time out to avoid infinite loop */
        --one_shot_timeout;
        if (RESET_VALUE == one_shot_timeout)
        {
            /* We have reached to a scenario where One-Shot event did not occur */
            APP_ERR_PRINT("Callback event not received during One-Shot operation\r\n");
            break;
        }
    }
    if (true == g_one_shot_expired)
    {
        APP_PRINT("\r\nTimer Expired in One-Shot Mode\r\n");
    }
    g_one_shot_expired = false;
    return err;
}

/***********************************************************************************************************************
* End of function tmr_oneshot_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: tmr_periodic_mode
*  Description  : This function is used to demonstrates TMR operation in Periodic mode.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t tmr_periodic_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t tmr_desired_period_ms = RESET_VALUE;
    uint32_t pclkb_freq_hz = RESET_VALUE;
    uint32_t period_counts = RESET_VALUE;
    /* Check the status of TMR timer in Periodic mode */
    APP_PRINT("\r\nEnter the desired period in millisecond "PERIOD_RANGE"\r\n");
    tmr_desired_period_ms = process_input_data();

    /* Get the source clock frequency (in Hz) */
    pclkb_freq_hz = R_FSP_SystemClockHzGet (FSP_PRIV_CLOCK_PCLKB);
    pclkb_freq_hz >>= (uint32_t) (g_periodic_timer_cfg.source_div);

    /* Convert period to PCLK counts so it can be set in hardware. */
    period_counts = (uint64_t) ((tmr_desired_period_ms * pclkb_freq_hz) / TIMER_UNITS_MILLISECONDS);

    /* Validate Period Count based on user input (time period in ms) */
    if (TMR_MAX_PERIOD_COUNT < period_counts)
    {
        APP_PRINT("\r\n INVALID INPUT, DESIRED PERIOD IS OUT OF RANGE. \r\n");
    }
    else
    {
        /* Check the status of TMR timer in Periodic mode */
        if (PERIODIC_MODE != g_timer_open_state)
        {
            /*Initialize Periodic timer */
            err = init_tmr_timer (&g_periodic_timer_ctrl, &g_periodic_timer_cfg, PERIODIC_MODE);

            APP_ERR_RETURN(err, "** TMR TIMER INIT FAILED ** \r\n");
            APP_PRINT("\r\nOpened Timer in Periodic Mode\r\n");

            err = start_tmr_timer (&g_periodic_timer_ctrl);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_RETURN(err, "** TMR TIMER START FAILED ** \r\n");
                /* Close Periodic timer instance */
                deinit_tmr_timer (&g_periodic_timer_ctrl);
                APP_ERR_TRAP(err);
            }
            APP_PRINT("Started Timer in Periodic Mode\r\n");
        }
        else
        {
            APP_PRINT("Periodic Timer Already Started, Update Timer Period to : %d\r\n", tmr_desired_period_ms);
        }

        /* Period set API set the desired period counts on the on-board LED */
        err = R_TMR_PeriodSet (&g_periodic_timer_ctrl, (uint32_t) period_counts);

        /* Handle error */
        APP_ERR_RETURN(err, "** R_TMR_PeriodSet API FAILED ** \r\n");
    }
    return err;
}

/***********************************************************************************************************************
* End of function tmr_periodic_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: init_tmr_timer
*  Description  : This function is used to initialize TMR timer.
*  Arguments    : p_timer_ctrl   Pointer to the control structure for the TMR instance.
*                 p_timer_cfg    Pointer to the configuration structure for the TMR instance.
*                 timer_mode
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t init_tmr_timer(timer_ctrl_t *const p_timer_ctrl, timer_cfg_t const *const p_timer_cfg,
        uint8_t timer_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initialize TMR Timer */
    err = R_TMR_Open (p_timer_ctrl, p_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** R_TMR_Open API FAILED ** \r\n");
        return err;
    }
    if (PERIODIC_MODE == timer_mode)
    {
        g_timer_open_state = PERIODIC_MODE;
    }
    else if (PWM_MODE == timer_mode)
    {
        g_timer_open_state = PWM_MODE;
    }
    else
    {
        g_timer_open_state = ONE_SHOT_MODE;
    }
    return err;
}

/***********************************************************************************************************************
* End of function init_tmr_timer
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: start_tmr_timer
*  Description  : This function is used to start the TMR timer.
*  Arguments    : p_timer_ctrl   Pointer to the control structure for the TMR instance.
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t start_tmr_timer(timer_ctrl_t *const p_timer_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Starts TMR timer */
    err = R_TMR_Start (p_timer_ctrl);

    /* Handle error */
    APP_ERR_RETURN(err, "** R_TMR_Start API FAILED ** \r\n");
    return err;
}

/***********************************************************************************************************************
* End of function start_tmr_timer
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: set_timer_duty_cycle
*  Description  : This function is used to set Duty Cycle of PWM timer.
*  Arguments    : duty_cycle_percent
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t duty_cycle_counts = RESET_VALUE;
    uint32_t current_period_counts = RESET_VALUE;
    timer_info_t info = { (timer_direction_t) RESET_VALUE, RESET_VALUE, RESET_VALUE };
    /* Get the current period setting. */
    err = R_TMR_InfoGet (&g_pwm_timer_ctrl, &info);

    /* Handle error */
    APP_ERR_RETURN(err, "\r\n** R_TMR_InfoGet API FAILED ** \r\n");

    /* Update period counts locally. */
    current_period_counts = info.period_counts;

    /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
     * UINT32_MAX / 100, this calculation could overflow. A cast to uint64_t is used to prevent this. The cast is
     * not required for 8-bit timers. */
    duty_cycle_counts = (uint32_t) ((current_period_counts * duty_cycle_percent) / TMR_MAX_PERCENT);

    err = R_TMR_DutyCycleSet (&g_pwm_timer_ctrl, duty_cycle_counts, TIMER_PIN);
    /* Handle error */
    APP_ERR_RETURN(err, "\r\n** R_TMR_DutyCycleSet API FAILED ** \r\n");
    return err;
}

/***********************************************************************************************************************
* End of function set_timer_duty_cycle
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: process_input_data
*  Description  : This function is used to process input string to integer value
*  Arguments    : None
*  Return Value : Integer value of input string.
**********************************************************************************************************************/
uint32_t process_input_data(void)
{
    char buf[BUF_SIZE] = { INITIAL_VALUE };
    uint32_t num_bytes = RESET_VALUE;
    uint32_t value = RESET_VALUE;

    while (RESET_VALUE == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            num_bytes = APP_READ(buf);
            if (RESET_VALUE == num_bytes)
            {
                APP_PRINT("\r\nInvalid Input\r\n");
            }
        }
    }

    /* Conversion from input string to integer value */
    value = (uint32_t) (atoi ((char*) buf));
    return value;
}

/***********************************************************************************************************************
* End of function process_input_data
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: deinit_tmr_timer
*  Description  : This function is used to close the TMR timer.
*  Arguments    : p_timer_ctrl   Pointer to the control structure for the TMR instance.
*  Return Value : None
**********************************************************************************************************************/
void deinit_tmr_timer(timer_ctrl_t *const p_timer_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Timer Close API call */
    err = R_TMR_Close (p_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* TMR Close failure message */
        APP_ERR_PRINT("\r\n ** R_TMR_Close API FAILED ** \r\n");
    }
    /* Reset open state of timer */
    g_timer_open_state = RESET_VALUE;
}

/***********************************************************************************************************************
* End of function deinit_tmr_timer
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: handle_error
*  Description  : This function is used to print the error, clean up the module, and trap the error.
*  Arguments    : p_timer_ctrl   Timer instance controller
*                 err_str        Error string
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static void handle_error(fsp_err_t err, timer_ctrl_t *const p_timer_ctrl , char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);
        /* De-initializes module peripherals */
        deinit_tmr_timer(p_timer_ctrl);
        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: one_shot_callback
*  Description  : This function is used to callback for TMR time overflow.
*  Arguments    : p_args    Pointer to callback arguments structure containing timer event information.
*  Return Value : None
**********************************************************************************************************************/
void one_shot_callback(timer_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        if (TIMER_EVENT_CYCLE_END == p_args->event)
        {
            /* Set boolean flag on One-Shot mode timer expired. */
            g_one_shot_expired = true;
        }
    }
}

/***********************************************************************************************************************
* End of function one_shot_callback
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @} (end addtogroup TMR_EP)
***********************************************************************************************************************/
