/***********************************************************************************************************************
 * File Name    : mram_ep.c
 * Description  : This file implements the MRAM functionality for the example project. It handles erase, write 
 *                operations to code mram and data mram memory
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
#include "mram_ep.h"

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
/* Private function declarations */
/* This function is used to initialize mram module */
static void mram_init(void);
/* This function is used to de-initialize mram module */
static void mram_deinit(void);
/* This function is used to write data to mram memory */
static void mram_write(void);
/* This function is used to erase data from mram memory */
static void mram_erase(void);
/* This function is used to read data from mram memory */
static void mram_read(void);
/* This function is used to handle error */
static void handle_error(fsp_err_t err, const char *api_name, const char *user_msg);
/* This function is used to handle mram operations selection: write, erase, read, go back */
static void handle_mram_menu(const char *menu_title);

/* If the BGO is used, user need to define macro BGO_SUPPORTED */
#ifdef BGO_SUPPORTED
/* Callback function */
void bgo_callback(flash_callback_args_t *p_args);
#endif

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
#ifdef BGO_SUPPORTED
/* This variable is used to notify mram memory erase has been completed */
static volatile _Bool g_b_mram_event_erase_complete = false;
/* This variable is used to notify mram memory write has been completed */
static volatile _Bool g_b_mram_event_write_complete = false;
#endif

/* This variable is used to notify mram module was initialized or not */
static _Bool g_mram_initialized        = false;
/* This variable is used to notify the EP is running in data mram region */
static _Bool g_code_mram_region        = false;
/* This variable is used to notify the EP is running in data mram region */
static _Bool g_data_mram_region        = false;
/* This variable is used to indicate the write buffer */
static uint8_t write_buffer[NUM_BYTES] = {RESET_VALUE};
/* This variable is used to indicate the read buffer */
static uint8_t read_buffer[NUM_BYTES]  = {RESET_VALUE};
/* This variable is used to indicate the MRAM address */
static uint32_t mram_address           = RESET_VALUE;
/* This variable is used to indicate the number of MRAM blocks */
static uint8_t mram_num_blocks         = RESET_VALUE;
extern bsp_leds_t g_bsp_leds;

