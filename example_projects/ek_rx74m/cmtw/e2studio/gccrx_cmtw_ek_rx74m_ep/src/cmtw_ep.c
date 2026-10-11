/*******************************************************************************************************************//**
 * File Name    : cmtw_ep.c
 * Description  : This file implements the CMTW functionality for the example project. It handles CMTW in
 *                One-Shot mode and Periodic mode
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Include Section
 **********************************************************************************************************************/
#include "common_utils.h"
#include "cmtw_ep.h"
#include <inttypes.h>

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
/* External variables */
extern bsp_leds_t g_bsp_leds;                                                       /* LED configuration from BSP */

/* Timer control flags */
static bool g_cmtw_initialized                       = DISABLE;                      /* CMTW initialization status */
static bool g_gpt_initialized                        = DISABLE;                      /* GPT initialization status */
static volatile bool g_is_oneshot_callback_completed = DISABLE;                      /* One-Shot callback completion flag */
static volatile bool g_is_start_measurement          = DISABLE;                      /* Measurement start flag */

/* Timer configuration */
static uint8_t g_current_demo_case                   = INITIAL_VALUE;                /* Current demo case selection */
static e_timer_input_type g_timer_input_type         = TIMER_INPUT_TYPE_MILLISECOND; /* Timer input type selection */
static uint32_t g_clock_freq                         = INITIAL_VALUE;                /* Last configured timer clock freq */
static uint64_t g_raw_counts                         = INITIAL_VALUE;                /* Timer raw counter value */
static uint64_t g_expected_timer_period_ms           = INITIAL_VALUE;                /* Expected timer period in ms */
static uint64_t g_expected_timer_frequency_hz        = INITIAL_VALUE;                /* Expected timer frequency in Hz */

/* LED control */
static volatile bsp_io_level_t g_led_level           = BSP_IO_LEVEL_HIGH;            /* LED level for toggling */

/* Capture buffer */
static volatile capture_buffer_t g_capture_buffer    = {INITIAL_VALUE};              /* Capture data buffer */

/* Timeout tracking */
static uint64_t g_timer_timeout                      = INITIAL_VALUE;                /* Timeout counter for One-Shot mode */

/* Buffer for string conversion */
static char g_timer_buffer[BUF_SIZE]                 = {INITIAL_VALUE};              /* String buffer for data conversion */

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
/* Generic timer functions */
static void timer_init(e_timer_type timer_type);
static void timer_deinit(e_timer_type timer_type);
static void timer_config(e_timer_type timer_type, e_timer_size timer_size, void *timer_instance);
static void timer_start(e_timer_type timer_type, void *timer_instance);
static void timer_enable(e_timer_type timer_type, void *timer_instance);
static void timer_reset(e_timer_type timer_type);

/* Calculate min/max functions */
static uint32_t calculate_max_period(uint32_t clock_freq, uint64_t max_raw_count);
static uint32_t calculate_min_period(uint32_t clock_freq, uint64_t min_raw_count);
static uint32_t calculate_max_frequency(uint32_t clock_freq, uint64_t min_raw_count);
static uint32_t calculate_min_frequency(uint32_t clock_freq, uint64_t max_raw_count);

/* User input functions */
static uint64_t get_timer_period_input(uint32_t min_period, uint32_t max_period);
static uint64_t get_timer_frequency_input(uint32_t min_frequency, uint32_t max_frequency);
static bool get_timer_input_type(void);
static uint32_t get_input(void);

/* Case execution functions */
static void one_shot_compare_match(void);
static void periodic_compare_match(void);
static void periodic_input_capture(void);
static void periodic_output_compare(void);

/* Helper functions */
static void handle_error(fsp_err_t err, const char *api_name, const char *user_msg);
static void turn_off_all_leds(void);
static uint32_t round_up(double float_value);

/*******************************************************************************************************************//**
 * @addtogroup cmtw_ep
 * @{
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Definition Section
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 *  @brief       This function is used to initialize the related modules and start the CMTW example operation
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
void cmtw_ep_entry(void)
{
    fsp_pack_version_t version      = {RESET_VALUE};    /* FSP version information */
    char rByte[TERM_BUFFER_SIZE]    = {RESET_VALUE};    /* User input read buffer */

    /* Turn OFF error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_LOW);

#if (USE_VIRTUAL_COM == 1)
    fsp_err_t err = FSP_SUCCESS;
    err= TERM_INIT();

    if (FSP_SUCCESS != err)
    {
        /* Turn ON error LED to indicate uart_init failed */
         R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t) BSP_IO_LEVEL_HIGH);

         /* Error trap */
         ERROR_TRAP;
    }

