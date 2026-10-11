/***********************************************************************************************************************
 * File Name    :  exdmac_ep.c
 * Description  :  This file implements the EXDMAC functionality for the example project. It handles EXDMAC transfer modes,
 *                 start timer, and error handling
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @addtogroup EXDMAC_EP
* @{
**********************************************************************************************************************/

#include "exdmac_ep.h"
#include "xspi_controller_ep.h"
#include "xspi_controller_commands.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
/* Flag indicating EXDMAC transfer completion */
volatile bool g_exdmac_transfer_complete = false;
/* Timeout tracking */
static uint64_t g_timer_timeout = INITIAL_VALUE;    

/* User menu selection */
uint8_t user_option = RESET_VALUE;

/* Include variable use BSP LED */
extern bsp_leds_t g_bsp_leds;

/* Normal Mode Buffers */
uint32_t g_normal_source_buffer[ONE] __attribute__((section(".xspi0_cs0"))) = {'K'};
uint32_t g_normal_destination_buffer[ONE];

/* Repeat Mode Buffers: first value triggers LED ON, second value (0x00) triggers LED OFF */
uint32_t g_repeat_source_buffer[TWO] __attribute__((section(".xspi0_cs0"))) = {0x00000070U, 0x00000000U};
uint32_t g_repeat_destination_buffer[ONE];

/* Block Mode Buffers */
uint32_t g_block_source_buffer[MAX_LENGTH] __attribute__((section(".xspi0_cs0"))) =
{
  'K','F','X','M','A','Z','U','N','P','D',
  'T','E','R','Y','B','Q','S','O','G','L',
  'H','C','J','V','W','I','A','N','Z','B',
  'X','K','M','D','E','R','T','Q','U','S',
  'Y','F','O','L','G','J','C','V','W','I',
  'P','X','A','N','M','Z','B','H','D','K'
};
uint32_t g_block_destination_buffer[MAX_LENGTH];

/* Repeat Block Mode Buffers */
uint32_t g_repeat_block_source_buffer[SRC_REPEAT_LENGTH] __attribute__((section(".xspi0_cs0"))) =
{
 'H','C','J','V','W','I','A','N','Z','B'
};
uint32_t g_repeat_block_destination_buffer[DST_REPEAT_LENGTH];

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
/* GPT Timer Functions */
static fsp_err_t gpt_timer_init(void);
static void gpt_timer_deinit(void);

/* EXDMAC Transfer Functions */
static fsp_err_t exdmac_transfer_init(exdmac_instance_ctrl_t  * p_transfer_ctrl,
                                      transfer_cfg_t const * const p_transfer_cfg);
static fsp_err_t exdmac_deinit(exdmac_instance_ctrl_t * const p_ctrl);
void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
                                  void const * volatile p_src,
                                  void const * volatile p_dest);
static fsp_err_t normal_mode(void);

static fsp_err_t repeat_mode(void);
/* Function performs software start for EXDMAC transfer instance */
fsp_err_t exdmac_transfer_software_start(transfer_ctrl_t * const p_transfer_ctrl);
void verify_repeat_data(void);

static fsp_err_t block_mode(void);

static fsp_err_t repeat_block_mode(void);

static void handle_error(fsp_err_t err,
        exdmac_instance_ctrl_t  *const p_ctrl,  char * err_str);

static void blink_led(bsp_io_port_pin_t led_pin, uint32_t delay_ms);

