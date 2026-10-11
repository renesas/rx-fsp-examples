/***********************************************************************************************************************
 * File Name    : cmt_ep.c
 * Description  : This file implements the CMT functionality for the example project. It handles CMT in One-Shot mode 
 *                and Periodic mode
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup cmt_ep
 * @{
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * Include Section
 **********************************************************************************************************************/
#include "cmt_ep.h"

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
/* Private function declarations */
/* This function is used to initialize CMT module */
static void cmt_init(void);

/* This function is used to de-initialize CMT module */
static void cmt_deinit(void);

/* This function is used to reset counter value to 0 */
static void cmt_reset(void);

/* This function is used to configure Timer */
static void cmt_config(e_timer_input_type timer_input_type, cmt_instance_ctrl_t * const timer_instance);

/* This function is used to start Timer */
static void cmt_start(e_timer_input_type timer_input_type, e_timer_mode timer_mode, e_timer_function timer_function,
                      cmt_instance_ctrl_t *const timer_instance);

/* This function is used to Handle error */
static void handle_error(fsp_err_t err, const char *api_name, const char *user_msg);

/* This function is used to turn off all LEDs on board */
static void turn_off_all_leds(void);

/* This function is used to calculate the timer max period */
static float max_period_calculate(cmt_instance_ctrl_t *const timer_instance);

/* This function is used to calculate the timer min period */
static float min_period_calculate(cmt_instance_ctrl_t *const timer_instance);

/* This function is used to calculate the timer max frequency */
static float max_frequency_calculate(cmt_instance_ctrl_t *const timer_instance);

/* This function is used to calculate the timer min frequency */
static float min_frequency_calculate(cmt_instance_ctrl_t *const timer_instance);

/* This function is used to get input string value and convert to integer value */
static uint32_t get_input_int(void);

/* This function is used to get timer input type from user (input is Period(ms) or Frequency(Hz)) */
static void get_timer_input_type(void);

/* This function is used to round up value */
static float round_up(float float_value);

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
#if (IO_SUPPORT == 1)
/* Global variables contain information about the LEDs on the board */
extern bsp_leds_t g_bsp_leds;
#endif

/* Use for toggle led */
static volatile bsp_io_level_t led_level = BSP_IO_LEVEL_HIGH;
/* CMT is initialized */
static bool g_cmt_initialized = false;
/* User option */
uint32_t g_user_option = {RESET_VALUE};
/* Clock frequency used for timer */
uint32_t g_clock_freq = {RESET_VALUE};
/* Timer mode */
uint8_t g_timer_mode = {RESET_VALUE};
/* Raw count */
uint32_t g_raw_counts = {RESET_VALUE};
/* Timer period */
uint64_t g_timer_period = {RESET_VALUE};
/* Timer frequency */
uint64_t g_timer_frequency = {RESET_VALUE};
/* Timer input type: millisecond or Hz */
e_timer_input_type g_timer_input_type = PERIOD_INPUT_TYPE;
/* Timer status */
timer_status_t g_timer_status;
/* Timer configuration information */
timer_info_t g_timer_info =
{
    .count_direction = (timer_direction_t)RESET_VALUE,
    .clock_frequency = RESET_VALUE,
    .period_counts = RESET_VALUE,
};

/* The variable is used to check if the One-Shot callback was completed */
static volatile bool g_is_oneshot_callback_completed = false;

/* The variable is used to check time out */
uint32_t timer_time_out = UINT32_MAX;

extern bsp_leds_t g_bsp_leds;

 /**********************************************************************************************************************
 * Function Definition Section
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 *  @brief       This function is used to initialize related module and start the CMT example operation.
 * 
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
void cmt_ep_entry(void)
{
    /* Version information */
    fsp_pack_version_t version = {RESET_VALUE};

    /* Read buffer */
    char rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};

    /* Fetch FSP version to display on console */
    R_FSP_VersionGet (&version);

    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

#if (USE_VIRTUAL_COM == 1)
    /* Turn OFF error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl,(bsp_io_port_pin_t)
            g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_LOW);

    /* Initialize terminal */
    err = (fsp_err_t)TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON error LED to indicate uart_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl,
                (bsp_io_port_pin_t)g_bsp_leds.p_leds[ERR_LED],
                (bsp_io_level_t)BSP_IO_LEVEL_HIGH);

        /* Error trap */
        ERROR_TRAP;
    }
