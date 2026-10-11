/***********************************************************************************************************************
 * File Name    : sdhi_ep.c
 * Description  : This file contains data structures and functions used in sdhi_ep.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "sdhi_ep.h"

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
sdmmc_device_t g_device;
static volatile sdmmc_response_t g_last_response;
extern bsp_leds_t g_bsp_leds;
static volatile uint32_t transfer_complete = RESET_VALUE;                   /* Flag set transfer is complete */
static volatile uint32_t transfer_error    = RESET_VALUE;                   /* Flag set transfer is error */
static volatile uint32_t g_erase_complete  = RESET_VALUE;                   /* Flag set erase is complete */
static volatile uint32_t sector_start      = RESET_VALUE;                   /* Sector starts writing to SD */
static uint8_t g_buff[SDMMC_BUFFER_SIZE] BSP_ALIGN_VARIABLE(4);             /* Buffer get data from SD */
static uint8_t g_code_buff[SDMMC_BUFFER_SIZE] BSP_ALIGN_VARIABLE(4) = {0};  /* Buffer random data and send via SD */

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
static fsp_err_t sdhi_operation(void);
static uint32_t process_input_data(void);
static fsp_err_t write_data(void);
static fsp_err_t read_data(void);
static fsp_err_t erase_data(void);
static fsp_err_t wait_for_transfer_complete(void);
static fsp_err_t wait_for_device(void);
static void generate_random_data(uint8_t *buffer);
static void deinit_sdhi_module(void);
static void handle_error(fsp_err_t err, char *err_str);

/*******************************************************************************************************************//**
 *  @brief       This function is used to initialize related module and start the SDHI example operation.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void sdhi_entry(void)
{
    fsp_err_t err              = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };
    bsp_io_level_t pin_value   = BSP_IO_LEVEL_LOW;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

#if (USE_VIRTUAL_COM == 1)
    /* Turn OFF error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_LOW);

    /* Initialize terminal */
    err = (fsp_err_t)TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON error LED to indicate uart_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_HIGH);

        /* Error trap */
        ERROR_TRAP;
    }
