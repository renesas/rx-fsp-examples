/***********************************************************************************************************************
 * File Name    :  dsmif_ep.c
 * Description  :  Implements DSMIF functionality: simulates PDM input, configures filters, reads 16-bit output,
 *                 detects overcurrent, and displays results via Terminal Application.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @addtogroup DSMIF_EP
* @{
**********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "dsmif_ep.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
volatile bool g_dsmif_error_detection;

static dsmif_error_status_t g_dsmif_error_status;

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
static fsp_err_t dsmif_simulate_pdm_signal(void);
static fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent);
static fsp_err_t dsmif_init(void);
static fsp_err_t deinit_all_peripherals(void);
static void handle_error(fsp_err_t err, char *err_str);
static void setting_filter(void);
uint32_t process_input_data(void);
extern bsp_leds_t g_bsp_leds;

/***********************************************************************************************************************
*  Function Name: dsmif_hal_entry
*  Description  : This function is used to start DSMIF example operation.
*  Arguments    : None
*  Return Value : None
**********************************************************************************************************************/
void dsmif_hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };
    char rByte[BUF_SIZE] = { INITIAL_VALUE };
    uint32_t read_data = RESET_VALUE;
    uint16_t conversion_data = RESET_VALUE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

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
    /* Example Project information printed on Terminal Application */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major,
            version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);
    APP_PRINT(MENU);

    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                                    (bsp_io_level_t) BSP_IO_LEVEL_LOW);


    while (true)
    {
        if (APP_CHECK_DATA)
        {
            /* Read First byte of data provided by user */
            APP_READ(rByte);
            /* Conversion from input string to integer value */
            read_data = (uint32_t) (atoi((char*) rByte));

            /* Handle user selection from menu */
            switch (read_data)
            {
                case SET_DUTY_CYCLE:
                {
                    /* Initialize and generate PWM to simulate a PDM signal */
                    err = dsmif_simulate_pdm_signal();
                    handle_error(err, "\r\ndsmif_simulate_pdm_signal API failed");
                    break;
                }
                case SET_FILTER_AND_DECIMATION:
                {
                    /* Configure filter and decimation settings */
                    setting_filter();
                    break;
                }
                case START_MEASUREMENT:
                {
                    /* Initializes the module */
                    err = dsmif_init();
                    handle_error(err, "\r\ndsmif_init API failed");
                    /* Wait for filter settling time. */
                    R_BSP_SoftwareDelay(DSMIF_MAXIMUM_FILTER_SETTLING_TIME_US, BSP_DELAY_UNITS_MICROSECONDS);

                    /* Enable error detection after waiting for the filter to stabilize. */
                    err = R_DSMIF_ErrorDetectionEnable(&g_dsmif_ctrl);
                    handle_error(err, "\r\nR_DSMIF_ErrorDetectionEnable API failed");

                    /* Read DSMIF conversion result */
                    err = R_DSMIF_Read(&g_dsmif_ctrl, (adc_channel_t) (ADC_CHANNEL_OVERCURRENT_MASK | ADC_CHANNEL_0),
                                        &conversion_data);
                    handle_error(err, "\r\nR_DSMIF_Read API failed");

                    APP_PRINT("\r\nDSMIF Result: Converted 16-bit value = %d\r\n", conversion_data);
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
        /* Check if overcurrent error is detected */
        if (true == g_dsmif_error_detection)
        {
            APP_PRINT("\r\n[ERROR] Overcurrent limit exceeded. Shutting down system.\r\n");
            /* Shut down peripherals and exit loop */
            err = deinit_all_peripherals();
            break;
        }
    }
}

/***********************************************************************************************************************
* End of function dsmif_hal_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: dsmif_simulate_pdm_signal
*  Description  : This function is used to simulate a PDM input signal using GPT to generate a
*                 PWM with a user-defined duty cycle.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t dsmif_simulate_pdm_signal(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t gpt_desired_duty_cycle_percent = RESET_VALUE;

    /* Prompt user for duty cycle percentage (0–100%) */
    APP_PRINT("\r\nEnter The Desired Duty Cycle in Percentage of Range 0 - 100: ");
    gpt_desired_duty_cycle_percent = process_input_data();

    /* Validate Duty cycle percentage */
    if (GPT_MAX_PERCENT < gpt_desired_duty_cycle_percent)
    {
        APP_ERR_PRINT("\r\n ** INVALID INPUT, DESIRED DUTY CYCLE IS OUT OF RANGE. ** \r\n");
    }
    else
    {
        /* If GPT timer is not opened yet, initialize and start it */
        if (MODULE_CLOSE == g_timer0_ctrl.open)
        {
            err = R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);
            APP_ERR_RETURN(err, "\r\n** R_GPT_Open API failed ** \r\n");

            /* Start the Timer. */
            err = R_GPT_Start(&g_timer0_ctrl);
            if (FSP_SUCCESS != err)
            {
                /* Cleanup if start fails */
                deinit_all_peripherals();
                APP_ERR_RETURN(err, "\r\n** R_GPT_Start API failed ** \r\n");
            }
            APP_PRINT("\r\nGPT started successfully with Duty Cycle: %d\r\n", gpt_desired_duty_cycle_percent);
        }
        else
        {
            /* Print Duty cycle on Terminal Application, when Timer already started */
            APP_PRINT("PWM Timer Already started, Update Duty Cycle to: %d\r\n", gpt_desired_duty_cycle_percent);
        }
        /* Set new PWM duty cycle */
        err = set_timer_duty_cycle((uint8_t)(gpt_desired_duty_cycle_percent));
        APP_ERR_RETURN(err, "\r\n** set_timer_duty_cycle API failed ** \r\n");
    }
    return FSP_SUCCESS;
}