#endif

    /* Example Project information printed on the virtual Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
                version.version_id_b.patch);

    APP_PRINT(CMT_INFO);

    APP_PRINT(EP_MENU);
 
    while (true)
    {
        if (APP_CHECK_DATA)
        {
            /* Cleaning buffer */
            memset(&rByte[0], NULL_CHAR, BUFFER_SIZE_DOWN);

            /* Get input */
            APP_READ(rByte);

            /* Conversion from input string to integer value */
            g_user_option = (uint32_t)(atoi((char *) rByte));

            /* Turn off all LEDs on board */
            /* Ensure all LEDs are off before running the EP */
            turn_off_all_leds();

            /* Close all opened modules */
            /* Ensure all modules are off before running the EP */
            cmt_deinit();

            switch (g_user_option)
            {
                case ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION:
                {
                    APP_PRINT("\r\n**** Run the EP with One-Shot mode operates together with "
                                "Compare Match function **** \r\n");

                    APP_PRINT(INPUT_OPTION);

                    /* Get timer input type: Period(ms) or Frequency(Hz) */
                    get_timer_input_type();

                    /* Initialize Timer */
                    cmt_init();

                    /* Configuration */
                    cmt_config(g_timer_input_type, &g_timer_one_shot_compare_match_ctrl);

                    /* Start timer */
                    cmt_start(g_timer_input_type, ONE_SHOT_MODE, COMPARE_MATCH_FUNCTION,
                                &g_timer_one_shot_compare_match_ctrl);

                    /* Reset timer */
                    cmt_reset();

                    /* Wait for One-Shot mode timer to expire. */
                    while (g_is_oneshot_callback_completed == false)
                    {
                        /* Start checking for time out to avoid infinite loop */
                        --timer_time_out;

                        /* Check for time elapse */
                        if (RESET_VALUE == timer_time_out)
                        {
                            /* We have reached to a scenario where One-Shot event did not occur */
                            APP_ERR_PRINT("\r\nCallback event not received during One-Shot operation \r\n");
                            break;
                        }
                    }

                    /* Check whether the callback has been completed or not */
                    if (g_is_oneshot_callback_completed == true)
                    {
                        /* Reset flag */
                        g_is_oneshot_callback_completed = false;

                        APP_PRINT("\r\nTimer Expired in One-Shot Mode \r\n");
                        APP_PRINT(EP_MENU);
                    }
                }
                break;

                case PERIODIC_MODE_COMPARE_MATCH_FUNCTION:
                {
                    APP_PRINT("\r\n**** Run the EP with Periodic mode operates together with "
                                "Compare Match function **** \r\n");
                    APP_PRINT(INPUT_OPTION);

                    /* Get timer input type: Period(ms) or Frequency(Hz) */
                    get_timer_input_type();

                    /* Initialize Timer */
                    cmt_init();

                    /* Configuration */
                    cmt_config(g_timer_input_type, &g_timer_periodic_compare_match_ctrl);

                    /* Start timer */
                    cmt_start(g_timer_input_type, PERIODIC_MODE, COMPARE_MATCH_FUNCTION,
                                &g_timer_periodic_compare_match_ctrl);

                    /* Reset timer */
                    cmt_reset();
                    
                    APP_PRINT(EP_MENU);
                }
                break;

                default:
                {
                    APP_PRINT(EP_MENU);
                }
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function initializes CMT module.
 * 
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
static void cmt_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    if (MODULE_CLOSE == g_timer_one_shot_compare_match_ctrl.open)
    {
        /* Open Timer-0 in One-Shot mode and operates together with Compare Match function */
        err = R_CMT_Open(&g_timer_one_shot_compare_match_ctrl, &g_timer_one_shot_compare_match_cfg);

        /* Handle error */
        handle_error(err, "cmt_init", "\r\nTimer mode: One-Shot \r\n"
                                        "\r\nTimer function: Compare Match \r\n"
                                        "\r\nTimer channel: 0 \r\n"
                                        "\r\nAPI failed: R_CMT_Open \r\n");
    }

    if (MODULE_CLOSE == g_timer_periodic_compare_match_ctrl.open)
    {
        /* Open Timer-1 in Periodic mode operates together with Compare Match function */
        err = R_CMT_Open(&g_timer_periodic_compare_match_ctrl, &g_timer_periodic_compare_match_cfg);

        /* Handle error */
        handle_error(err, "cmt_init", "\r\nTimer mode: Periodic \r\n"
                                        "\r\nTimer function: Compare Match \r\n"
                                        "\r\nTimer channel: 1 \r\n"
                                        "\r\nAPI failed: R_CMT_Open \r\n");
    }
    g_cmt_initialized = true;
}