#endif

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(EP_INFO);

    /* Check whether the battery is powered or not */
    R_IOPORT_PinRead(&g_ioport_ctrl, SDHI_PE_PIN, &pin_value);
    if (RESET_VALUE == pin_value)
    {
        APP_PRINT("\r\nSDHI_PE_PIN has incorrect configuration, please select Mode -> Output Mode (Initial High).\r\n");
        APP_ERR_TRAP(err);
    }

    /* Randomly generate data and assign it to buffer write */
    generate_random_data(g_code_buff);

    /* Open module */
    err = R_SDHI_Open(&g_sdmmc_ctrl, &g_sdmmc_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_PRINT("\r\n** R_SDHI_Open API FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    err = R_SDHI_MediaInit(&g_sdmmc_ctrl, &g_device);

    /* Handle error */
    handle_error(err, "\r\n** R_SDHI_MediaInit API Failed **\r\n");

    /* Get information from SD Card */
    uint32_t memory_SD = (uint32_t)(
        ((float)g_device.sector_count * CONVERT_BYTE_TO_MEGABYTE)
        * (float)g_device.sector_size_bytes
    );
    APP_PRINT(SD_INFO, memory_SD);

    /* Check if the SD card is write-protected */
    if (g_device.write_protected == false)
    {
        APP_PRINT(SD_STATUS(g_device.write_protected));
    }
    else
    {
        APP_PRINT(SD_STATUS(g_device.write_protected));
        APP_ERR_TRAP(FSP_ERR_CARD_WRITE_PROTECTED);
    }

    /* Display Menu options for the user to select */
    APP_PRINT(EP_MENU);

    while (true)
    {
        if (APP_CHECK_DATA)
        {
            err = sdhi_operation();
            handle_error(err, "\r\n** sdhi_operation API Failed **\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function performs SD Card write operation.
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
static fsp_err_t write_data(void)
{
    fsp_err_t err = FSP_SUCCESS; /* Variable to store error status. */

    /* Print a message indicating the start of the write process. */
    APP_PRINT("\r\n\r\nSector starts writing: ");

    /* Process user input to get the starting sector. */
    sector_start = process_input_data();

    /* Write data from the buffer g_code_buff to the SD card, starting from sector_start. */
    err = R_SDHI_Write(&g_sdmmc_ctrl, g_code_buff, sector_start, SECTOR_COUNT);

    /* Check and handle any error that occurs during the write operation. */
    APP_ERR_RETURN(err, "\r\n** R_SDHI_Write API Failed **\r\n");

    /* Wait for the write operation to complete. */
    err = wait_for_transfer_complete();
    /* Handle errors */
    APP_ERR_RETURN(err, "\r\n** wait_for_transfer_complete API Failed **\r\n");

    /* Notify that data has been successfully written to the SD card. */
    APP_PRINT("\r\n\r\nData has been written to SD Card successfully at sector %u.\r\n", sector_start);

    /* Prompt the user to press any key to return to the main menu. */
    APP_PRINT("\r\nPress any other key (except 1, 2 and 3) to go back to the main menu: \r\n");

    return err; /* Return the status of the write operation. */
}

/*******************************************************************************************************************//**
 * @brief       This function performs SD Card read operation.
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
static fsp_err_t read_data(void)
{
    fsp_err_t err         = FSP_SUCCESS;    /* Variable to store error status. */
    bool is_sdcard_empty  = true;           /* Flag to check if the buffer is all zeros. */
    bool arrays_are_equal = true;           /* Flag to check if buffers are identical. */

    err = R_SDHI_Read(&g_sdmmc_ctrl, g_buff, sector_start, SECTOR_COUNT);
    /* Handle error */
    APP_ERR_RETURN(err, "\r\n** R_SDHI_Read API Failed **\r\n");

    /* Wait for the read operation to complete. */
    err = wait_for_transfer_complete();
    /* Handle errors */
    APP_ERR_RETURN(err, "\r\n** wait_for_transfer_complete API Failed **\r\n");

    for (uint32_t i = RESET_VALUE; i < SDMMC_BUFFER_SIZE; i++)
    {
        if (g_buff[i] == RESET_VALUE)
        {
            is_sdcard_empty = false; /* Buffer contains non-zero data. */
        }

        if (g_buff[i] != g_code_buff[i])
        {
            arrays_are_equal = false; /* Data mismatch found. */
        }

        /* If both flags are defined, early termination is possible */
        if (!is_sdcard_empty && !arrays_are_equal)
        {
            break;
        }
    }
    /* Process the result after checking the entire array. */
    if (!is_sdcard_empty)
    {
        APP_PRINT("\r\nNo data available on the SD card\r\n");
    }
    else if (!arrays_are_equal)
    {
        APP_PRINT("\r\nThe data read does not match the data written.\r\n");
    }
    else
    {
        APP_PRINT("\r\n\r\nData has been read successfully and saved to SD Card");
        APP_PRINT("\r\nThe data read matches the data written. Integrity verified\r\n");
    }
    /* Prompt the user to press any key to return to the main menu. */
    APP_PRINT("\r\nPress any other key (except 1, 2 and 3) to go back to the main menu: \r\n");

    return err; /* Return the status of the read operation. */
}

/*******************************************************************************************************************//**
 * @brief       This function performs SD Card Erase operation.
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
static fsp_err_t erase_data(void)
{
    fsp_err_t err = FSP_SUCCESS; /* Variable to store error status. */

    /* Notify the user that valid data has been detected and erasure is beginning. */
    APP_PRINT("\r\n\r\nData detected on the SD card. Proceeding to erase...\r\n");

    /* Attempt to erase the specified sectors on the SD card. */
    err = R_SDHI_Erase(&g_sdmmc_ctrl, sector_start, SECTOR_COUNT);

    /* Check and handle any error that occurs during the erase operation. */
    APP_ERR_RETURN(err, "\r\n** R_SDHI_Erase API Failed **\r\n");

    /* Wait for the device to complete the operation */
    err = wait_for_device();
    /* Handle errors */
    APP_ERR_RETURN(err, "\r\n** wait_for_device API Failed **\r\n");

    /* Notify the user that the data has been successfully erased. */
    APP_PRINT("\r\nData has been erased successfully\r\n");

    /* Prompt the user to press any key to return to the main menu. */
    APP_PRINT("\r\nPress any other key (except 1, 2 and 3) to go back to the main menu: \r\n");

    return err; /* Return the final status of the erase operation. */
}

/*******************************************************************************************************************//**
 * @brief       This function reads the command (input) from User and processes the command (input).
 * @param[in]   None
 * @retval      FSP_SUCCESS             Upon successful, action of SDHI read, write, or erase SD
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful read, write, or erase
 ***********************************************************************************************************************/
static fsp_err_t sdhi_operation(void)
{
    fsp_err_t err                   = FSP_SUCCESS;                   /* Error status */
    char readBuff[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    uint32_t rByte                  = APP_READ(readBuff);            /* Read user input */
    /* Function returns the converted integral number as an int value.*/
    int32_t inputRead               = atoi((char *)readBuff);

    if (rByte > RESET_VALUE)
    {
        switch (inputRead)
        {
            case SD_WRITE:
            {
                err = write_data();
                /* Handle error */
                APP_ERR_RETURN(err, "\r\n** write_data API Failed **\r\n");
            }
            break;
            case SD_READ:
            {
                err = read_data();
                /* Handle error */
                APP_ERR_RETURN(err, "\r\n** read_data API Failed **\r\n");
            }
            break;
            case SD_ERASE:
            {
                err = erase_data();
                /* Handle error */
                APP_ERR_RETURN(err, "\r\n** erase_data API Failed **\r\n");
            }
            break;
            default:
            {
                /* Menu for User Selection */
                APP_PRINT("\r\nInvalid value input:\r\n");
                APP_PRINT(EP_MENU);
            }
            break;
        }
    }
    return err;
}

/*****************************************************************************************************************
 *  @brief      Process input string to integer value
 *  @param[in]  None
 *  @retval     Integer value of input string.
 ****************************************************************************************************************/
static uint32_t process_input_data(void)
{
    char buf[BUFFER_SIZE_DOWN] = { INITIAL_VALUE };
    uint32_t num_bytes         = RESET_VALUE;
    uint32_t value             = RESET_VALUE;

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
    value = (uint32_t)(atoi((char *)buf));
    return value;
}

/*******************************************************************************************************************//**
 * @brief       Generate random data and store it in buffer.
 * @param[in]   buffer - Pointer to array `uint8_t` to store random values.
 * @retval      None.
 ***********************************************************************************************************************/
static void generate_random_data(uint8_t *buffer)
{
    /* Generate random values between 1 and 255 */
    for (uint32_t i = RESET_VALUE; i < SDMMC_BUFFER_SIZE; i++)
    {
        buffer[i] = (uint8_t)(rand() % 255) + 1; /* Generate random number between 1 and 255 */
    }
}

/*******************************************************************************************************************//**
 * @brief       Waiting status that SD returns
 * @param[in]   Status error
 * @retval      None
 **********************************************************************************************************************/
static fsp_err_t wait_for_transfer_complete(void)
{
    fsp_err_t err    = FSP_SUCCESS;
    uint32_t timeout = UINT32_MAX;

    while (timeout-- > RESET_VALUE)
    {
        /* If the timeout expires without a response, a timeout error is returned */
        if (RESET_VALUE == timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "\r\n** No events received **\r\n");
        }
        /* Check if the transfer is complete */
        if (transfer_complete)
        {
            /* Reset flag and return*/
            transfer_complete = RESET_VALUE;
            transfer_error = RESET_VALUE;
            return err;
        }
        /*  Check if there is a transmission error */
        if (transfer_error)
        {
            /* Reset flag and return*/
            transfer_error = RESET_VALUE;
            return err;
        }
    }
    return FSP_ERR_TIMEOUT;
}

/*******************************************************************************************************************//**
 * @brief       Waiting status that SD returns
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
static fsp_err_t wait_for_device(void)
{
    fsp_err_t err    = FSP_SUCCESS;
    uint32_t timeout = UINT32_MAX;

    /* Loop checks if deletion is completed or device is no longer busy */
    while (timeout > RESET_VALUE)
    {
        /* If the timeout expires without a response, a timeout error is returned */
        if (RESET_VALUE == timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "\r\n** No events received **\r\n");
        }
        /* If the deletion is complete then exit the loop */
        if (g_erase_complete)
        {
            break;
        }
    }

    sdmmc_status_t status;
    memset(&status, RESET_VALUE, sizeof(status));
    timeout = UINT32_MAX;

    do
    {
        timeout--;
        /* If the timeout expires without a response, a timeout error is returned */
        if (RESET_VALUE == timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "\r\n** No events received **\r\n");
        }
        /* Get status for device */
        err = R_SDHI_StatusGet(&g_sdmmc_ctrl, &status);
        /* Handle error */
        APP_ERR_RETURN(err, "\r\n** R_SDHI_StatusGet API Failed **\r\n");
    } while (status.transfer_in_progress); /* Continue until the device is no longer busy */

    /* Reset status when deletion is complete or error occurs */
    transfer_complete = RESET_VALUE;
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Close the SDHI driver and handle the return closing API error, to the Application.
 * @param[in]   None
 * @retval      None.
 ***********************************************************************************************************************/
static void deinit_sdhi_module(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (MODULE_CLOSE != g_sdmmc_ctrl.open)
    {
        /* Close the SDHI driver */
        err = R_SDHI_Close(&g_sdmmc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** R_SDHI_Close API failed **  \r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function calls back to SDHI and is triggered when Write, Read or Erase.
 * @param[in]   None
 * @retval      None.
 ***********************************************************************************************************************/
void sdmmc_callback(sdmmc_callback_args_t *p_args)
{
    if (SDMMC_EVENT_TRANSFER_COMPLETE & p_args->event)
    {
        transfer_complete = SET_FLAG;
    }

    if (SDMMC_EVENT_RESPONSE & p_args->event)
    {
        g_last_response.status = p_args->response.status;
    }

    if (SDMMC_EVENT_ERASE_COMPLETE & p_args->event)
    {
        g_erase_complete = SET_FLAG;
    }

    if (SDMMC_EVENT_TRANSFER_ERROR & p_args->event)
    {
        transfer_error = SET_FLAG;
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function handles error if error occurred, closes all opened modules, print and traps error.
 *  @param[in]   err        error value
 *  @param[in]   err_str    error string
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* De-initializes SDHI Driver */
        deinit_sdhi_module();

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