/***********************************************************************************************************************
*  Function Name: exdmac_hal_entry
*  Description  : This function is used to start EXDMAC example operation.
*  Arguments    : None
*  Return Value : None
**********************************************************************************************************************/
void exdmac_hal_entry(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t err = FSP_SUCCESS;
    /* FSP variable to store major/minor versions */
    fsp_pack_version_t version = { RESET_VALUE };
    char rByte[SIZE_64] = { RESET_VALUE };

    /* Fetch FSP version to display on console */
    R_FSP_VersionGet(&version);

#if (USE_VIRTUAL_COM == 1)
    /* Turn OFF error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl,
                      (bsp_io_port_pin_t)g_bsp_leds.p_leds[ERR_LED],
                      (bsp_io_level_t) BSP_IO_LEVEL_LOW);

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

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO,
              EP_VERSION,
              version.version_id_b.major,
              version.version_id_b.minor,
              version.version_id_b.patch);

    /* Example Project information printed on Terminal Application */
    APP_PRINT(EP_INFO);
    APP_PRINT(MENU);

    /* Initialize OSPI driver module and Flash device in SPI mode */
    err = xspi_controller_init();
    handle_error(err, NULL, "xspi_controller_init FAILED\r\n");

    /* Handle Repeat Mode for Software Trigger call */
    err = repeat_mode();
    handle_error(err, &g_transfer_repeat_ctrl,
                 "\r\n** Repeat Mode transfer failed **\r\n");

    while (true)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte);
            /* User input selection */
            user_option = (uint8_t) atoi(rByte);
            switch (user_option)
            {
                case NORMAL:
                {
                    err = normal_mode();
                    /* Handle error */
                    handle_error(err, &g_transfer_normal_ctrl,
                                 "\r\n** Normal Mode transfer failed **\r\n");
                    exdmac_deinit(&g_transfer_normal_ctrl);
                    gpt_timer_deinit();
                    APP_PRINT("\r\nEnter any key (other than 1,2,3 or 4) "
                              "to go back to the menu.\r\n");
                    break;
                }
                case REPEAT:
                {
                    /* Trigger software-initiated EXDMAC transfer for Repeat Mode */
                    err = exdmac_transfer_software_start(&g_transfer_repeat_ctrl);
                    /* Handle error */
                    handle_error(err, &g_transfer_repeat_ctrl,
                                 "\r\n** Repeat Mode transfer failed **\r\n");
                    /* Verify repeat data after transfer */
                    verify_repeat_data();
                    APP_PRINT("\r\n\nEnter any key (other than 1,2,3 or 4) "
                              "to go back to the menu.\r\n");
                    break;
                }
                case BLOCK:
                {
                    err = block_mode();
                    /* Handle error */
                    handle_error(err, &g_transfer_block_ctrl,
                                 "\r\n** Block Mode transfer failed **\r\n");
                    exdmac_deinit(&g_transfer_block_ctrl);
                    gpt_timer_deinit();
                    APP_PRINT("\r\n\nEnter any key "
                              "(other than 1,2,3 or 4) to go back to the menu.\r\n");
                    break;
                }
                case REPEAT_BLOCK:
                {
                    err = repeat_block_mode();
                    /* Handle error */
                    handle_error(err, &g_transfer_repeat_block_ctrl,
                                 "\r\n** Repeat Block Mode transfer failed **\r\n");
                    exdmac_deinit(&g_transfer_repeat_block_ctrl);
                    APP_PRINT("\r\nEnter any key (other than 1,2,3 or 4) "
                              "to go back to the menu.\r\n");
                    break;
                }
                default:
                {
                    APP_PRINT("\r\nInvalid menu option!!\r\n");
                    APP_PRINT(MENU);
                    break;
                }
            }
        }
    }
}
/***********************************************************************************************************************
* End of function exdmac_hal_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: gpt_timer_init
*  Description  : This function initializes GPT timer
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
    if (user_option != REPEAT)
    {
        /* Start GPT timer */
        err = R_GPT_Start(&g_timer_ctrl);

        /* Handle error upon failure */
        if (FSP_SUCCESS != err)
        {
            gpt_timer_deinit(); /* Attempt to close GPT */
            APP_ERR_RETURN(err, "\r\n** R_GPT_Start API failed ** \r\n");
        }
    }
    return err;
}
/***********************************************************************************************************************
* End of function gpt_timer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: exdmac_transfer_init
*  Description  : This function is used to initialize EXDMAC transfer
*  Arguments    : p_transfer_ctrl
*                 p_transfer_cfg
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t exdmac_transfer_init(exdmac_instance_ctrl_t *p_transfer_ctrl,
        transfer_cfg_t const *const p_transfer_cfg)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t err = FSP_SUCCESS;

    /* Open the EXDMAC module */
    err = R_EXDMAC_Open(p_transfer_ctrl, p_transfer_cfg);

    /* Handle error upon failure */
    APP_ERR_RETURN(err, "\r\n** R_EXDMAC_Open API failed ** \r\n");

    /* Enable EXDMAC transfer */
    err = R_EXDMAC_Enable(p_transfer_ctrl);

    /* Handle error upon failure */
    if (FSP_SUCCESS != err)
    {
        /* Attempt to close the EXDMAC module on failure */
        exdmac_deinit(p_transfer_ctrl);
    }
    return err;
}
/***********************************************************************************************************************
* End of function exdmac_transfer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: normal_mode
*  Description  : This function is used to execute EXDMAC transfer in Normal Mode and verifies the transferred data
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t normal_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize timeout counter for safety */
    g_timer_timeout = TIMEOUT;
    
    /* Set the Source and Destination Addresses for EXDMAC.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_normal_cfg,
                                 (void*) g_normal_source_buffer,
                                 (void*) g_normal_destination_buffer);

    /* Initialize EXDMAC transfer configuration */
    err = exdmac_transfer_init(&g_transfer_normal_ctrl,
                               &g_transfer_normal_cfg);
    APP_ERR_RETURN(err, "\r\n** exdmac_transfer_init API failed ** \r\n");

    /* Initialize and Start GPT module */
    err = gpt_timer_init();
    APP_ERR_RETURN(err, "\r\n** gpt_timer_init API failed ** \r\n");

    APP_PRINT("\r\nEXDMAC g_transfer_normal transfer in progress... \r\n");
    while (!g_exdmac_transfer_complete)
    {
        /* Wait for the EXDMAC transfer to complete */
        R_BSP_SoftwareDelay(EXDMAC_WAIT_LOOP_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

        --g_timer_timeout;
        /* Check for timeout to prevent infinite loop */
        if (g_timer_timeout == INITIAL_VALUE)
        {
            APP_ERR_PRINT("\r\n[ERROR] EXDMAC Normal Mode transfer timeout!\r\n");
            return FSP_ERR_TIMEOUT;
        }
    }
    g_exdmac_transfer_complete = CLEAR_FLAG;

    /* Verify that the data was transferred correctly
     * by comparing source and destination buffers */
    if (ZERO == memcmp(g_normal_source_buffer,
                       g_normal_destination_buffer,
                       sizeof(g_normal_source_buffer)))
    {
        APP_PRINT("\r\n[INFO] EXDMAC transfer completed in Normal Mode. "
                  "Data verified successfully.");
        APP_PRINT("\r\n[INFO] LED2 lights up for 2 seconds and then turns off.\r\n");
        blink_led((bsp_io_port_pin_t) g_bsp_leds.p_leds[BSP_LED_LED2], LED_ON_DURATION_MS);
    }
    else
    {
        APP_PRINT("\r\n[ERROR] EXDMAC transfer completed in Normal Mode, "
                  "but data mismatch detected!\r\n");
    }
    return err;
}
/***********************************************************************************************************************
* End of function normal_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: repeat_mode
*  Description  : This function is used to execute EXDMAC transfer in Repeat Mode.
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t repeat_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Set the Source and Destination Addresses for EXDMAC.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_repeat_cfg,
                                 (void*) g_repeat_source_buffer,
                                 (void*) g_repeat_destination_buffer);

    /* Initialize EXDMAC transfer configuration */
    err = exdmac_transfer_init(&g_transfer_repeat_ctrl,
                               &g_transfer_repeat_cfg);
    APP_ERR_RETURN(err, "\r\n** exdmac_transfer_init API failed ** \r\n");

    return err;
}
/***********************************************************************************************************************
* End of function repeat_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: block_mode
*  Description  : This function is used to execute EXDMAC transfer in Block Mode and verifies the transferred data
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t block_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize timeout counter for safety */
    g_timer_timeout = TIMEOUT;

    /* Set the Source and Destination Addresses for EXDMAC.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_block_cfg,
                                 (void*) g_block_source_buffer,
                                 (void*) g_block_destination_buffer);

    /* Initialize EXDMAC transfer configuration */
    err = exdmac_transfer_init(&g_transfer_block_ctrl,
                               &g_transfer_block_cfg);
    APP_ERR_RETURN(err, "\r\n** exdmac_transfer_init API failed ** \r\n");

    /* Initialize and start GPT timer */
    err = gpt_timer_init();
    APP_ERR_RETURN(err, "\r\n** gpt_timer_init API failed ** \r\n");

    while (!g_exdmac_transfer_complete)
    {
        /* Waiting EXDMAC transfer complete */
        R_BSP_SoftwareDelay(EXDMAC_WAIT_LOOP_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

        --g_timer_timeout;
        /* Check for timeout to prevent infinite loop */
        if (g_timer_timeout == INITIAL_VALUE)
        {
            APP_ERR_PRINT("\r\n[ERROR] EXDMAC Block Mode transfer timeout!\r\n");
            return FSP_ERR_TIMEOUT;
        }
    }
    g_exdmac_transfer_complete = CLEAR_FLAG;

    /* Verify that the data was transferred correctly
     * by comparing source and destination buffers */
    if (ZERO == memcmp(g_block_source_buffer,
                       g_block_destination_buffer,
                       sizeof(g_block_source_buffer)))
    {
        APP_PRINT("\r\n[INFO] EXDMAC transfer completed in Block Mode. "
                  "Data verified successfully.");
        APP_PRINT("\r\n[INFO] LED2 lights up for 2 seconds and then turns off.\r\n");
        /* LED2 ON for 2 seconds */
        blink_led((bsp_io_port_pin_t) g_bsp_leds.p_leds[BSP_LED_LED2], LED_ON_DURATION_MS);
    }
    else
    {
        APP_PRINT("\r\n[ERROR] EXDMAC transfer completed in Block Mode, "
                  "but data mismatch detected!\r\n");
    }
    return err;
}
/***********************************************************************************************************************
* End of function block_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: repeat_block_mode
*  Description  : This function is used to execute EXDMAC transfer in Repeat Block Mode and verifies the transferred data
*  Arguments    : None
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t repeat_block_mode(void)
{
    fsp_err_t err = FSP_SUCCESS;

    bool is_data_match = true;

    /* Set the Source and Destination Addresses for EXDMAC.
     * NOTE: If the source and destination addresses are not set prior to
     * opening the transfer interface, then the open will fail. */
    set_transfer_dst_src_address(&g_transfer_repeat_block_cfg,
                                 (void*) g_repeat_block_source_buffer,
                                 (void*) g_repeat_block_destination_buffer);

    /* Initialize EXDMAC transfer configuration */
    err = exdmac_transfer_init(&g_transfer_repeat_block_ctrl,
                               &g_transfer_repeat_block_cfg);
    APP_ERR_RETURN(err, "\r\n** exdmac_transfer_init API failed ** \r\n");

    /*
     * GPT operates in one-shot mode. Restart it once per block to generate
     * the required number of EXDMAC activation requests.
     */
    for (uint32_t i = 0; i < REPEAT_COUNT; i++)
    {
        /* Initialize timeout counter for safety */
        g_timer_timeout = TIMEOUT;
        g_exdmac_transfer_complete = CLEAR_FLAG;
        /* Initialize and start GPT module */
        err = gpt_timer_init();
        APP_ERR_RETURN(err, "\r\n** gpt_timer_init API failed ** \r\n");

        while (!g_exdmac_transfer_complete)
        {
            R_BSP_SoftwareDelay(EXDMAC_WAIT_LOOP_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

            --g_timer_timeout;
            /* Check for timeout to prevent infinite loop */
            if (g_timer_timeout == INITIAL_VALUE)
            {
                APP_ERR_PRINT("\r\n[ERROR] EXDMAC Repeat Block Mode transfer timeout!\r\n");
                return FSP_ERR_TIMEOUT;
            }
        }

       gpt_timer_deinit();
    }

    for (uint32_t i = 0; i < REPEAT_COUNT; i++)
    {
        if (ZERO != memcmp(&g_repeat_block_destination_buffer[i * SRC_REPEAT_LENGTH],
                           g_repeat_block_source_buffer,
                           BLOCK_BYTES))
        {
            APP_PRINT("\r\n[ERROR] Data mismatch at block %u", (unsigned) i);
            is_data_match = false;
        }
    }

    g_exdmac_transfer_complete = CLEAR_FLAG;

    /* Verify that the data was transferred correctly
     * by comparing source and destination buffers */
    if (is_data_match)
    {
        APP_PRINT("\r\n[INFO] EXDMAC transfer completed in Repeat-Block Mode. "
                  "Data verified successfully.");
        APP_PRINT("\r\n[INFO] LED2 lights up for 2 seconds and then turns off.\r\n");
        /* LED2 ON for 2 seconds */
        blink_led((bsp_io_port_pin_t) g_bsp_leds.p_leds[BSP_LED_LED2], LED_ON_DURATION_MS);
    }
    else
    {
        APP_PRINT("\r\n[ERROR] EXDMAC transfer completed in Repeat-Block Mode, "
                  "but data mismatch detected!\r\n");
    }
    return err;
}
/***********************************************************************************************************************
* End of function repeat_block_mode
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: set_transfer_dst_src_address
*  Description  : This function is used to set the source and destination address for the EXDMAC transfer instance
*  Arguments    : p_config       Transfer instance configuration structure
*                 p_src          Source address
*                 p_dest         Destination address
*  Return Value : None
**********************************************************************************************************************/
void set_transfer_dst_src_address(transfer_cfg_t const *const p_config,
                                  void const *volatile p_src,
                                  void const *volatile p_dest)
{
    p_config->p_info->p_src = (void*) p_src;
    p_config->p_info->p_dest = (void*) p_dest;
}
/***********************************************************************************************************************
* End of function set_transfer_dst_src_address
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: gpt_timer_deinit
*  Description  : This function is used to de-initialize GPT timer
*  Arguments    : None
*  Return Value : None
**********************************************************************************************************************/
static void gpt_timer_deinit(void)
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
* End of function gpt_timer_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: exdmac_deinit
*  Description  : This function is used to de-initialize EXDMAC
*  Arguments    : p_ctrl
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
static fsp_err_t exdmac_deinit(exdmac_instance_ctrl_t *const p_ctrl)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t err = FSP_SUCCESS;

    /* Disable EXDMAC transfer */
    err = R_EXDMAC_Close(p_ctrl);

    /* Handle error upon failure */
    if (FSP_SUCCESS != err)
    {
        /* Print failure message */
        APP_ERR_PRINT("\r\nR_EXDMAC_Close API failed");
    }
    return err;
}
/***********************************************************************************************************************
* End of function exdmac_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: handle_error
*  Description  : This function is used to close all opened modules, prints and traps the error.
*  Arguments    : err         error value
*                 p_ctrl      instance controller
*                 err_str     error string
*  Return Value : None
**********************************************************************************************************************/
static void handle_error(fsp_err_t err,
        exdmac_instance_ctrl_t *const p_ctrl, char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT("%s", err_str);

        /* De-initializes the timer */
        gpt_timer_deinit();

        if (NULL != p_ctrl)
        {
            exdmac_deinit(p_ctrl);
        }

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: g_timer_callback
*  Description  : This function is used to callback for GPT timer overflow.
*  Arguments    : p_args        Pointer to callback argument
*  Return Value : None
**********************************************************************************************************************/
void g_timer_callback(timer_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        if (TIMER_EVENT_CYCLE_END == p_args->event)
        {
            g_exdmac_transfer_complete = true;
        }
    }
}
/***********************************************************************************************************************
* End of function g_timer_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: exdmac_transfer_software_start
*  Description  : This function is used to start EXDMAC transfer software.
*  Arguments    : p_transfer_ctrl        Pointer to argument
*  Return Value : FSP_SUCCESS    Upon successful operation
*                 Any Other Error code apart from FSP_SUCCESS
**********************************************************************************************************************/
fsp_err_t exdmac_transfer_software_start(transfer_ctrl_t * const p_transfer_ctrl)
{

    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Start EXDMAC transfer by software */
    fsp_err = R_EXDMAC_SoftwareStart(p_transfer_ctrl, TRANSFER_START_MODE_SINGLE);

    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("\r\nEXDMAC_SoftwareStart failed.\n");
    }

    return fsp_err;
}
/***********************************************************************************************************************
* End of function exdmac_transfer_software_start
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: verify_repeat_data
*  Description  : This function is used to verify data in repeat mode.
*  Arguments    : None
*  Return Value : None
**********************************************************************************************************************/
void verify_repeat_data(void)
{
    /* LED1 state */
    bsp_io_level_t led1_state = BSP_IO_LEVEL_LOW;

    R_BSP_SoftwareDelay(EXDMAC_WAIT_LOOP_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

    if (g_repeat_destination_buffer[0] != g_repeat_source_buffer[1])
    {
        led1_state = BSP_IO_LEVEL_HIGH;
        R_IOPORT_PinWrite(&g_ioport_ctrl,
                          (bsp_io_port_pin_t) g_bsp_leds.p_leds[BSP_LED_LED1],
                          (bsp_io_level_t) led1_state);

        APP_PRINT("\r\n[INFO] EXDMAC transfer completed in Repeat Mode. LED1 ON.");
    }
    else
    {
        led1_state = BSP_IO_LEVEL_LOW;
        R_IOPORT_PinWrite(&g_ioport_ctrl,
                          (bsp_io_port_pin_t) g_bsp_leds.p_leds[BSP_LED_LED1],
                          (bsp_io_level_t) led1_state);

        APP_PRINT("\r\n[INFO] EXDMAC transfer completed in Repeat Mode. LED1 OFF.");
    }
}
/***********************************************************************************************************************
* End of function verify_repeat_data
***********************************************************************************************************************/

/***********************************************************************************************************************
*  Function Name: blink_led
*  Description  : This function is used to blink an led.
*  Arguments    : led_pin     LED pin to blink
*                 delay_ms    Delay in milliseconds
*  Return Value : None
**********************************************************************************************************************/
static void blink_led(bsp_io_port_pin_t led_pin, uint32_t delay_ms)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(delay_ms, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, led_pin, BSP_IO_LEVEL_LOW);
}
/***********************************************************************************************************************
* End of function blink_led
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @} (end addtogroup EXDMAC_EP)
**********************************************************************************************************************/