/*******************************************************************************************************************//**
 * @brief       This function closes the opened CMT modules before the project ends up in an Error Trap.
 * 
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
static void cmt_deinit(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Close Timer-0 in One-Shot mode and operates together with Compare Match function */
    if (MODULE_CLOSE != g_timer_one_shot_compare_match_ctrl.open)
    {
        /* Close One-Shot timer module */
        err = R_CMT_Close(&g_timer_one_shot_compare_match_ctrl);

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* One-Shot timer close failure message */
            APP_ERR_PRINT("\r\nTimer mode: One-Shot \r\n"
                            "\r\nTimer function: Compare Match \r\n"
                            "\r\nTimer channel: 0 \r\n"
                            "\r\nAPI failed: R_CMT_Close \r\n");

            APP_ERR_TRAP(err);
        }
    }

    /* Close Timer-1 in Periodic mode and operates together with Compare Match function */
    if (MODULE_CLOSE != g_timer_periodic_compare_match_ctrl.open)
    {
        /* Close Period timer module */
        err = R_CMT_Close(&g_timer_periodic_compare_match_ctrl);

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* Period timer close failure message */
            APP_ERR_PRINT("\r\nTimer mode: Periodic \r\n"
                            "\r\nTimer function: Compare Match \r\n"
                            "\r\nTimer channel: 1 \r\n"
                            "\r\nAPI failed: R_CMT_Close \r\n");

            APP_ERR_TRAP(err);
        }
    }

    g_cmt_initialized = false;
}