/***********************************************************************************************************************
* End of function dsmif_simulate_pdm_signal
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: set_timer_duty_cycle
*  Description  : This function is used to set Duty Cycle of PWM timer.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
***********************************************************************************************************************/
static fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t duty_cycle_counts = RESET_VALUE;
    uint32_t current_period_counts = RESET_VALUE;
    timer_info_t info = { (timer_direction_t) RESET_VALUE, RESET_VALUE, RESET_VALUE };

    /* Get the current period setting. */
    err = R_GPT_InfoGet(&g_timer0_ctrl, &info);

    /* Handle error */
    APP_ERR_RETURN(err, "\r\n** R_GPT_InfoGet API failed ** \r\n");

    /* Update period counts locally. */
    current_period_counts = info.period_counts;

    /* Calculate the desired duty cycle based on the current period. Note that if the period could be larger than
     * UINT32_MAX / 100, this calculation could overflow. A cast to uint64_t is used to prevent this. The cast is
     * not required for 16-bit timers. */
    duty_cycle_counts = (uint32_t) ((uint64_t) (current_period_counts * duty_cycle_percent) / GPT_MAX_PERCENT);

    err = R_GPT_DutyCycleSet(&g_timer0_ctrl, duty_cycle_counts, TIMER_PIN);
    /* Handle error */
    APP_ERR_RETURN(err, "\r\n** R_GPT_DutyCycleSet API failed ** \r\n");

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
***********************************************************************************************************************/
uint32_t process_input_data(void)
{
    char buf[BUF_SIZE] ={ INITIAL_VALUE };
    uint32_t num_bytes = RESET_VALUE;
    uint32_t value = RESET_VALUE;

    /* Wait until user inputs valid data from Terminal Application */
    while (RESET_VALUE == num_bytes)
    {
        if (APP_CHECK_DATA) /* Check if data is available */
        {
            /* Read input into buffer */
            num_bytes = APP_READ(buf);
            if (RESET_VALUE == num_bytes)
            {
                /* Notify user of invalid input */
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
*  Function Name: setting_filter
*  Description  : This function is used to configures the DSMIF overcurrent detection filter based on user selection.
*  Arguments    : None
*  Return Value : None
***********************************************************************************************************************/
static void setting_filter(void)
{
    /* Print the filter setting menu for user input */
    APP_PRINT(FILTER_SETTING);

    /* Get user selection from Terminal Application */
    uint32_t filter_setting = process_input_data();
    APP_PRINT("\r\nFilter option %d has been selected.\n", filter_setting);

    /* Apply filter configuration based on selected option */
    switch (filter_setting)
    {
        case SINC1_64:
        {
            /* Set Sinc and Decimation for feature Overcurrent Detection */
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.filter_order = DSMIF_FILTER_ORDER_1ST;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.data_shift = DSMIF_DATA_SHIFT_5_0;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.decimation_count = 64 - 1;
            break;
        }
        case SINC1_256:
        {
            /* Set Sinc and Decimation for feature Overcurrent Detection */
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.filter_order = DSMIF_FILTER_ORDER_1ST;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.data_shift = DSMIF_DATA_SHIFT_7_0;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.decimation_count = 256 - 1;
            break;
        }
        case SINC2_64:
        {

            /* Set Sinc and Decimation for feature Overcurrent Detection */
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.filter_order = DSMIF_FILTER_ORDER_2ND;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.data_shift = DSMIF_DATA_SHIFT_11_0;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.decimation_count = 64 - 1;
            break;
        }
        case SINC2_256:
        {
            /* Set Sinc and Decimation for feature Overcurrent Detection */
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.filter_order = DSMIF_FILTER_ORDER_2ND;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.data_shift = DSMIF_DATA_SHIFT_15_0;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.decimation_count = 256 - 1;
            break;
        }
        case SINC3_64:
        {
            /* Set Sinc and Decimation for feature Overcurrent Detection */
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.filter_order = DSMIF_FILTER_ORDER_3RD;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.data_shift = DSMIF_DATA_SHIFT_17_2;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.decimation_count = 64 - 1;
            break;
        }
        case SINC3_256:
        {
            /* Set Sinc and Decimation for feature Overcurrent Detection */
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.filter_order = DSMIF_FILTER_ORDER_3RD;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.data_shift = DSMIF_DATA_SHIFT_23_8;
            g_dsmif_channel0.error_detection_cfg.overcurrent_filter_cfg.decimation_count = 256 - 1;
            break;
        }
        default:
        {
            APP_PRINT("\r\nInvalid Menu Option Selected\r\n");
            break;
        }
    }
}

/***********************************************************************************************************************
* End of function setting_filter
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: dsmif_init
*  Description  : This function is used to initializes and starts the DSMIF conversion.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
***********************************************************************************************************************/
static fsp_err_t dsmif_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the DSMIF module */
    err = R_DSMIF_Open(&g_dsmif_ctrl, &g_dsmif_cfg);

    /* Handle error upon failure */
    APP_ERR_RETURN(err, "\r\n** R_DSMIF_Open API failed ** \r\n");

    /* Start DSMIF conversion (scan) */
    err = R_DSMIF_ScanStart(&g_dsmif_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Cleanup if start fails */
        deinit_all_peripherals();
        APP_ERR_RETURN(err, "\r\n** R_DSMIF_ScanStart API failed ** \r\n");
    }
    return FSP_SUCCESS;
}

/***********************************************************************************************************************
* End of function dsmif_init
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: deinit_all_peripherals
*  Description  : This function is used to de-initializes GPT and DSMIF peripherals if they are currently open.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
***********************************************************************************************************************/
static fsp_err_t deinit_all_peripherals(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the GPT module if already open */
    if (MODULE_CLOSE != g_timer0_ctrl.open)
    {
        err = R_GPT_Close(&g_timer0_ctrl);
        /* Handle error upon failure */
        if (FSP_SUCCESS != err)
        {
            /* Print failure message */
            APP_ERR_PRINT("\r\nR_GPT_Close API failed");
        }
    }

    /* Close the DSMIF module if already open */
    if (MODULE_CLOSE != g_dsmif_ctrl.open)
    {
        err = R_DSMIF_Close(&g_dsmif_ctrl);
        /* Handle error upon failure */
        if (FSP_SUCCESS != err)
        {
            /* Print failure message */
            APP_ERR_PRINT("\r\nR_DSMIF_Close API failed");
        }
    }
    return err;
}

/***********************************************************************************************************************
* End of function deinit_all_peripherals
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: handle_error
*  Description  : This function is used to closes all opened modules, prints and traps the error.
*  Arguments    : err         error value
*                 p_timer_ctl timer instance controller
*                 err_str     error string
*  Return Value : None
***********************************************************************************************************************/
static void handle_error(fsp_err_t err, char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);
        /* De-initializes module peripherals */
        deinit_all_peripherals();
        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}

/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: dsmif_callback
*  Description  : This function is used to callback for DSMIF ADC conversion error event.
*  Arguments    : p_args        Pointer to callback argument
*  Return Value : None
***********************************************************************************************************************/
void dsmif_callback(adc_callback_args_t *p_args)
{
    if (ADC_EVENT_CONVERSION_ERROR == p_args->event)
    {
        g_dsmif_error_detection = true;

        /* Read the error status in order to clear the error flags. */
        R_DSMIF_ErrorStatusGet(&g_dsmif_ctrl, &g_dsmif_error_status);
    }
}

/***********************************************************************************************************************
* End of function dsmif_callback
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @} (end addtogroup DSMIF_EP)
***********************************************************************************************************************/
