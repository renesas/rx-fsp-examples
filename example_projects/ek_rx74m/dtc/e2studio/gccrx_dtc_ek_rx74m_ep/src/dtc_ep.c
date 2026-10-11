/***********************************************************************************************************************
 * File Name    :  dtc_ep.c
 * Description  :  This file implements the DTC functionality for the example project. It handles DTC transfer modes,
 *                 start timer, and error handling
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @addtogroup DTC_EP
* @{
**********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "dtc_ep.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
/* Flag indicating DTC transfer completion */
volatile bool g_dtc_transfer_complete = false;
static dtc_extended_cfg_t g_transfer_cfg_extend;
uint8_t g_converted_rtt_input = RESET_VALUE;

/* LED1 port pin control register for port */
static volatile uint32_t *p_ioport_pdr_register_led1 = &R_PORT15->PCNTR1;

/* LED2 port pin control register for port */
static volatile uint32_t *p_ioport_pdr_register_led2 = &R_PORT8->PCNTR1;

/* Config DTC for chain mode */
static transfer_cfg_t g_chain_transfer_cfg;
static transfer_info_t g_chain_transfer_info[] DTC_TRANSFER_INFO_ALIGNMENT =
{ [ZERO] =
{ .transfer_settings_u.transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_u.transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .transfer_settings_u.transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_u.transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_EACH,
  .transfer_settings_u.transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_u.transfer_settings_word_b.size = TRANSFER_SIZE_4_BYTE,
  .transfer_settings_u.transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = ZERO,
  .length = MAX_LENGTH, },

  [ONE] =
  { .transfer_settings_u.transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
    .transfer_settings_u.transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
    .transfer_settings_u.transfer_settings_word_b.irq = TRANSFER_IRQ_END,
    .transfer_settings_u.transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
    .transfer_settings_u.transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
    .transfer_settings_u.transfer_settings_word_b.size = TRANSFER_SIZE_4_BYTE,
    .transfer_settings_u.transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
    .p_dest = (void*) NULL,
    .p_src = (void const*) NULL,
    .num_blocks = ZERO,
    .length = MAX_LENGTH, },
};

/* LED Port data array for LED 1 */
uint32_t g_source_data_led1[MAX_LENGTH] =
{
  0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,
  0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,
  0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,
  0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,
  0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,
  0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020,0x00200020,0x00000020
};
/* LED Port data array for LED 2 */
uint32_t g_source_data_led2[MAX_LENGTH] =
{
  0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,
  0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,
  0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,
  0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,
  0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,
  0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,0x00800080,0x00000080,
};

uint32_t g_src_buffer[DATA_LENGTH] __attribute__((aligned(4))) =
{
  'K','F','X','M','A','Z','U','N','P','D',
  'T','E','R','Y','B','Q','S','O','G','L',
  'H','C','J','V','W','I','A','N','Z','B',
  'X','K','M','D','E','R','T','Q','U','S',
  'Y','F','O','L','G','J','C','V','W','I',
  'P','X','A','N','M','Z','B','H','D','K'
};

uint32_t g_dst_buffer[DATA_LENGTH]__attribute__((aligned(4)));

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
static fsp_err_t gpt_timer_init(void);
static fsp_err_t dtc_transfer_init(dtc_instance_ctrl_t  * p_transfer_ctrl,
        transfer_cfg_t const * const p_transfer_cfg);
static fsp_err_t normal_mode(void);
static fsp_err_t repeat_mode(void);
static fsp_err_t block_mode(void);
static fsp_err_t chain_mode(void);
static fsp_err_t dtc_deinit(dtc_instance_ctrl_t * const p_ctrl);
void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        void const * volatile p_src,\
                                    void const * volatile p_dest );
static void set_chain_transfer_dst_src_address(void);
static void timer_deinit(void);
static void handle_error(fsp_err_t err,
        dtc_instance_ctrl_t  *const p_ctrl,  char * err_str);