/*******************************************************************************************************************//**
 * @brief       This function resets the counter value of the opened CMT module to 0.
 * 
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
static void cmt_reset(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Reset Timer-0 in One-Shot mode and operates together with Compare Match function */
    if (MODULE_CLOSE != g_timer_one_shot_compare_match_ctrl.open)
    {
        /* Reset timer */
        err = R_CMT_Reset(&g_timer_one_shot_compare_match_ctrl);

        /* Handle error */
        handle_error(err, "cmt_reset", "\r\nTimer mode: One-Shot \r\n"
                                        "\r\nTimer function: Compare Match \r\n"
                                        "\r\nTimer channel: 0 \r\n"
                                        "\r\nAPI failed: R_CMT_Reset \r\n");
    }

    /* Reset Timer-1 in Periodic mode and operates together with Compare Match function */
    if (MODULE_CLOSE != g_timer_periodic_compare_match_ctrl.open)
    {
        /* Reset Timer-1 */
        err = R_CMT_Reset(&g_timer_periodic_compare_match_ctrl);

        /* Handle error */
        handle_error(err, "cmt_reset", "\r\nTimer mode: Periodic \r\n"
                                        "\r\nTimer function: Compare Match \r\n"
                                        "\r\nTimer channel: 1 \r\n"
                                        "\r\nAPI failed: R_CMT_Reset \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function checks the status of Timer and sets period to it.
 * 
 * @param[in]   timer_input_type       Type of input: Period(ms) or Frequency(Hz)
 * @param[in]   timer_instance         Timer instance
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
static void cmt_config(e_timer_input_type timer_input_type, cmt_instance_ctrl_t *const timer_instance)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;
    /* Timer max period */
    uint32_t max_timer_period;
    /* Timer min period */
    uint32_t min_timer_period;
    /* Timer max frequency */
    uint32_t max_timer_frequency;
    /* Timer min frequency */
    uint32_t min_timer_frequency;

    switch (timer_input_type)
    {
        /* Input is: millisecond */
        case PERIOD_INPUT_TYPE:
        {
            /* Timer max period */
            max_timer_period = (uint32_t)max_period_calculate(timer_instance);

            /* Timer min period */
            /* Round up the value. Example: Timer period min = 0.3(ms) --> round up to 1(ms)*/
            min_timer_period = (uint32_t)round_up((min_period_calculate(timer_instance))); 

            /* Get timer period */
            while (true)
            {
                APP_PRINT("\r\nEnter the Timer period, range from %" PRIu32 " to %" PRIu32 " ms: ",
                          min_timer_period, max_timer_period);

                /* Get timer period */
                g_timer_period = get_input_int();

                /* Validate time period */
                if (max_timer_period < g_timer_period || g_timer_period < min_timer_period)
                {
                    APP_PRINT("\r\nInput out of range \r\n");
                }
                else
                {
                    break;
                }
            }
        }
        break;

        /* Input is: Hz */
        case FREQUENCY_INPUT_TYPE:
        {
            /* Max Timer frequency */
            max_timer_frequency = (uint32_t)max_frequency_calculate(timer_instance);

            /* Min Timer frequency */
            /* Round up the value. Example: Timer frequency min = 0.3(Hz) --> round up to 1(Hz)*/
            min_timer_frequency = (uint32_t)round_up((min_frequency_calculate(timer_instance)));

            /* Get timer frequency */
            while (true)
            {
                APP_PRINT("\r\nEnter the Timer frequency, range from %" PRIu32 " to %" PRIu32 " Hz: ",
                            min_timer_frequency, max_timer_frequency);

                /* Get timer frequency */
                g_timer_frequency = get_input_int();

                /* Validate time frequency */
                if ((max_timer_frequency < g_timer_frequency) || (g_timer_frequency < min_timer_frequency))
                {
                    APP_PRINT("\r\nInput out of range \r\n");
                }
                else
                {
                    break;
                }
            }
        }
        break;

        default:
        {
            return;
        }
        break;
    }

    /* Get info */
    err = R_CMT_InfoGet(timer_instance, &g_timer_info);

    /* Handle error */
    handle_error(err, "cmt_config", "API R_CMT_InfoGet failed");

    /* Get timer clock */
    g_clock_freq = g_timer_info.clock_frequency;

    /* Calculate raw count */
    switch (timer_input_type)
    {
        /* Input is: millisecond */
        case PERIOD_INPUT_TYPE:
        {
            /* Depending on the selected clock source, raw count values can be calculated based on
               the user-defined time period */
            /* raw count = ( input period(ms) * timer clock(Hz) / 1000) */
            g_raw_counts = (uint32_t)(((uint64_t)g_clock_freq * g_timer_period) / CONVERT_MILLISECOND_TO_SECOND);

            APP_PRINT("\r\nTimer period is set: %" PRIu32 " ms \r\n", (unsigned long)((uint32_t) g_timer_period));
        }
        break;

        /* Input is: Hz */
        case FREQUENCY_INPUT_TYPE:
        {
            /* Depending on the selected clock source, raw count values can be calculated based on 
               the user-defined frequency */
            /* raw count = ( timer clock(Hz) / input frequency(Hz) ) */
            g_raw_counts = (uint32_t)((uint64_t)g_clock_freq / g_timer_frequency);

            APP_PRINT("\r\nTimer frequency is set: %" PRIu32 " Hz \r\n", (unsigned long)((uint32_t) g_timer_frequency));
        }
        break;

        default:
        {
            return;
        }
        break;
    }

    /* Set period value */
    err = R_CMT_PeriodSet(timer_instance, g_raw_counts);

    /* Handle error */
    handle_error(err, "cmt_config", "API R_CMT_PeriodSet failed");
}

/*******************************************************************************************************************//**
 * @brief       This function starts the Timer.
 * 
 * @param[in]   timer_input_type      Type of input: Period(ms) or Frequency(Hz)
 * @param[in]   timer_mode            Timer mode: One-Shot or Periodic
 * @param[in]   timer_function        Timer function: Compare Match, Output Compare or Input Capture
 * @param[in]   timer_instance        Timer instance
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
static void cmt_start(e_timer_input_type timer_input_type, e_timer_mode timer_mode, e_timer_function timer_function,
                      cmt_instance_ctrl_t *const timer_instance)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Retrieve the status of timer */
    err = R_CMT_StatusGet(timer_instance, &g_timer_status);

    /* Handle error */
    handle_error(err, "cmt_start", "API R_CMT_StatusGet failed");

    /* Timer is stopped */
    if (TIMER_STATE_COUNTING != g_timer_status.state)
    {
        /* Start Timer-0 in One-Shot mode and operates together with Compare Match function */
        if ((timer_mode == ONE_SHOT_MODE) && (timer_function == COMPARE_MATCH_FUNCTION))
        {
/* Turn On LED  */
#if (IO_SUPPORT == 1)
            err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_ONE], LED_ON);
#else
            R_BSP_PinAccessEnable();
            R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_15_PIN_05, LED_ON); /* LED 1 */
            R_BSP_PinAccessDisable();