/*******************************************************************************************************************//**
 * @addtogroup MRAM_EP
 * @{
 **********************************************************************************************************************/
 /**********************************************************************************************************************
 * Function Definition Section
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 *  @brief       This function is used to initialize related modules and start the MRAM example operation.
 * 
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
void mram_ep_entry()
{
    fsp_pack_version_t version;                      /* Version information */
    uint8_t user_option   = INIT_VALUE;              /* User option */
    char rByte[BUFF_SIZE] = {RESET_VALUE};           /* Read buffer */

    /* Fetch FSP version to display on console */
    R_FSP_VersionGet(&version);

    /* Turn OFF error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                     (bsp_io_level_t)BSP_IO_LEVEL_LOW);
#if (USE_VIRTUAL_COM == 1)
    fsp_err_t err = FSP_SUCCESS;

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
    APP_PRINT(MRAM_EP_INFO);
    APP_PRINT(EP_MENU_MRAM_REGION);

    /* Initialize mram module */
    mram_init();

    while (EXIT != user_option)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte);
            if (strlen((char *) rByte) > 2U)
            {
                APP_PRINT(EP_MENU_MRAM_REGION);
                user_option = INIT_VALUE;
                memset(rByte, RESET_VALUE, BUFF_SIZE);
                continue;
            }
            user_option = (uint8_t)rByte[0];

            /* Select the MRAM region: CODE MRAM or DATA MRAM */
            switch (user_option)
            {
                case CODE_MRAM_REGION:
                    g_code_mram_region = true;
                    handle_mram_menu("EP running in CODE MRAM region");
                    g_code_mram_region = false;
                    break;

                case DATA_MRAM_REGION:
                    g_data_mram_region = true;
                    handle_mram_menu("EP running in DATA MRAM region");
                    g_data_mram_region = false;
                    break;

                case EXIT:
                    APP_PRINT("\r\nExiting the MRAM. "
                            "Please restart the application.\r\n");
                    mram_deinit();
                    continue;

                default:
                    APP_PRINT(EP_MENU_MRAM_REGION);
                    break;
            }

            if ((user_option == CODE_MRAM_REGION) ||
                    (user_option == DATA_MRAM_REGION))
            {
                APP_PRINT(EP_MENU_MRAM_REGION);
            }

            /* Reset value */
            user_option = INIT_VALUE;
            memset(rByte, RESET_VALUE, BUFF_SIZE);
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to handle mram operations selection: write, erase, read, go back
 * 
 *  @param[in]   menu_title     Title of the operation
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void handle_mram_menu(const char *menu_title)
{
    /* User option */
    uint8_t user_option   = INIT_VALUE;
    /* Read buffer */
    char rByte[BUFF_SIZE]  = {RESET_VALUE};
    /* Condition for breaking the loop */
    _Bool break_loop       = false;

    APP_PRINT("\r\n\n------------------------------------------------------------------------");
    APP_PRINT("\r\n                %s", menu_title);

    APP_PRINT(EP_MENU_MRAM_OPERATION);

    while (break_loop == false)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte);
            if (strlen((char *) rByte) > 2U)
            {
                APP_PRINT(EP_MENU_MRAM_OPERATION);
                memset(rByte, RESET_VALUE, BUFF_SIZE);
                continue;
            }
            user_option = (uint8_t)rByte[0];

            switch (user_option)
            {
                case MRAM_WRITE:
                    APP_PRINT("\r\n------------------------------------------------------------------------");
                    APP_PRINT("\r\n        MRAM Write operations\r\n");
                    mram_write();
                    break;

                case MRAM_ERASE:
                    APP_PRINT("\r\n------------------------------------------------------------------------");
                    APP_PRINT("\r\n        MRAM Erase operations\r\n");
                    mram_erase();
                    break;

                case MRAM_READ:
                    APP_PRINT("\r\n------------------------------------------------------------------------");
                    APP_PRINT("\r\n        MRAM Read operations\r\n");
                    mram_read();
                    break;

                case EXIT:
                    break_loop = true;
                    break;

                default:
                    APP_PRINT(EP_MENU_MRAM_OPERATION);
                    break;
            }

            if ((user_option >= MRAM_WRITE) && (user_option <= MRAM_READ))
            {
                APP_PRINT("------------------------------------------------------------------------");
                APP_PRINT("\r\nEnter any key (other than 0, 1, 2, 3) "
                        "to go back to the MRAM operations selection menu: ");
            }

            /* Reset value */
            user_option = INIT_VALUE;
            memset(rByte, RESET_VALUE, BUFF_SIZE);
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to handle errors if they occur: closes all opened modules, prints the error 
 *               and enters a trap state
 * 
 *  @param[in]   err        Error code
 *  @param[in]   api_name   The API name to be checked.
 *  @param[in]   user_msg   User message
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, const char *api_name, const char *user_msg)
{
    if (FSP_SUCCESS != err)
    {
        APP_PRINT("\r\n[ERR] "
                "In Function: %s() \r\n%s \r\n", api_name, user_msg);

        if (g_mram_initialized == true)
        {
            mram_deinit();
        }

        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is used to initialize MRAM module.
 * 
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
void mram_init()
{
    fsp_err_t err = FSP_SUCCESS;        /* Error status */

    /* Check status */
    if (g_mram_initialized == false)
    {
        /* Open MRAM module */
        err = R_MRAM_Open(&g_mram_ctrl, &g_mram_cfg);
        /* Handle Error */
        handle_error(err, "mram_init", "R_MRAM_Open failed");

        /* Set flag to notify mram module was initialized */
        g_mram_initialized = true;
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is used to de-initialize MRAM module.
 * 
 * @param[in]   None
 * @param[out]  None
 * @retval      None
 **********************************************************************************************************************/
void mram_deinit(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Check status */
    if (g_mram_initialized == true)
    {
        /* Reset flag */
        g_mram_initialized = false;

        /* Close module */
        err = R_MRAM_Close(&g_mram_ctrl);
        /* Error handle */
        handle_error(err, "mram_deinit", "API R_MRAM_Close failed");
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to write data to mram memory
 * 
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void mram_write()
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */

    /* Set MRAM address: CODE MRAM or DATA MRAM */
    if (g_code_mram_region == true)
    {
        mram_address = CODE_MRAM_ADDRESS;
    }
    else if (g_data_mram_region == true)
    {
        mram_address = DATA_MRAM_ADDRESS;
    }

    /* Prepare data */
    for (uint8_t index = 0; index < NUM_BYTES; index++)
    {
        write_buffer[index] = index;
    }

    /* Disable interrupts to prevent vector table access
     * while MRAM is programming. */
    __CLRPSW_I();

    /* Write data to MRAM memory */
    err = R_MRAM_Write(&g_mram_ctrl,
                        (uint32_t)write_buffer, mram_address, NUM_BYTES);

    /* Error Handle */
    handle_error(err, "mram_write", "API R_MRAM_Write failed");

/* If the BGO is used, user need to define macro BGO_SUPPORTED */
#ifdef BGO_SUPPORTED
    /* Handle the case when the EP is running a data mram operation */
    if (g_data_mram_region == true)
    {
        /* Wait for the write complete event flag, if BGO is SET  */
        if (true == g_mram_cfg.data_flash_bgo)
        {
            while (!g_b_mram_event_write_complete);
            g_b_mram_event_write_complete = false;
        }
    }
#endif

    APP_PRINT("\r\n        [MRAM WRITE]: Write successful. "
            "Data written: 0x00 to 0x%02X", NUM_BYTES - 1);

    /* Read data from mram memory to verify */
    memcpy(read_buffer, (uint8_t *)mram_address, NUM_BYTES);

    /* Verify write data */
    if (RESET_VALUE == memcmp(read_buffer, write_buffer, NUM_BYTES))
    {
        APP_PRINT("\r\n        [MRAM WRITE]: Data verification successful\r\n");
    }
    else
    {
        APP_PRINT("\r\n        [MRAM WRITE]: Data verification failed, "
                "mismatch detected\r\n");
        err = FSP_ERR_WRITE_FAILED;
        APP_ERR_TRAP(err);
    }

    /* Enable interrupts after MRAM operations are complete. */
    __SETPSW_I();
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to erase data from mram memory
 * 
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void mram_erase()
{
    fsp_err_t err = FSP_SUCCESS; /* Error status */

    /* Set MRAM address and MRAM num blocks */
    if (g_code_mram_region == true)
    {
        mram_address    = CODE_MRAM_ADDRESS;
        mram_num_blocks = CODE_MRAM_NUM_BLOCKS;
    }
    else if (g_data_mram_region == true)
    {
        mram_address    = DATA_MRAM_ADDRESS;
        mram_num_blocks = DATA_MRAM_NUM_BLOCKS;
    }

    uint8_t *p_addr = (uint8_t *)mram_address;

    /* Disable interrupts to prevent vector table access
     * while MRAM is programming. */
    __CLRPSW_I();

    /* Erase data from MRAM memory */
    err = R_MRAM_Erase(&g_mram_ctrl, mram_address, mram_num_blocks);

    /* Error Handle */
    handle_error(err, "mram_erase", "API R_MRAM_Erase failed");

/* If the BGO is used, user need to define macro BGO_SUPPORTED */
#ifdef BGO_SUPPORTED
    /* Handle the case when the EP is running a data mram operation */
    if (g_data_mram_region == true)
    {
        /* Wait for the erase complete event flag, if BGO is SET */
        if (true == g_mram_cfg.data_flash_bgo)
        {
            APP_PRINT("\r\nBGO has enabled");
            while (!g_b_mram_event_erase_complete);
            g_b_mram_event_erase_complete = false;
        }
    }
#endif

    APP_PRINT("\r\n        [MRAM ERASE]: Erase successful");

    /* Verify erase data */
    for (uint8_t index = 0; index < NUM_BYTES; index++)
    {
        if (p_addr[index] != UINT8_MAX)
        {
            APP_PRINT("\r\n        [MRAM ERASE]: "
                    "Data verification failed, mismatch detected");
            APP_PRINT("\r\n        [MRAM ERASE]: "
                    "Erase failed at index: %d \r\n" , index);
            err = FSP_ERR_ERASE_FAILED;
            APP_ERR_TRAP(err);
        }
    }

    APP_PRINT("\r\n        [MRAM ERASE]: Data verification successful\r\n");

    /* Enable interrupts after MRAM operations are complete. */
    __SETPSW_I();
}

/*******************************************************************************************************************//**
 *  @brief       This function is used read data from MRAM memory
 * 
 *  @param[in]   None
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
static void mram_read()
{
    /* Set MRAM address: CODE MRAM or DATA MRAM */
    if (g_code_mram_region == true)
    {
        mram_address = CODE_MRAM_ADDRESS;
    }
    else if (g_data_mram_region == true)
    {
        mram_address = DATA_MRAM_ADDRESS;
    }

    /* Read data from MRAM into RAM read_buffer */
    memcpy(read_buffer, (uint8_t *)mram_address, NUM_BYTES);

    /* Print data */
    APP_PRINT("\r\n        Read data is shown below:\r\n");
    APP_PRINT("\r\n        ");

    for (uint32_t index = 0; index < NUM_BYTES; index++)
    {
        APP_PRINT("%02X    ", read_buffer[index]);

        if (((index + 1) % 8) == 0)
        {
            APP_PRINT("\r\n        ");
        }
    }
}

/* If the BGO is used, user need to define macro BGO_SUPPORTED */
#ifdef BGO_SUPPORTED
/*******************************************************************************************************************//**
 *  @brief       This function is callback
 * 
 *  @param[in]   p_args
 *  @param[out]  None
 *  @retval      None
 **********************************************************************************************************************/
void bgo_callback(flash_callback_args_t *p_args)
{
    if (FLASH_EVENT_ERASE_COMPLETE == p_args->event)
    {
        g_b_mram_event_erase_complete = true;
    }
    else if (FLASH_EVENT_WRITE_COMPLETE == p_args->event)
    {
        g_b_mram_event_write_complete = true;
    }
    else
    {
        /*No operation */
    }
}
#endif

/*******************************************************************************************************************//**
 * @} (end addtogroup MRAM_EP)
 **********************************************************************************************************************/