#endif /* USE_VIRTUAL_COM */

    /* Fetch and display FSP version */
    R_FSP_VersionGet(&version);
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);
    APP_PRINT(CMTW_INFO);
    APP_PRINT(EP_MENU);

    while (true)
    {
        if (APP_CHECK_DATA)
        {
            /* Read user selection */
            memset(&rByte[0], NULL_CHAR, TERM_BUFFER_SIZE);
            APP_READ(rByte);
            g_current_demo_case = (uint8_t)(atoi((char *)rByte));

            /* Cleanup before running demo */
            turn_off_all_leds();
            timer_deinit(TIMER_TYPE_CMTW);
            timer_deinit(TIMER_TYPE_GPT);

            /* Execute selected demo case */
            switch (g_current_demo_case)
            {
                case ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION:
                {
                    one_shot_compare_match();
                }
                break;

                case PERIODIC_MODE_COMPARE_MATCH_FUNCTION:
                {
                    periodic_compare_match();
                }
                break;

                case PERIODIC_MODE_INPUT_CAPTURE_FUNCTION:
                {
                    periodic_input_capture();
                }
                break;

                case PERIODIC_MODE_OUTPUT_COMPARE_FUNCTION:
                {
                    periodic_output_compare();
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
 *  @brief       This function is used to initialize timer module (CMTW or GPT)
 *  @param[in]   timer_type    Timer type (CMTW or GPT)
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void timer_init(e_timer_type timer_type)
{
    fsp_err_t err           = FSP_SUCCESS;              /* Error status */

    if (timer_type == TIMER_TYPE_CMTW)
    {
        if (g_cmtw_initialized == ENABLE)
        {
            return;
        }

        /* Initialize based on current demo case */
        switch (g_current_demo_case)
        {
            case ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION:
            {
                if (MODULE_CLOSE == g_timer_cmtw_one_shot_ctrl.open)
                {
                    err = R_CMTW_Open(&g_timer_cmtw_one_shot_ctrl, &g_timer_cmtw_one_shot_cfg);
                    handle_error(err, "timer_init", "R_CMTW_Open failed for One-Shot mode");
                }
            }
            break;

            case PERIODIC_MODE_COMPARE_MATCH_FUNCTION:
            {
                if (MODULE_CLOSE == g_timer_cmtw_periodic_ctrl.open)
                {
                    err = R_CMTW_Open(&g_timer_cmtw_periodic_ctrl, &g_timer_cmtw_periodic_cfg);
                    handle_error(err, "timer_init", "R_CMTW_Open failed for Periodic mode");
                }
            }
            break;

            case PERIODIC_MODE_INPUT_CAPTURE_FUNCTION:
            {
                if (MODULE_CLOSE == g_timer_cmtw_periodic_input_capture_ctrl.open)
                {
                    err = R_CMTW_Open(&g_timer_cmtw_periodic_input_capture_ctrl,
                                      &g_timer_cmtw_periodic_input_capture_cfg);
                    handle_error(err, "timer_init", "R_CMTW_Open failed for Input Capture");
                }
            }
            break;

            case PERIODIC_MODE_OUTPUT_COMPARE_FUNCTION:
            {
                if (MODULE_CLOSE == g_timer_cmtw_output_generate_ctrl.open)
                {
                    err = R_CMTW_Open(&g_timer_cmtw_output_generate_ctrl,
                                      &g_timer_cmtw_output_generate_cfg);
                    handle_error(err, "timer_init", "R_CMTW_Open failed for Output Compare");
                }
            }
            break;

            default:
            {
                /* Do nothing */
            }
            break;
        }
        g_cmtw_initialized = ENABLE;
    }
    else /* TIMER_TYPE_GPT */
    {
        if (g_gpt_initialized == ENABLE)
        {
            return;
        }

        /* Initialize GPT based on current demo case */
        if (g_current_demo_case == PERIODIC_MODE_INPUT_CAPTURE_FUNCTION)
        {
            if (MODULE_CLOSE == g_timer_gpt_output_generate_ctrl.open)
            {
                err = R_GPT_Open(&g_timer_gpt_output_generate_ctrl, &g_timer_gpt_output_generate_cfg);
                handle_error(err, "timer_init", "R_GPT_Open failed for Output Generate");
            }
        }
        else if (g_current_demo_case == PERIODIC_MODE_OUTPUT_COMPARE_FUNCTION)
        {
            if (MODULE_CLOSE == g_timer_gpt_periodic_input_capture_ctrl.open)
            {
                err = R_GPT_Open(&g_timer_gpt_periodic_input_capture_ctrl,
                                 &g_timer_gpt_periodic_input_capture_cfg);
                handle_error(err, "timer_init", "R_GPT_Open failed for Input Capture");
            }
        }
        else
        {
            /* Do nothing */
        }
        g_gpt_initialized = ENABLE;
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to deinitialize timer module (CMTW or GPT)
 *  @param[in]   timer_type    Timer type (CMTW or GPT)
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void timer_deinit(e_timer_type timer_type)
{
    fsp_err_t err           = FSP_SUCCESS;              /* Error status */

    if (timer_type == TIMER_TYPE_CMTW)
    {
        if (g_cmtw_initialized == DISABLE)
        {
            return;
        }

        g_cmtw_initialized = DISABLE;

        /* Close all CMTW instances */
        if (MODULE_CLOSE != g_timer_cmtw_one_shot_ctrl.open)
        {
            err = R_CMTW_Close(&g_timer_cmtw_one_shot_ctrl);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("R_CMTW_Close failed for One-Shot timer\r\n");
            }
        }

        if (MODULE_CLOSE != g_timer_cmtw_periodic_ctrl.open)
        {
            err = R_CMTW_Close(&g_timer_cmtw_periodic_ctrl);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("R_CMTW_Close failed for Periodic timer\r\n");
            }
        }

        if (MODULE_CLOSE != g_timer_cmtw_periodic_input_capture_ctrl.open)
        {
            err = R_CMTW_Close(&g_timer_cmtw_periodic_input_capture_ctrl);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("R_CMTW_Close failed for Input Capture timer\r\n");
            }
        }

        if (MODULE_CLOSE != g_timer_cmtw_output_generate_ctrl.open)
        {
            err = R_CMTW_Close(&g_timer_cmtw_output_generate_ctrl);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("R_CMTW_Close failed for Output Compare timer\r\n");
            }
        }
    }
    else /* TIMER_TYPE_GPT */
    {
        if (g_gpt_initialized == DISABLE)
        {
            return;
        }

        g_gpt_initialized = DISABLE;

        /* Close all GPT instances */
        if (MODULE_CLOSE != g_timer_gpt_output_generate_ctrl.open)
        {
            err = R_GPT_Close(&g_timer_gpt_output_generate_ctrl);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("R_GPT_Close failed for Output Generate timer\r\n");
            }
        }

        if (MODULE_CLOSE != g_timer_gpt_periodic_input_capture_ctrl.open)
        {
            err = R_GPT_Close(&g_timer_gpt_periodic_input_capture_ctrl);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("R_GPT_Close failed for Input Capture timer\r\n");
            }
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to configure timer with user input
 *  @param[in]   timer_type       Timer type (CMTW or GPT)
 *  @param[in]   timer_size       Timer size (16-bit or 32-bit)
 *  @param[in]   timer_instance   Pointer to timer instance control structure
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void timer_config(e_timer_type timer_type, e_timer_size timer_size, void *timer_instance)
{
    fsp_err_t err               = FSP_SUCCESS;          /* Error status */
    uint32_t clock_freq         = INITIAL_VALUE;        /* Timer clock frequency */
    uint32_t max_period         = INITIAL_VALUE;        /* Maximum timer period */
    uint32_t min_period         = INITIAL_VALUE;        /* Minimum timer period */
    uint32_t max_frequency      = INITIAL_VALUE;        /* Maximum timer frequency */
    uint32_t min_frequency      = INITIAL_VALUE;        /* Minimum timer frequency */
    uint64_t max_raw_count      = INITIAL_VALUE;        /* Maximum raw count value */

    /* Timer information structure */
    timer_info_t timer_info     = {(timer_direction_t)INITIAL_VALUE,
                                    INITIAL_VALUE,
                                    INITIAL_VALUE};

    /* Determine max raw count based on timer size */
    max_raw_count = (timer_size == TIMER_32_BIT) ? MAX_RAW_COUNT_32BIT : MAX_RAW_COUNT_16BIT;

    /* Get timer information */
    if (timer_type == TIMER_TYPE_CMTW)
    {
        err = R_CMTW_InfoGet((cmtw_instance_ctrl_t *)timer_instance, &timer_info);
        handle_error(err, "timer_config", "R_CMTW_InfoGet failed");
    }
    else /* TIMER_TYPE_GPT */
    {
        err = R_GPT_InfoGet((gpt_instance_ctrl_t *)timer_instance, &timer_info);
        handle_error(err, "timer_config", "R_GPT_InfoGet failed");
    }

    clock_freq = timer_info.clock_frequency;
    g_clock_freq = clock_freq;

    /* Calculate min/max and get user input */
    if (g_timer_input_type == TIMER_INPUT_TYPE_MILLISECOND)
    {
        max_period = calculate_max_period(clock_freq, max_raw_count);
        min_period = calculate_min_period(clock_freq, MIN_RAW_COUNT);
        g_expected_timer_period_ms = get_timer_period_input(min_period, max_period);

        /* Calculate raw counts: raw_count = (clock_freq * period_ms) / 1000 */
        g_raw_counts = ((uint64_t)clock_freq * g_expected_timer_period_ms) / CONVERT_MILLISECOND_TO_SECOND;

        APP_PRINT("\r\nExpected Timer period is set: %" PRIu32 " ms \r\n", (unsigned long)(uint32_t)g_expected_timer_period_ms);
    }
    else /* TIMER_INPUT_TYPE_FREQUENCY */
    {
        max_frequency = calculate_max_frequency(clock_freq, MIN_RAW_COUNT);
        min_frequency = calculate_min_frequency(clock_freq, max_raw_count);
        g_expected_timer_frequency_hz = get_timer_frequency_input(min_frequency, max_frequency);

        /* Calculate raw counts: raw_count = clock_freq / frequency */
        if ((g_current_demo_case == ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION) ||
            (g_current_demo_case == PERIODIC_MODE_COMPARE_MATCH_FUNCTION))
        {
            g_raw_counts = ((uint64_t)clock_freq / g_expected_timer_frequency_hz) / 2;

            if (g_raw_counts < MIN_RAW_COUNT)
            {
                g_raw_counts = MIN_RAW_COUNT;
            }
        }
        else
        {
            g_raw_counts = (uint64_t)clock_freq / g_expected_timer_frequency_hz;

            if (g_raw_counts < MIN_RAW_COUNT)
            {
                g_raw_counts = MIN_RAW_COUNT;
            }
        }

        APP_PRINT("\r\nExpected Timer frequency is set: %" PRIu32 " Hz \r\n", (unsigned long)(uint32_t)g_expected_timer_frequency_hz);
    }

    /* Set period */
    if (timer_type == TIMER_TYPE_CMTW)
    {
        err = R_CMTW_PeriodSet((cmtw_instance_ctrl_t *)timer_instance, (uint32_t)g_raw_counts);
        handle_error(err, "timer_config", "R_CMTW_PeriodSet failed");
    }
    else /* TIMER_TYPE_GPT */
    {
        err = R_GPT_PeriodSet((gpt_instance_ctrl_t *)timer_instance, (uint32_t)g_raw_counts);
        handle_error(err, "timer_config", "R_GPT_PeriodSet failed");
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to start timer
 *  @param[in]   timer_type       Timer type (CMTW or GPT)
 *  @param[in]   timer_instance   Pointer to timer instance control structure
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void timer_start(e_timer_type timer_type, void *timer_instance)
{
    fsp_err_t err                   = FSP_SUCCESS;      /* Error status */
    timer_status_t timer_status     = {INITIAL_VALUE};  /* Timer status structure */
    float expected_pulse_width      = INITIAL_VALUE;    /* Expected pulse width value */

    /* Get timer status */
    if (timer_type == TIMER_TYPE_CMTW)
    {
        err = R_CMTW_StatusGet((cmtw_instance_ctrl_t *)timer_instance, &timer_status);
        handle_error(err, "timer_start", "R_CMTW_StatusGet failed");

        /* Print LED behavior information before starting */
        if (timer_status.state != TIMER_STATE_COUNTING)
        {
            if (g_current_demo_case == ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION)
            {
                /* Turn on LED */
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[BSP_LED_LED1], LED_ON);
                handle_error(err, "timer_start", "R_IOPORT_PinWrite failed");

                if (g_timer_input_type == TIMER_INPUT_TYPE_MILLISECOND)
                {
                                        APP_PRINT("\r\nLED %u will be turned on and automatically turned off after %" PRIu32 " ms \r\n",
                              LED_ONE, (unsigned long)(uint32_t)g_expected_timer_period_ms);
                    APP_PRINT("\r\nPlease wait until the timer expires...\n");
                }
                else
                {
                                        APP_PRINT("\r\nWith frequency: %" PRIu32 " Hz, LED %u will be turned on and automatically "
                              "turned off after %" PRIu32 " ms \r\n", (unsigned long)(uint32_t)g_expected_timer_frequency_hz, LED_ONE,
                              (unsigned long)(uint32_t)((g_raw_counts * CONVERT_MILLISECOND_TO_SECOND) / g_clock_freq));
                    APP_PRINT("\r\nPlease wait until the timer expires...\n");
                }
            }
            else if (g_current_demo_case == PERIODIC_MODE_COMPARE_MATCH_FUNCTION)
            {
                if (g_timer_input_type == TIMER_INPUT_TYPE_MILLISECOND)
                {
                                        APP_PRINT("\r\nLED %u will be toggled after: %" PRIu32 " ms \r\n", LED_TWO,
                              (unsigned long)(uint32_t)g_expected_timer_period_ms);
                }
                else
                {
                                        APP_PRINT("\r\nWith frequency: %" PRIu32 " Hz, LED %u will be toggled after: %" PRIu32 " ms \r\n",
                              (unsigned long)(uint32_t)g_expected_timer_frequency_hz, LED_TWO,
                              (unsigned long)(uint32_t)((g_raw_counts * CONVERT_MILLISECOND_TO_SECOND) / g_clock_freq));
                }
            }
            else if (g_current_demo_case == PERIODIC_MODE_OUTPUT_COMPARE_FUNCTION)
            {
                /* Enable output pin */
                err = R_CMTW_OutputEnable((cmtw_instance_ctrl_t *)timer_instance, CMTW_IO_PIN_TOC1);
                handle_error(err, "timer_start", "R_CMTW_OutputEnable failed");

                expected_pulse_width = (float)(g_raw_counts / 2) * (float)CONVERT_MILLISECOND_TO_SECOND / (float)g_clock_freq;
                snprintf(g_timer_buffer, sizeof(g_timer_buffer), "%lu", (unsigned long)(uint32_t)expected_pulse_width);

                if (g_timer_input_type == TIMER_INPUT_TYPE_MILLISECOND)
                {
                                        APP_PRINT("\r\n(CMTW) Waveform output is generated in TOC1 pin with: \r\n    Period     = %" PRIu32
                              " ms \r\n    Duty cycle = 50 %% \r\n", (unsigned long)(uint32_t)g_expected_timer_period_ms);
                }
                else
                {
                                        APP_PRINT("\r\nWith frequency: %" PRIu32 " Hz, (CMTW) Waveform output is generated in TOC1 pin with: "
                              "\r\n    Period     = %" PRIu32 " ms \r\n    Duty cycle = 50 %% \r\n",
                              (unsigned long)(uint32_t)g_expected_timer_frequency_hz,
                              (unsigned long)(uint32_t)((g_raw_counts * CONVERT_MILLISECOND_TO_SECOND) / g_clock_freq));
                }

                APP_PRINT("\r\n(GPT) Waveform input is captured in GTIOC1A pin \r\n");
                APP_PRINT("\r\nLED %u will be toggled after: %s ms \r\n", LED_THREE, g_timer_buffer);
            }
            else
            {
                /* Do nothing */
            }
        }

        /* Start CMTW timer */
        err = R_CMTW_Start((cmtw_instance_ctrl_t *)timer_instance);
        handle_error(err, "timer_start", "R_CMTW_Start failed");
    }
    else /* TIMER_TYPE_GPT */
    {
        /* Print waveform information before starting (Case 3: Input Capture) */
        if (g_current_demo_case == PERIODIC_MODE_INPUT_CAPTURE_FUNCTION)
        {
            expected_pulse_width = (float)(g_raw_counts / 2) * (float)CONVERT_MILLISECOND_TO_SECOND / (float)g_clock_freq;
            snprintf(g_timer_buffer, sizeof(g_timer_buffer), "%lu", (unsigned long)(uint32_t)expected_pulse_width);

            if (g_timer_input_type == TIMER_INPUT_TYPE_MILLISECOND)
            {
                                APP_PRINT("\r\n(GPT) Waveform output is generated in GTIOC1B pin with: \r\n    Period     = %" PRIu32
                          " ms \r\n    Duty cycle = 50 %% \r\n", (unsigned long)(uint32_t)g_expected_timer_period_ms);
            }
            else
            {
                                APP_PRINT("\r\nWith frequency: %" PRIu32 " Hz, (GPT) Waveform output is generated in GTIOC1B pin with: "
                          "\r\n    Period     = %" PRIu32 " ms \r\n    Duty cycle = 50 %% \r\n",
                          (unsigned long)(uint32_t)g_expected_timer_frequency_hz,
                          (unsigned long)(uint32_t)((g_raw_counts * CONVERT_MILLISECOND_TO_SECOND) / g_clock_freq));
            }

            APP_PRINT("\r\n(CMTW) Waveform input is captured in TIC1 pin \r\n");
            APP_PRINT("\r\nLED %u will be toggled after: %s ms \r\n", LED_THREE, g_timer_buffer);
        }

        /* Start GPT timer */
        err = R_GPT_Start((gpt_instance_ctrl_t *)timer_instance);
        handle_error(err, "timer_start", "R_GPT_Start failed");
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to enable timer external triggers
 *  @param[in]   timer_type       Timer type (CMTW or GPT)
 *  @param[in]   timer_instance   Pointer to timer instance control structure
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void timer_enable(e_timer_type timer_type, void *timer_instance)
{
    fsp_err_t err           = FSP_SUCCESS;              /* Error status */

    if (timer_type == TIMER_TYPE_CMTW)
    {
        err = R_CMTW_Enable((cmtw_instance_ctrl_t *)timer_instance);
        handle_error(err, "timer_enable", "R_CMTW_Enable failed");
    }
    else /* TIMER_TYPE_GPT */
    {
        err = R_GPT_Enable((gpt_instance_ctrl_t *)timer_instance);
        handle_error(err, "timer_enable", "R_GPT_Enable failed");
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to reset timer counter
 *  @param[in]   timer_type    Timer type (CMTW or GPT)
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void timer_reset(e_timer_type timer_type)
{
    fsp_err_t err           = FSP_SUCCESS;              /* Error status */

    if (timer_type == TIMER_TYPE_CMTW)
    {
        if (MODULE_CLOSE != g_timer_cmtw_one_shot_ctrl.open)
        {
            err = R_CMTW_Reset(&g_timer_cmtw_one_shot_ctrl);
            handle_error(err, "timer_reset", "R_CMTW_Reset failed for One-Shot timer");
        }

        if (MODULE_CLOSE != g_timer_cmtw_periodic_ctrl.open)
        {
            err = R_CMTW_Reset(&g_timer_cmtw_periodic_ctrl);
            handle_error(err, "timer_reset", "R_CMTW_Reset failed for Periodic timer");
        }

        if (MODULE_CLOSE != g_timer_cmtw_periodic_input_capture_ctrl.open)
        {
            err = R_CMTW_Reset(&g_timer_cmtw_periodic_input_capture_ctrl);
            handle_error(err, "timer_reset", "R_CMTW_Reset failed for Input Capture timer");
        }

        if (MODULE_CLOSE != g_timer_cmtw_output_generate_ctrl.open)
        {
            err = R_CMTW_Reset(&g_timer_cmtw_output_generate_ctrl);
            handle_error(err, "timer_reset", "R_CMTW_Reset failed for Output Compare timer");
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to calculate maximum timer period
 *  @param[in]   clock_freq      Timer clock frequency in Hz
 *  @param[in]   max_raw_count   Maximum raw count value
 *  @param[out]  None
 *  @retval      Maximum period in milliseconds
 **********************************************************************************************************************/
static uint32_t calculate_max_period(uint32_t clock_freq, uint64_t max_raw_count)
{
    return (uint32_t)(((double)max_raw_count / (double) clock_freq) * CONVERT_SECOND_TO_MILLISECOND);
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to calculate minimum timer period
 *  @param[in]   clock_freq      Timer clock frequency in Hz
 *  @param[in]   min_raw_count   Minimum raw count value
 *  @param[out]  None
 *  @retval      Minimum period in milliseconds
 **********************************************************************************************************************/
static uint32_t calculate_min_period(uint32_t clock_freq, uint64_t min_raw_count)
{
    return round_up(((double) min_raw_count / (double) clock_freq) * CONVERT_SECOND_TO_MILLISECOND);
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to calculate maximum timer frequency
 *  @param[in]   clock_freq      Timer clock frequency in Hz
 *  @param[in]   min_raw_count   Minimum raw count value
 *  @param[out]  None
 *  @retval      Maximum frequency in Hz
 **********************************************************************************************************************/
static uint32_t calculate_max_frequency(uint32_t clock_freq, uint64_t min_raw_count)
{
    return (uint32_t)(clock_freq / min_raw_count);
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to calculate minimum timer frequency
 *  @param[in]   clock_freq      Timer clock frequency in Hz
 *  @param[in]   max_raw_count   Maximum raw count value
 *  @param[out]  None
 *  @retval      Minimum frequency in Hz
 **********************************************************************************************************************/
static uint32_t calculate_min_frequency(uint32_t clock_freq, uint64_t max_raw_count)
{
    return (uint32_t)((clock_freq + max_raw_count - 1ULL) / max_raw_count);
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to get period input from user with validation
 *  @param[in]   min_period    Minimum valid period
 *  @param[in]   max_period    Maximum valid period
 *  @param[out]  None
 *  @retval      User input period value
 **********************************************************************************************************************/
static uint64_t get_timer_period_input(uint32_t min_period, uint32_t max_period)
{
    uint64_t period                 = INITIAL_VALUE;                       /* User input period value */
    uint32_t recommended_period_max = PERIOD_REC_MAX_MS;                   /* Recommended maximum period */
    uint32_t recommended_period_min;                                       /* Recommended minimum period */
    bool is_compare_match_function;                                        /* Flag for Compare Match function */

    /* Determine recommended values and mode based on demo case */
    is_compare_match_function = ((g_current_demo_case == ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION) ||
                                 (g_current_demo_case == PERIODIC_MODE_COMPARE_MATCH_FUNCTION));

    recommended_period_min = is_compare_match_function ? PERIOD_REC_MIN_COMPARE_MS : PERIOD_REC_MIN_CAPTURE_MS;

    while (true)
    {
        /* Display input */
        APP_PRINT("\r\nInput type is: millisecond");
        APP_PRINT("\r\n    Timer period range (ms): %" PRIu32 " to %" PRIu32 " (min - max value)",
                  min_period, max_period);
        APP_PRINT("\r\n    Recommended value  (ms): %" PRIu32 " to %" PRIu32,
                  recommended_period_min, recommended_period_max);

        display_input:
        APP_PRINT("\r\nEnter the expected Timer period (%" PRIu32 " to %" PRIu32 "): \r\n> ",
                  recommended_period_min, recommended_period_max);

        period = get_input();

        /* Validate input range */
        if ((period >= min_period) && (period <= max_period))
        {
            /* Check if period is outside recommended threshold */
            if ((period > PERIOD_REC_MAX_MS) ||
                (!is_compare_match_function && (period < PERIOD_REC_MIN_CAPTURE_MS)))
            {
                if (is_compare_match_function)
                {
                    APP_PRINT("\r\n[WARNING]: Period is out of the recommended threshold, LED may not be visible \n");

                    goto display_input;
                }
                else
                {
                    APP_PRINT("\r\n[WARNING]: Period is out of the recommended threshold, LED may not be visible \n"
                              "           and affects the display of actual pulse width measurement logs"
                              " on Terminal Application \n");

                    goto display_input;
                }
            }
            break;
        }

        APP_PRINT("\r\nInput out of range \r\n");
        goto display_input;
    }

    return period;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to get frequency input from user with validation
 *  @param[in]   min_frequency    Minimum valid frequency
 *  @param[in]   max_frequency    Maximum valid frequency
 *  @param[out]  None
 *  @retval      User input frequency value
 **********************************************************************************************************************/
static uint64_t get_timer_frequency_input(uint32_t min_frequency, uint32_t max_frequency)
{
    uint64_t frequency = INITIAL_VALUE;                             /* User input frequency value */
    uint32_t recommended_frequency_min = FREQ_REC_MIN_HZ;           /* Recommended minimum frequency */
    uint32_t recommended_frequency_max = FREQ_REC_MAX_HZ;           /* Recommended maximum frequency */
    bool is_compare_match_function;                                 /* Flag for Compare Match mode */

    /* Determine mode based on demo case */
    is_compare_match_function = ((g_current_demo_case == ONE_SHOT_MODE_COMPARE_MATCH_FUNCTION) ||
                                 (g_current_demo_case == PERIODIC_MODE_COMPARE_MATCH_FUNCTION));

    while (true)
    {
        /* Display input */
        APP_PRINT("\r\nInput type is: Hz");
        APP_PRINT("\r\n    Timer frequency range (Hz): %" PRIu32 " to %" PRIu32 " (min - max value)",
                  min_frequency, max_frequency);
        APP_PRINT("\r\n    Recommended value     (Hz): %" PRIu32 " to %" PRIu32,
                  recommended_frequency_min, recommended_frequency_max);

        display_input:
        APP_PRINT("\r\nEnter the expected Timer frequency (%" PRIu32 " to %" PRIu32 "): \r\n> ",
                  recommended_frequency_min, recommended_frequency_max);

        frequency = get_input();

        /* Validate input range */
        if ((frequency >= min_frequency) && (frequency <= max_frequency))
        {
            /* Check if frequency is outside recommended threshold */
            if ((frequency > FREQ_REC_MAX_HZ) || (frequency < FREQ_REC_MIN_HZ))
            {
                if (is_compare_match_function)
                {
                    APP_PRINT("\r\n[WARNING]: Frequency is out of the recommended threshold, LED may not be visible \n");

                    goto display_input;
                }
                else
                {
                    APP_PRINT("\r\n[WARNING]: Frequency is out of the recommended threshold, LED may not be visible \n"
                              "           and affects the display of actual pulse width measurement logs"
                              " on Terminal Application \n");

                    goto display_input;
                }
            }
            break;
        }

        APP_PRINT("\r\nInput out of range \r\n");
        goto display_input;
    }

    return frequency;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to get timer input type from user
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      true  - User selected valid input type (continue with demo)
 *               false - User wants to go back to Main Menu
 **********************************************************************************************************************/
static bool get_timer_input_type(void)
{
    char rByte[TERM_BUFFER_SIZE] = {RESET_VALUE};            /* User input read buffer */
    uint8_t user_option     = INITIAL_VALUE;            /* User option selection */

    while (true)
    {
        if (APP_CHECK_DATA)
        {
            memset(&rByte[0], NULL_CHAR, TERM_BUFFER_SIZE);
            APP_READ(rByte);
            user_option = (uint8_t)(atoi((char *)rByte));

            if (user_option == TIMER_INPUT_TYPE_MILLISECOND)
            {
                g_timer_input_type = TIMER_INPUT_TYPE_MILLISECOND;
                return true;
            }
            else if (user_option == TIMER_INPUT_TYPE_FREQUENCY)
            {
                g_timer_input_type = TIMER_INPUT_TYPE_FREQUENCY;
                return true;
            }
            else if (user_option == TIMER_INPUT_TYPE_GO_BACK)
            {
                return false;
            }
            else
            {
                APP_PRINT(INPUT_OPTION);
            }
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to get integer input from user
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      Integer value from user input
 **********************************************************************************************************************/
static uint32_t get_input(void)
{
    char buf[BUF_SIZE]  = {RESET_VALUE};                /* Input buffer */
    uint32_t num_bytes  = INITIAL_VALUE;                /* Number of bytes read */

    while (num_bytes == INITIAL_VALUE)
    {
        if (APP_CHECK_DATA)
        {
            memset(&buf[0], NULL_CHAR, BUF_SIZE);
            num_bytes = APP_READ(buf);

            if (num_bytes == INITIAL_VALUE)
            {
                APP_PRINT("\r\nInvalid Input \r\n");
            }
        }
    }

    return (uint32_t)(atoi((char *)buf));
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to run Case 1: One-Shot mode with Compare Match. CMTW timer is
 *               configured in One-Shot mode with user-defined period/frequency. LED1 turns on when timer
 *               starts and automatically turns off when timer expires.
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void one_shot_compare_match(void)
{
    APP_PRINT("Run the EP in One-Shot mode operating together with the Compare Match function\r\n\n");
    APP_PRINT(INPUT_OPTION);

    /* ========== STEP 1: Get user input ========== */
    /* Get timer input type: millisecond or frequency */
    if (get_timer_input_type() == false)
    {
        /* User wants to go back to Main Menu */
        APP_PRINT(EP_MENU);
        return;
    }

    /* ========== STEP 2: CMTW timer setup ========== */
    /* Initialize CMTW timer module */
    timer_init(TIMER_TYPE_CMTW);

    /* Configure CMTW with user-defined period/frequency */
    timer_config(TIMER_TYPE_CMTW, TIMER_32_BIT, &g_timer_cmtw_one_shot_ctrl);

    /* Start CMTW timer (LED1 turns on, timer starts counting) */
    timer_start(TIMER_TYPE_CMTW, &g_timer_cmtw_one_shot_ctrl);

    /* Reset CMTW counter to ensure clean start */
    timer_reset(TIMER_TYPE_CMTW);

    /* ========== STEP 3: Wait for timer completion ========== */
    /* Initialize timeout counter for safety */
    g_timer_timeout = TIMEOUT;

    /* Wait for One-Shot timer callback to complete */
    while (g_is_oneshot_callback_completed == DISABLE)
    {
        --g_timer_timeout;

        /* Check for timeout to prevent infinite loop */
        if (g_timer_timeout == INITIAL_VALUE)
        {
            APP_ERR_PRINT("\r\nCallback event not received during One-Shot operation \r\n");
            break;
        }
    }

    /* ========== STEP 4: Verify completion and cleanup ========== */
    /* Check if timer completed successfully */
    if (g_is_oneshot_callback_completed == ENABLE)
    {
        /* Reset flag for next operation */
        g_is_oneshot_callback_completed = DISABLE;
        APP_PRINT("\r\nTimer expired in One-Shot mode \r\n");
    }

    /* Display menu for next operation */
    APP_PRINT(EP_MENU);
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to run Case 2: Periodic mode with Compare Match. CMTW timer is
 *               configured in Periodic mode with user-defined period/frequency. LED2 toggles continuously
 *               at each timer period until user selects another demo.
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void periodic_compare_match(void)
{
    APP_PRINT("Run the EP in Periodic mode operating together with the Compare Match function\r\n\n");
    APP_PRINT(INPUT_OPTION);

    /* ========== STEP 1: Get user input ========== */
    /* Get timer input type: millisecond or frequency */
    if (get_timer_input_type() == false)
    {
        /* User wants to go back to Main Menu */
        APP_PRINT(EP_MENU);
        return;
    }

    /* ========== STEP 2: CMTW timer setup ========== */
    /* Initialize CMTW timer module */
    timer_init(TIMER_TYPE_CMTW);

    /* Configure CMTW with user-defined period/frequency */
    timer_config(TIMER_TYPE_CMTW, TIMER_32_BIT, &g_timer_cmtw_periodic_ctrl);

    /* Start CMTW timer (LED2 toggles at each period) */
    timer_start(TIMER_TYPE_CMTW, &g_timer_cmtw_periodic_ctrl);

    /* Reset CMTW counter to ensure clean start */
    timer_reset(TIMER_TYPE_CMTW);

    /* ========== STEP 3: Continuous operation ========== */
    /* Timer runs continuously and toggles LED2 via callback */
    /* User can select another demo from menu to stop */
    APP_PRINT(EP_MENU);
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to run Case 3: Periodic mode with Input Capture. GPT generates
 *               waveform on GTIOC1A pin with user-defined period/frequency. CMTW captures the waveform on
 *               TIC1 pin and measures actual pulse width. LED3 toggles on each capture event.
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void periodic_input_capture(void)
{
    fsp_err_t err                       = FSP_SUCCESS;      /* Error status */
    capture_buffer_t local_buf          = {INITIAL_VALUE};  /* Local buffer for captured data */
    float measured_pulse_width          = INITIAL_VALUE;    /* Measured pulse width in milliseconds */
    uint32_t cmtw_clock_freq            = INITIAL_VALUE;    /* CMTW clock frequency */

    /* CMTW timer information */
    timer_info_t cmtw_timer_info        = {(timer_direction_t)INITIAL_VALUE,
                                            INITIAL_VALUE,
                                            INITIAL_VALUE};

    APP_PRINT("Run the EP with Periodic mode operates together with Input Capture function\r\n\n");
    APP_PRINT(INPUT_OPTION);

    /* ========== STEP 1: Initialize and get user input ========== */
    /* Reset capture buffer for clean measurement */
    g_capture_buffer.event_count = INITIAL_VALUE;

    /* Get timer input type: millisecond or frequency */
    if (get_timer_input_type() == false)
    {
        /* User wants to go back to Main Menu */
        APP_PRINT(EP_MENU);
        return;
    }

    /* ========== STEP 2: CMTW timer setup (Capture device) ========== */
    /* Initialize CMTW timer module */
    timer_init(TIMER_TYPE_CMTW);

    /* Get CMTW timer information (clock freq needed for pulse width calculation) */
    err = R_CMTW_InfoGet(&g_timer_cmtw_periodic_input_capture_ctrl, &cmtw_timer_info);
    handle_error(err, "periodic_input_capture", "R_CMTW_InfoGet failed");
    cmtw_clock_freq = cmtw_timer_info.clock_frequency;

    /* ========== STEP 3: GPT timer setup (Output device) ========== */
    /* Initialize GPT timer module */
    timer_init(TIMER_TYPE_GPT);

    /* Configure GPT with user-defined period/frequency to generate waveform */
    timer_config(TIMER_TYPE_GPT, TIMER_32_BIT, &g_timer_gpt_output_generate_ctrl);

    /* ========== STEP 4: Start both timers ========== */
    /* Enable CMTW capture trigger (ready to capture incoming signal from GPT) */
    timer_enable(TIMER_TYPE_CMTW, &g_timer_cmtw_periodic_input_capture_ctrl);

    /* Start GPT timer (generates waveform on GTIOC1A pin, captured by CMTW TIC1) */
    timer_start(TIMER_TYPE_GPT, &g_timer_gpt_output_generate_ctrl);

    APP_PRINT("\r\n******* Start measuring the actual pulse width captured on the TIC1 pin ******* \r\n");

    /* ========== STEP 5: Measurement loop ========== */
    /* Measure pulse width until reaching threshold (7 captures) */
    while (g_capture_buffer.event_count < CAPTURE_EVENT_COUNT_THRESHOLD)
    {
        /* Wait for capture event from ISR callback */
        if (g_is_start_measurement == ENABLE)
        {
            g_is_start_measurement = DISABLE;

            /* Check if new capture data is ready */
            if (g_capture_buffer.buffer_ready == ENABLE)
            {
                /* Copy data from volatile buffer to local buffer */
                local_buf = g_capture_buffer;
                g_capture_buffer.buffer_ready = DISABLE;

                /* Calculate measured pulse width using CMTW clock frequency */
                measured_pulse_width = ((float)local_buf.capture_count / (float)cmtw_clock_freq) *
                                       CONVERT_SECOND_TO_MILLISECOND;

                /* Convert to string for display */
                snprintf(g_timer_buffer, sizeof(g_timer_buffer), "%lu", (unsigned long)(uint32_t)measured_pulse_width);

                /* Skip first 2 readings (unstable), then display measurement */
                if (g_capture_buffer.event_count >= SKIP_FIRST_READING_THRESHOLD)
                {
                    APP_PRINT("\r\nPulse width measurement value = %s ms \r\n", g_timer_buffer);
                }
            }
        }
    }

    /* ========== STEP 6: Cleanup ========== */
    APP_PRINT("\r\n******* End measuring the actual pulse width captured on the TIC1 pin ******* \r\n\n");

    /* Reset CMTW timer for next operation */
    timer_reset(TIMER_TYPE_CMTW);

    /* Display menu for next operation */
    APP_PRINT(EP_MENU);
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to run Case 4: Periodic mode with Output Compare. CMTW generates
 *               waveform on TOC1 pin with user-defined period/frequency. GPT captures the waveform on
 *               GTIOC1A pin and measures actual pulse width. LED3 toggles on each capture event.
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void periodic_output_compare(void)
{
    fsp_err_t err                       = FSP_SUCCESS;      /* Error status */
    uint64_t total_capture_count        = INITIAL_VALUE;    /* Total capture count including overflows */
    float measured_pulse_width          = INITIAL_VALUE;    /* Measured pulse width in milliseconds */

    /* GPT timer information */
    timer_info_t gpt_timer_info         = {(timer_direction_t)INITIAL_VALUE,
                                            INITIAL_VALUE,
                                            INITIAL_VALUE};

    APP_PRINT("Run the EP with Periodic mode operates together with Output Compare function\r\n\n");
    APP_PRINT(INPUT_OPTION);

    /* ========== STEP 1: Initialize and get user input ========== */
    /* Reset capture buffer for clean measurement */
    g_capture_buffer.event_count    = INITIAL_VALUE;
    g_capture_buffer.overflow_count = INITIAL_VALUE;
    g_capture_buffer.capture_count  = INITIAL_VALUE;

    /* Get timer input type: millisecond or frequency */
    if (get_timer_input_type() == false)
    {
        /* User wants to go back to Main Menu */
        APP_PRINT(EP_MENU);
        return;
    }

    /* ========== STEP 2: CMTW timer setup (Output device) ========== */
    /* Initialize CMTW timer module */
    timer_init(TIMER_TYPE_CMTW);

    /* Configure CMTW with user-defined period/frequency to generate waveform */
    timer_config(TIMER_TYPE_CMTW, TIMER_32_BIT, &g_timer_cmtw_output_generate_ctrl);

    /* Start CMTW timer (generates waveform on TOC1 pin, captured by GPT GTIOC1A) */
    timer_start(TIMER_TYPE_CMTW, &g_timer_cmtw_output_generate_ctrl);

    /* ========== STEP 3: GPT timer setup (Capture device) ========== */
    /* Initialize GPT timer module */
    timer_init(TIMER_TYPE_GPT);

    /* Enable GPT capture trigger (ready to capture incoming signal from CMTW) */
    timer_enable(TIMER_TYPE_GPT, &g_timer_gpt_periodic_input_capture_ctrl);

    APP_PRINT("\r\n******* Start measuring the actual pulse width captured on the GTIOC1A pin ******* \r\n");

    /* ========== STEP 4: Measurement loop ========== */
    /* Measure pulse width until reaching threshold (7 captures) */
    while (g_capture_buffer.event_count < CAPTURE_EVENT_COUNT_THRESHOLD)
    {
        /* Wait for capture event from ISR callback */
        if (g_is_start_measurement == ENABLE)
        {
            g_is_start_measurement = DISABLE;

            /* Get GPT timer information (clock freq and period for calculation) */
            err = R_GPT_InfoGet(&g_timer_gpt_periodic_input_capture_ctrl, &gpt_timer_info);
            handle_error(err, "periodic_output_compare", "R_GPT_InfoGet failed");

            /* Calculate total count including overflow: (period × overflow_count) + capture_count */
            total_capture_count = (gpt_timer_info.period_counts * g_capture_buffer.overflow_count) +
                                  g_capture_buffer.capture_count;

            /* Calculate measured pulse width using GPT clock frequency */
            measured_pulse_width = ((float)total_capture_count / (float)gpt_timer_info.clock_frequency) *
                                   CONVERT_SECOND_TO_MILLISECOND;

            /* Reset capture counters for next measurement */
            g_capture_buffer.capture_count  = INITIAL_VALUE;
            g_capture_buffer.overflow_count = INITIAL_VALUE;

            /* Convert to string for display */
            snprintf(g_timer_buffer, sizeof(g_timer_buffer), "%lu", (unsigned long)(uint32_t)measured_pulse_width);

            /* Skip first 2 readings (unstable), then display measurement */
            if (g_capture_buffer.event_count >= SKIP_FIRST_READING_THRESHOLD)
            {
                APP_PRINT("\r\nPulse width measurement value = %s ms \r\n", g_timer_buffer);
            }
        }
    }

    /* ========== STEP 5: Cleanup ========== */
    APP_PRINT("\r\n******* End measuring the actual pulse width captured on the GTIOC1A pin ******* \r\n\n");

    /* Reset CMTW timer for next operation */
    timer_reset(TIMER_TYPE_CMTW);

    /* Display menu for next operation */
    APP_PRINT(EP_MENU);
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to handle errors
 *  @param[in]   err          Error code
 *  @param[in]   api_name     API name
 *  @param[in]   user_msg     User message
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char *api_name, const char *user_msg)
{
    if (err != FSP_SUCCESS)
    {
        APP_PRINT("\r\n[ERR] In Function: %s() \r\n%s \r\n", api_name, user_msg);

        if (g_cmtw_initialized == ENABLE)
        {
            timer_deinit(TIMER_TYPE_CMTW);
        }

        if (g_gpt_initialized == ENABLE)
        {
            timer_deinit(TIMER_TYPE_GPT);
        }

        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to turn off all LEDs
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void turn_off_all_leds(void)
{
    fsp_err_t err           = FSP_SUCCESS;              /* Error status */
    uint8_t led_index       = INITIAL_VALUE;            /* LED index counter */

    if (g_bsp_leds.led_count == LED_COUNT_ZERO)
    {
        APP_ERR_PRINT("\r\nError: No LEDs found on the board \r\n");
        return;
    }

    for (led_index = INITIAL_VALUE; led_index < g_bsp_leds.led_count; led_index++)
    {
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[led_index], LED_OFF);
        handle_error(err, "turn_off_all_leds", "R_IOPORT_PinWrite failed");
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to round up float value
 *  @param[in]   float_value    Float value to round up
 *  @param[out]  None
 *  @retval      Rounded up value
 **********************************************************************************************************************/
static uint32_t round_up(double float_value)
{
    uint32_t round_value = (uint32_t) float_value;    /* Integer part of float value */

    if (float_value > (double) round_value)
    {
        return (round_value + 1U);
    }
    return round_value;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used as One-Shot timer callback
 *  @param[in]   p_args    Callback arguments
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
void one_shot_timer_callback(timer_callback_args_t *p_args)
{
    fsp_err_t err           = FSP_SUCCESS;              /* Error status */

    if (g_bsp_leds.led_count == LED_COUNT_ZERO)
    {
        APP_ERR_PRINT("\r\nError: No LEDs found on the board \r\n");
        return;
    }

    if (p_args->event == TIMER_EVENT_CYCLE_END)
    {
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[BSP_LED_LED1], LED_OFF);
        handle_error(err, "one_shot_timer_callback", "R_IOPORT_PinWrite failed");
        g_is_oneshot_callback_completed = ENABLE;
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used as Periodic timer callback
 *  @param[in]   p_args    Callback arguments
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
    fsp_err_t err           = FSP_SUCCESS;              /* Error status */
    g_led_level             ^= BSP_IO_LEVEL_HIGH;       /* LED state */

    if (g_bsp_leds.led_count == LED_COUNT_ZERO)
    {
        APP_ERR_PRINT("\r\nError: No LEDs found on the board \r\n");
        return;
    }

    /* Case 2: Periodic Compare Match */
    if ((p_args->event == TIMER_EVENT_CYCLE_END) && (g_current_demo_case == PERIODIC_MODE_COMPARE_MATCH_FUNCTION))
    {
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[BSP_LED_LED2], g_led_level);
        handle_error(err, "periodic_timer_callback", "R_IOPORT_PinWrite failed");
    }

    /* Case 3: CMTW Input Capture */
    if (p_args->event == TIMER_EVENT_CAPTURE_B)
    {
        g_capture_buffer.capture_count = p_args->capture;
        g_capture_buffer.event_count++;
        g_capture_buffer.buffer_ready  = ENABLE;
        g_is_start_measurement         = ENABLE;

        err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[BSP_LED_LED3], g_led_level);
        handle_error(err, "periodic_timer_callback", "R_IOPORT_PinWrite failed");
    }

    /* Case 4: GPT Input Capture */
    if (p_args->event == TIMER_EVENT_CAPTURE_A)
    {
        g_capture_buffer.capture_count = p_args->capture;
        g_capture_buffer.event_count++;
        g_is_start_measurement         = ENABLE;

        err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[BSP_LED_LED3], g_led_level);
        handle_error(err, "periodic_timer_callback", "R_IOPORT_PinWrite failed");
    }

    /* Case 4: GPT Overflow */
    if ((p_args->event == TIMER_EVENT_CYCLE_END) && (g_current_demo_case == PERIODIC_MODE_OUTPUT_COMPARE_FUNCTION))
    {
        g_capture_buffer.overflow_count++;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup cmtw_ep)
 **********************************************************************************************************************/