#endif
            /* Handle error */
            handle_error(err, "cmt_start", "API R_IOPORT_PinWrite failed");

            /* Print behavior of LED */
            if (timer_input_type == PERIOD_INPUT_TYPE)
            {
                APP_PRINT("\r\nLED %u will be turned on with period: %" PRIu32 " ms and then turned off \r\n",
                          LED_ONE,(unsigned long)((uint32_t) g_timer_period));
            }
            else
            {
                APP_PRINT("\r\nLED %u will be turned on with frequency: %" PRIu32 " Hz and then turned off \r\n",
                          LED_ONE,(unsigned long)((uint32_t) g_timer_frequency));
            }
        }

        /* Start Timer-1 in Periodic mode and operates together with Compare Match function */
        if ((timer_mode == PERIODIC_MODE) && (timer_function == COMPARE_MATCH_FUNCTION))
        {
            /* Print behavior of LED */
            if (timer_input_type == PERIOD_INPUT_TYPE)
            {
                APP_PRINT("\r\nLED %u will be toggled after: %" PRIu32 " ms \r\n", LED_TWO,(unsigned long)((uint32_t) g_timer_period));
            }
            else
            {
                APP_PRINT("\r\nLED %u will be toggled with frequency: %" PRIu32 " Hz \r\n", LED_TWO, (unsigned long)((uint32_t)g_timer_frequency));
            }
        }
    }
    /* Timer is running */
    else
    {
        /* Input is: millisecond */
        if (timer_input_type == PERIOD_INPUT_TYPE)
        {
            APP_PRINT("\r\nTimer is already running. Timer period updated to %" PRIu32 " ms \r\n",(unsigned long)((uint32_t) g_timer_period));
        }
        /* Input is: Hz */
        else
        {
            APP_PRINT("\r\nTimer is already running. Timer frequency updated to %" PRIu32 " Hz \r\n",(unsigned long)((uint32_t) g_timer_frequency));
        }
    }

    /* Start timer */
    err = R_CMT_Start(timer_instance);

    /* Handle error */
    handle_error(err, "cmt_start", "API R_CMT_Start failed");
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for One-Shot mode timer.
 * 
 * @param[in]   (timer_callback_args_t *)
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
void one_shot_timer_callback(timer_callback_args_t *p_args)
{
    fsp_err_t err = FSP_SUCCESS; /* Error status */

#if (IO_SUPPORT == 1)
    /* If this board has no LEDs then trap here */
    if (LED_COUNT_ZERO == g_bsp_leds.led_count)
    {
        APP_ERR_PRINT("\r\nError: No LEDs found on the board \r\n");
        return;
    }
#endif

    /* Check events that can trigger a callback function */
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
/* Turn OFF LED */
#if (IO_SUPPORT == 1)
        fsp_err_t err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_ONE], LED_OFF);
#else
        R_BSP_PinAccessEnable();
        R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_15_PIN_05, LED_OFF); /* LED 1 */
        R_BSP_PinAccessDisable();
#endif
        /* Handle error */
        handle_error(err, "one_shot_timer_callback", "API R_IOPORT_PinWrite failed");

        /* Set flag to indicate that the callback has completed */
        g_is_oneshot_callback_completed = true;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is callback for periodic mode timer.
 * 
 * @param[in]   (timer_callback_args_t *)
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
#if (IO_SUPPORT == 1)
    /* If this board has no LEDs then trap here */
    if (LED_COUNT_ZERO == g_bsp_leds.led_count)
    {
        APP_ERR_PRINT("\r\nError: No LEDs found on the board \r\n");
        return;
    }
#endif

    /* Toggle led */
    led_level ^= BSP_IO_LEVEL_HIGH;

    /* Check events that can trigger a callback function */
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
/* Toggle led */
#if (IO_SUPPORT == 1)
        fsp_err_t err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_TWO], led_level);