/* Include variable use BSP LED */
extern bsp_leds_t g_bsp_leds;
/***********************************************************************************************************************
*  Function Name: dtc_hal_entry
*  Description  : This function is used to start DTC example operation.
*  Arguments    : None
*  Return Value : None
**********************************************************************************************************************/
void dtc_hal_entry(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t err = FSP_SUCCESS;
    /* FSP variable to store major/minor versions */
    fsp_pack_version_t version = { RESET_VALUE };
    char rByte[SIZE_64] = { RESET_VALUE };

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
    /* Fetch FSP version to display on console */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on Virtual Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor,
            version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(EP_INFO);
    APP_PRINT(MENU);
    while (true)
    {
        if (APP_CHECK_DATA)
        {

            APP_READ(rByte);
            /* User input selection */
            g_converted_rtt_input = (uint8_t) atoi(rByte);
            switch (g_converted_rtt_input)
            {
                case NORMAL:
                {
                    err = normal_mode();

                    /* Handle error */
                    handle_error(err, &g_transfer_normal_ctrl,
                            "\r\n** Normal Mode transfer failed **\r\n");
                    dtc_deinit(&g_transfer_normal_ctrl);
                    timer_deinit();

                    APP_PRINT("\r\nEnter any key (other than 1,2,3 or 4) "
                            "to go back to the menu\r\n");
                    break;
                }
                case REPEAT:
                {
                    err = repeat_mode();

                    /* Handle error */
                    handle_error(err, &g_transfer_repeat_ctrl,
                            "\r\n** Repeat Mode transfer failed **\r\n");
                    dtc_deinit(&g_transfer_repeat_ctrl);
                    timer_deinit();

                    APP_PRINT("\r\n\nEnter any key (other than 1,2,3 or 4) "
                            "to go back to the menu\r\n");
                    break;
                }
                case BLOCK:
                {
                    err = block_mode();
                    /* Handle error */
                    handle_error(err, &g_transfer_block_ctrl,
                            "\r\n** Repeat Mode transfer failed **\r\n");
                    dtc_deinit(&g_transfer_block_ctrl);
                    timer_deinit();
                    APP_PRINT("\r\n\nEnter any key "
                            "(other than 1,2,3 or 4) to go back to the menu\r\n");
                    break;
                }
                case CHAIN:
                {
                    err = chain_mode();
                    /* Handle error */
                    handle_error(err, &g_transfer_normal_ctrl,
                            "\r\n** Chain Mode transfer failed **\r\n");
                    dtc_deinit(&g_transfer_normal_ctrl);
                    timer_deinit();
                    APP_PRINT("\r\nEnter any key (other than 1,2,3 or 4) "
                            "to go back to the menu\r\n");
                    break;
                }
                default:
                {
                    APP_PRINT(MENU);
                    break;
                }
            }
        }
    }
}
/***********************************************************************************************************************
* End of function dtc_hal_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: gpt_timer_init
*  Description  : This function is used to initialization GPT timer
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t gpt_timer_init(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t err = FSP_SUCCESS;

    /* Open the GPT timer */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);

    /* Handle error upon failure */
    APP_ERR_RETURN(err, "\r\n** R_GPT_Open API failed ** \r\n");

    /* Start GPT timer only if mode is not REPEAT,
     * because REPEAT mode manages timer start inside the loop */
    if (g_converted_rtt_input != REPEAT)
    {
        /* Start GPT timer */
        err = R_GPT_Start(&g_timer_ctrl);

        /* Handle error upon failure */
        if (FSP_SUCCESS != err)
        {
            timer_deinit(); /* Attempt to close GPT */
            APP_ERR_RETURN(err, "\r\n** R_GPT_Start API failed ** \r\n");
        }
    }
    return err;
}
/***********************************************************************************************************************
* End of function gpt_timer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: dtc_transfer_init
*  Description  : This function is used to DTC transfer initialization
*  Arguments    : p_transfer_ctrl
*                 p_transfer_cfg
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t dtc_transfer_init(dtc_instance_ctrl_t *p_transfer_ctrl,
        transfer_cfg_t const *const p_transfer_cfg)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t err = FSP_SUCCESS;

    /* Open the DTC module */
    err = R_DTC_Open(p_transfer_ctrl, p_transfer_cfg);

    /* Handle error upon failure */
    APP_ERR_RETURN(err, "\r\n** R_DTC_Open API failed ** \r\n");

    /* Enable DTC transfer */
    err = R_DTC_Enable(p_transfer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Attempt to close the DTC module on failure */
        err = dtc_deinit(p_transfer_ctrl);

        /* Return the original error from Enable */
        APP_ERR_RETURN(err, "\r\n** R_DTC_Enable API failed ** \r\n");
    }
    return err;
}
/***********************************************************************************************************************
* End of function dtc_transfer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: normal_mode
*  Description  : This function is used to performs data transfer using DTC in Normal Mode
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t normal_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Set the Source and Destination Addresses for DTC.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_normal_cfg,
            (void*) g_source_data_led1, (void*) p_ioport_pdr_register_led1);

    /* Initialize DTC transfer configuration */
    err = dtc_transfer_init(&g_transfer_normal_ctrl, &g_transfer_normal_cfg);
    APP_ERR_RETURN(err, "\r\n** dtc_transfer_init API failed ** \r\n");

    /* Initialize and Start GPT module */
    err = gpt_timer_init();
    APP_ERR_RETURN(err, "\r\n** gpt_timer_init API failed ** \r\n");

    APP_PRINT("\r\nDTC transfer in progress. Wait for 6 second \n");
    while (!g_dtc_transfer_complete)
    {
        /* Waiting DTC transfer complete */
        R_BSP_SoftwareDelay(DTC_WAIT_LOOP_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    }
    g_dtc_transfer_complete = CLEAR_FLAG;

    APP_PRINT("\r\nDTC transfer completed using Normal Mode \n");
    return err;
}
/***********************************************************************************************************************
* End of function normal_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: repeat_mode
*  Description  : This function is used to executes DTC transfer in Repeat Mode.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t repeat_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Set the Source and Destination Addresses for DTC.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_repeat_cfg,
            (void*) g_source_data_led1, (void*) p_ioport_pdr_register_led1);

    /* Initialize DTC transfer configuration */
    err = dtc_transfer_init(&g_transfer_repeat_ctrl, &g_transfer_repeat_cfg);
    APP_ERR_RETURN(err, "\r\n** dtc_transfer_init API failed ** \r\n");

    /* Initialize GPT module */
    err = gpt_timer_init();
    APP_ERR_RETURN(err, "\r\n** gpt_timer_init API failed ** \r\n");

    APP_PRINT("\r\nDTC transfer triggered every 1 second\r\n");
    for (uint8_t i = ONE; i <= TRANSFER_COUNT; i++)
    {
        /* Set the GPT timer period to trigger DTC every 1 second */
        err = R_GPT_PeriodSet(&g_timer_ctrl, ONE_SECONDS);
        APP_ERR_RETURN(err, "\r\n** R_GPT_PeriodSet API failed ** \r\n");

        /* Start GPT timer to trigger DTC transfer */
        err = R_GPT_Start(&g_timer_ctrl);
        APP_ERR_RETURN(err, "\r\n** R_GPT_Start API failed ** \r\n");

        /* Wait until DTC transfer is marked complete in the callback */
        while (!g_dtc_transfer_complete)
        {
            /* Waiting DTC transfer complete */
            R_BSP_SoftwareDelay(DTC_WAIT_LOOP_DELAY,
                                BSP_DELAY_UNITS_MILLISECONDS);
        }
        /* Clear transfer complete flag for next iteration */
        g_dtc_transfer_complete = CLEAR_FLAG;

        /* Log successful completion of current transfer */
        APP_PRINT("\r\nDTC transfer %d completed.", i);
    }
    APP_PRINT("\r\n\nDTC transfer completed using Repeat Mode");
    return err;
}
/***********************************************************************************************************************
* End of function repeat_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: block_mode
*  Description  : This function is used to executes DTC transfer in Block Mode and verifies the transferred data.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t block_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Set the Source and Destination Addresses for DTC.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_block_cfg,
            (void*) g_src_buffer, (void*) g_dst_buffer);

    /* Initialize DTC transfer configuration */
    err = dtc_transfer_init(&g_transfer_block_ctrl, &g_transfer_block_cfg);
    APP_ERR_RETURN(err, "\r\n** dtc_transfer_init API failed ** \r\n");

    /* Initialize and start GPT module */
    err = gpt_timer_init();
    APP_ERR_RETURN(err, "\r\n** gpt_timer_init API failed ** \r\n");

    while (!g_dtc_transfer_complete)
    {
        /* Waiting DTC transfer complete */
        R_BSP_SoftwareDelay(DTC_WAIT_LOOP_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    }
    g_dtc_transfer_complete = CLEAR_FLAG;

    /* Verify that the data was transferred correctly
     * by comparing source and destination buffers */
    if (ZERO == memcmp(g_src_buffer, g_dst_buffer, DATA_LENGTH))
    {
        APP_PRINT("\r\n[INFO] DTC transfer completed in Block Mode. "
                "Data verified successfully.");
    }
    else
    {
        APP_PRINT("\r\n[ERROR] DTC transfer completed in Block Mode, "
                "but data mismatch detected!");
    }
    return err;
}
/***********************************************************************************************************************
* End of function block_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: set_chain_transfer_dst_src_address
*  Description  : This function is used to initializes chain transfer configuration.
*  Arguments    : None
*  Return Value : None
**********************************************************************************************************************/
static void set_chain_transfer_dst_src_address(void)
{
    /* Configure transfer information for ZERO transfer */
    g_chain_transfer_info[ZERO].p_src = g_source_data_led1;
    g_chain_transfer_info[ZERO].p_dest = (void*)
            (uintptr_t) p_ioport_pdr_register_led1;
    g_chain_transfer_info[ZERO].length = MAX_LENGTH;

    /* Configure transfer information for ONE transfer */
    g_chain_transfer_info[ONE].p_src = g_source_data_led2;
    g_chain_transfer_info[ONE].p_dest = (void*)
            (uintptr_t) p_ioport_pdr_register_led2;
    g_chain_transfer_info[ONE].length = MAX_LENGTH;

    /* p_extend setup */
    g_transfer_cfg_extend.activation_source =
            VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW;

    /* p_cfg setup */
    g_chain_transfer_cfg.p_info = g_chain_transfer_info;
    g_chain_transfer_cfg.p_extend = &g_transfer_cfg_extend;
}
/***********************************************************************************************************************
* End of function set_chain_transfer_dst_src_address
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: chain_mode
*  Description  : This function is used to performs chained DTC transfer triggered by GPT timer.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t chain_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Initialize chain transfer configuration and buffers */
    set_chain_transfer_dst_src_address();

    /* Initialize DTC transfer configuration */
    err = dtc_transfer_init(&g_transfer_normal_ctrl, &g_chain_transfer_cfg);
    APP_ERR_RETURN(err, "\r\n** dtc_transfer_init API failed ** \r\n");

    /* Initialize and Start GPT module */
    err = gpt_timer_init();
    APP_ERR_RETURN(err, "\r\n** gpt_timer_init API failed ** \r\n");

    APP_PRINT("\r\nDTC transfer in progress. Wait for 6 second \n");
    while (!g_dtc_transfer_complete)
    {
        /* Waiting DTC transfer complete */
        R_BSP_SoftwareDelay(DTC_WAIT_LOOP_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    }
    g_dtc_transfer_complete = CLEAR_FLAG;
    APP_PRINT("\r\nDTC transfer completed using Chain Mode\n");
    return err;
}
/***********************************************************************************************************************
* End of function chain_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: set_transfer_dst_src_address
*  Description  : This function is used to set the source and destination address for the DTC transfer instance
*  Arguments    : p_config       Transfer instance configuration structure
*                 p_src          Source address
*                 p_dest         Destination address
*  Return Value : None
**********************************************************************************************************************/
void set_transfer_dst_src_address(transfer_cfg_t const *const p_config, void const *volatile p_src,
        void const *volatile p_dest)
{
    p_config->p_info->p_src = (void*) p_src;
    p_config->p_info->p_dest = (void*) p_dest;
    p_config->p_info->length = MAX_LENGTH;
}
/***********************************************************************************************************************
* End of function set_transfer_dst_src_address
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: timer_deinit
*  Description  : This function is used to GPT timer de-initialization
*  Arguments    : None
*  Return Value : None
**********************************************************************************************************************/
static void timer_deinit(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t err = FSP_SUCCESS;

    /* Close GPT driver  */
    err = R_GPT_Close(&g_timer_ctrl);
    /* Handle error upon failure */
    if (FSP_SUCCESS != err)
    {
        /* Print failure message */
        APP_ERR_PRINT("\r\nR_GPT_Close API failed");
    }
}
/***********************************************************************************************************************
* End of function timer_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: dtc_deinit
*  Description  : This function is used to DTC de-initialization
*  Arguments    : p_ctrl
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t dtc_deinit(dtc_instance_ctrl_t *const p_ctrl)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t err = FSP_SUCCESS;

    /* Disable DTC transfer */
    err = R_DTC_Close(p_ctrl);

    /* Handle error upon failure */
    if (FSP_SUCCESS != err)
    {
        /* Print failure message */
        APP_ERR_PRINT("\r\nR_DTC_Close API failed");
    }
    return err;
}
/***********************************************************************************************************************
* End of function dtc_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: handle_error
*  Description  : This function is used to closes all opened modules, prints and traps the error.
*  Arguments    : err         error value
*                 p_timer_ctl timer instance controller
*                 err_str     error string
*  Return Value : None
**********************************************************************************************************************/
static void handle_error(fsp_err_t err,
        dtc_instance_ctrl_t *const p_ctrl, char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* De-initializes hardware used by sensor */
        timer_deinit();
        dtc_deinit(p_ctrl);
        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: g_timer_callback
*  Description  : This function is used to callback for GPT time overflow.
*  Arguments    : p_args        Pointer to callback argument
*  Return Value : None
**********************************************************************************************************************/
void g_timer_callback(timer_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        if (TIMER_EVENT_CYCLE_END == p_args->event)
        {
            g_dtc_transfer_complete = true;
        }
    }
}
/***********************************************************************************************************************
* End of function g_timer_callback
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @} (end addtogroup DTC_EP)
**********************************************************************************************************************/