#else
        R_BSP_PinAccessEnable();
        R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_08_PIN_07, led_level);
        R_BSP_PinAccessDisable();
        fsp_err_t err = FSP_SUCCESS;
#endif
        /* Handle error */
        handle_error(err, "periodic_timer_callback,", "API R_IOPORT_PinWrite failed");
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function handles errors if they occur, closes all opened modules, prints the error, and traps it.
 * 
 *  @param[in]   err        Error code
 *  @param[in]   api_name   The API name to be checked.
 *  @param[in]   user_msg   User message
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char *api_name, const char *user_msg)
{
    if (FSP_SUCCESS != err)
    {
        APP_PRINT("\r\n[ERR] In Function: %s() \r\n%s \r\n", api_name, user_msg);

        if (g_cmt_initialized == true)
        {
            cmt_deinit();
        }

        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to turn off all leds on board.
 * 
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void turn_off_all_leds(void)
{
#if (IO_SUPPORT == 1)
    /* If this board has no LEDs then trap here */
    if (LED_COUNT_ZERO == g_bsp_leds.led_count)
    {
        APP_ERR_PRINT("\r\nError: No LEDs found on the board \r\n");
        return;
    }

    /* Turn off all leds */
    for (uint8_t i = 0; i < g_bsp_leds.led_count; i++)
    {
        /* Turn OFF LED */
        fsp_err_t err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[i], LED_OFF);

        /* Handle error */
        handle_error(err, "turn_off_all_leds", "API R_IOPORT_PinWrite failed");
    }
#else
    R_BSP_PinAccessEnable();

    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_06_PIN_04, LED_OFF);
    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_08_PIN_07, LED_OFF);
    R_BSP_PinWrite((bsp_io_port_pin_t)BSP_IO_PORT_15_PIN_05, LED_OFF);

    R_BSP_PinAccessDisable();
#endif
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to calculate the timer max period.
 * 
 *  @param[in]   timer_instance     Timer instance
 *  @param[out]  None
 *  @retval      timer_max_period   Max timer period (ms)
 **********************************************************************************************************************/
static float max_period_calculate(cmt_instance_ctrl_t *const timer_instance)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Timer max period */
    float timer_max_period;

    /* Get info */
    err = R_CMT_InfoGet(timer_instance, &g_timer_info);

    /* Handle error */
    handle_error(err, "max_period_calculate", "API R_CMT_InfoGet failed");

    /* Get Clock Frequency */
    g_clock_freq = g_timer_info.clock_frequency;

    /* Calculate timer max period (millisecond) */
    /* max period (ms) = MAX raw count / timer clock (Hz) * 1000 */
    timer_max_period = (((float)MAX_RAW_COUNT_16BIT * (float) (CONVERT_SECOND_TO_MILLISECOND)) /(float)(g_clock_freq));

    return timer_max_period;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to calculate the timer min period.
 * 
 *  @param[in]   timer_instance     Timer instance
 *  @param[out]  None
 *  @retval      timer_min_period   Min timer period (Millisecond)
 **********************************************************************************************************************/
static float min_period_calculate(cmt_instance_ctrl_t *const timer_instance)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Timer min period */
    float timer_min_period;

    /* Get info */
    err = R_CMT_InfoGet(timer_instance, &g_timer_info);

    /* Handle error */
    handle_error(err, "max_period_calculate", "API R_CMT_InfoGet failed");

    /* Get Clock Frequency */
    g_clock_freq = g_timer_info.clock_frequency;

    /* Calculate timer min period (millisecond) */
    /* min period (ms) = MIN raw count / timer clock (Hz) * 1000 */
    timer_min_period = ((float) (MIN_RAW_COUNT) / (float) (g_clock_freq)) * (float) (CONVERT_SECOND_TO_MILLISECOND);

    return timer_min_period;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to calculate the timer max frequency.
 * 
 *  @param[in]   timer_instance         Timer instance
 *  @param[out]  None
 *  @retval      timer_max_frequency    Max timer frequency (Hz)
 **********************************************************************************************************************/
static float max_frequency_calculate(cmt_instance_ctrl_t *const timer_instance)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Timer max frequency */
    float timer_max_frequency;

    /* Get info */
    err = R_CMT_InfoGet(timer_instance, &g_timer_info);

    /* Handle error */
    handle_error(err, "max_frequency_calculate", "API R_CMT_InfoGet failed");

    /* Get Clock Frequency */
    g_clock_freq = g_timer_info.clock_frequency;

    /* Calculate timer max frequency (Hz) */
    /* max frequency (Hz) = timer clock (Hz) / MIN raw count */
    timer_max_frequency = ((float)g_clock_freq / MIN_RAW_COUNT);

    return timer_max_frequency;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to calculate the timer min frequency.
 * 
 *  @param[in]   timer_instance         Timer instance
 *  @param[out]  None
 *  @retval      timer_min_frequency    Min timer frequency (Hz)
 **********************************************************************************************************************/
static float min_frequency_calculate(cmt_instance_ctrl_t *const timer_instance)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Timer min frequency */
    float timer_min_frequency;

    /* Get info */
    err = R_CMT_InfoGet(timer_instance, &g_timer_info);

    /* Handle error */
    handle_error(err, "min_frequency_calculate", "API R_CMT_InfoGet failed");

    /* Get Clock Frequency */
    g_clock_freq = g_timer_info.clock_frequency;

    /* Calculate timer min frequency (Hz) */
    /* min frequency (Hz) = timer clock (Hz) / MAX raw count */
    timer_min_frequency = ((float)g_clock_freq / MAX_RAW_COUNT_16BIT);

    return timer_min_frequency;
}

/*******************************************************************************************************************//**
 *  @brief      Get input string and convert to integer value.
 * 
 *  @param[in]  None
 *  @param[out] None
 *  @retval     integer value of input string.
 **********************************************************************************************************************/
static uint32_t get_input_int(void)
{
    /* Buffer to store input data from the user */
    char buf[BUF_SIZE] = {RESET_VALUE};

    /* Number of bytes read from input */
    uint32_t num_bytes = {RESET_VALUE};

    /* Final integer value parsed from input */
    uint32_t value = {RESET_VALUE};

    /* Loop until valid input data is received */
    while (RESET_VALUE == num_bytes)
    {
        /* Check if input data is available */
        if (APP_CHECK_DATA)
        {
            /* Cleaning buffer */
            memset(&buf[0], NULL_CHAR, BUF_SIZE);

            /* Read data into the buffer */
            num_bytes = APP_READ(buf);

            /* If no valid data, print error message */
            if (RESET_VALUE == num_bytes)
            {
                APP_PRINT("\r\nInvalid Input \r\n");
            }
        }
    }

    /* Conversion from input string to integer value */
    value = (uint32_t)(atoi((char *)buf));

    return value;
}

/*******************************************************************************************************************//**
 *  @brief      This function is used to get timer input type from user (input is Period(ms) or Frequency(Hz)).
 * 
 *  @param[in]  None
 *  @param[out] None
 *  @retval     None
 **********************************************************************************************************************/
static void get_timer_input_type()
{
    /* Read buffer */
    char rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};

    /* variable used to check the condition of while loop */
    bool valid_input = false;

    while (valid_input == false)
    {
        if (APP_CHECK_DATA)
        {
            /* Cleaning buffer */
            memset(&rByte[0], NULL_CHAR, BUFFER_SIZE_DOWN);

            /* Get input */
            APP_READ(rByte);

            /* Conversion from input string to integer value */
            g_user_option = (uint32_t)(atoi((char *)rByte));

            switch (g_user_option)
            {
                /* Input is: millisecond */
                case PERIOD_INPUT_TYPE:
                {
                    g_timer_input_type = PERIOD_INPUT_TYPE;
                    valid_input = true;
                    break;
                }
                /* Input is: Hz */
                case FREQUENCY_INPUT_TYPE:
                {
                    g_timer_input_type = FREQUENCY_INPUT_TYPE;
                    valid_input = true;
                    break;
                }
                default:
                {
                    APP_PRINT(INPUT_OPTION);
                    break;
                }
            }
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function is used to round up value.
 * 
 *  @param[in]  float_value     Float value needs to be rounded up
 *  @retval     round_value     The round up value
 **********************************************************************************************************************/
static float round_up(float float_value)
{
    uint32_t round_value = (uint32_t)float_value;
    
    if (float_value > round_value)
    {
        return (float) (round_value + 1);
    }
    else
    {
        return (float) round_value;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup cmt_ep)
 **********************************************************************************************************************/
