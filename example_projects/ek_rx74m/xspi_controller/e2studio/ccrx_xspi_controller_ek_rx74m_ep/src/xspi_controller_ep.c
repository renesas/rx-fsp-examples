/***********************************************************************************************************************
 * File Name    : xspi_controller_ep.c
 * Description  : Contains data structures and functions used in xspi_controller_ep.h
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "xspi_controller_commands.h"
#include "xspi_controller_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup xspi_controller_ep.c
 * @{
 **********************************************************************************************************************/

/* External variables */
extern spi_flash_direct_transfer_t g_xspi_controller_direct_transfer [XSPI_CONTROLLER_TRANSFER_MAX];

/* Global variables */
uint8_t g_read_data [XSPI_CONTROLLER_APP_DATA_SIZE]  = {RESET_VALUE};
const uint8_t g_write_data [XSPI_CONTROLLER_APP_DATA_SIZE] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

/* Function declarations */
static fsp_err_t xspi_controller_write_enable(void);
static fsp_err_t xspi_controller_wait_operation(uint32_t timeout);
static fsp_err_t xspi_controller_erase_operation(uint8_t *const p_address, uint32_t *const p_time);
static fsp_err_t xspi_controller_write_operation(uint8_t *const p_address, uint32_t *const p_time);
static fsp_err_t xspi_controller_read_operation(uint8_t *const p_address, uint32_t *const p_time);
static fsp_err_t timer_start_measure(void);
static fsp_err_t timer_get_measure(uint32_t *p_time);
static fsp_err_t xspi_controller_setup_calibrate_data(void);

/*******************************************************************************************************************//**
 * @brief       This functions enables write and verify the read data.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful operation
 * @retval      FSP_ERR_ABORTED Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t xspi_controller_write_enable(void)
{
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data_u.data    = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };

    /* Transfer write enable command */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_controller_ctrl.spi_protocol)
                   ? g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_WRITE_ENABLE_SPI]
                   : g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_WRITE_ENABLE_OPI];
    err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

    /* Read Status Register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_controller_ctrl.spi_protocol)
                   ? g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_READ_STATUS_SPI]
                   : g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_READ_STATUS_OPI];
    err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

    /* Check Write Enable bit in Status Register */
    if (XSPI_CONTROLLER_WEN_BIT_MASK != (transfer.data_u.data & XSPI_CONTROLLER_WEN_BIT_MASK))
    {
        APP_ERR_RETURN(FSP_ERR_ABORTED, "Write enable FAILED\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function wait until OSPI operation completes.
 * @param[in]   timeout         Maximum waiting time
 * @retval      FSP_SUCCESS     Upon successful wait OSPI operating
 * @retval      FSP_ERR_TIMEOUT Upon time out
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t xspi_controller_wait_operation(uint32_t timeout)
{
    fsp_err_t          err    = FSP_SUCCESS;
    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;

    while (status.write_in_progress)
    {
        /* Get device status */
        R_XSPI_CONTROLLER_StatusGet(&g_xspi_controller_ctrl, &status);
        APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_StatusGet API FAILED\r\n");
        if (RESET_VALUE == timeout)
        {
            APP_ERR_RETURN(FSP_ERR_TIMEOUT, "OSPI time out occurred\r\n");
        }
        R_BSP_SoftwareDelay(1, XSPI_CONTROLLER_TIME_UNIT);
        timeout--;
    }
    return err;
}

/**********************************************************************************************************************
 * @brief       This function performs an erase sector operation on the flash device.
 * @param[in]   *p_address  Pointer to flash device memory address
 * @param[out]  *p_time     Pointer will be used to store execute time
 * @retval      FSP_SUCCESS Upon successful erase operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t xspi_controller_erase_operation(uint8_t * const p_address, uint32_t * const p_time)
{
    fsp_err_t   err             = FSP_SUCCESS;
    uint32_t    sector_size     = RESET_VALUE;
    uint32_t    erase_timeout   = RESET_VALUE;

    sector_size   = XSPI_CONTROLLER_SECTOR_SIZE_4K;
    erase_timeout = XSPI_CONTROLLER_TIME_ERASE_4K;

    /* Start measure */
    err = timer_start_measure();
    APP_ERR_RETURN(err, "timer_start_measure FAILED\r\n");

    /* Performs erase sector */
    err = R_XSPI_CONTROLLER_Erase(&g_xspi_controller_ctrl, p_address, sector_size);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_Erase API FAILED\r\n");

    /* Wait till operation completes */
    err = xspi_controller_wait_operation(erase_timeout);
    APP_ERR_RETURN(err, "xspi_controller_wait_operation FAILED\r\n");

    /* Get execution time */
    err = timer_get_measure(p_time);
    APP_ERR_RETURN(err, "timer_get_measure FAILED\r\n");
    return err;
}

/**********************************************************************************************************************
 * @brief       This function performs an write operation on the flash device.
 * @param[in]   *p_address      Pointer to flash device memory address
 * @param[out]  *p_time     Pointer will be used to store execute time
 * @retval      FSP_SUCCESS Upon successful write operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t xspi_controller_write_operation(uint8_t * const p_address, uint32_t * const p_time)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint32_t    erase_time  = RESET_VALUE;

    /* Erase sector before write data to flash device */
    err = xspi_controller_erase_operation(p_address, &erase_time);
    APP_ERR_RETURN(err, "xspi_controller_erase_operation FAILED\r\n");

    /* Start measure */
    err = timer_start_measure();
    APP_ERR_RETURN(err, "timer_start_measure FAILED\r\n");

    /* Write data to flash device */
    err = R_XSPI_CONTROLLER_Write(&g_xspi_controller_ctrl, g_write_data, p_address, XSPI_CONTROLLER_APP_DATA_SIZE);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_Write API FAILED\r\n");

    /* Wait until write operation completes */
    err = xspi_controller_wait_operation(XSPI_CONTROLLER_TIME_WRITE);
    APP_ERR_RETURN(err, "xspi_controller_wait_operation FAILED\r\n");

    /* Get execution time */
    err = timer_get_measure(p_time);
    APP_ERR_RETURN(err, "timer_get_measure FAILED\r\n");
    return err;
}

/**********************************************************************************************************************
 * @brief       This function performs an read operation on the flash device.
 * @param[in]   *p_address  Pointer to flash device memory address
 * @param[out]  *p_time     Pointer will be used to store execute time
 * @retval      FSP_SUCCESS Upon successful read operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t xspi_controller_read_operation(uint8_t * const p_address, uint32_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clean read buffer */
    memset(&g_read_data, RESET_VALUE, XSPI_CONTROLLER_APP_DATA_SIZE);

    /* Start measure */
    err = timer_start_measure();
    APP_ERR_RETURN(err, "timer_start_measure FAILED\r\n");

    /* Read data from flash device */
    memcpy(&g_read_data, p_address, XSPI_CONTROLLER_APP_DATA_SIZE);

    /* Get execution time */
    err = timer_get_measure(p_time);
    APP_ERR_RETURN(err, "timer_get_measure FAILED\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions initializes OSPI module and Flash device.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful initialization of OSPI module and Flash device
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t xspi_controller_init(void)
{
    /* By default, the flash device is in SPI mode, so it is necessary to open the OSPI module in SPI mode */
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data_u.data    = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };

    /* Open OSPI module */
    err = R_XSPI_CONTROLLER_Open(&g_xspi_controller_ctrl, &g_xspi_controller_cfg);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_Open API FAILED \r\n");

    /* Configure DDR sampling window extend */
    R_XSPI0->LIOCFGCS_b[1].DDRSMPEX = 0x7;

    /* Switch OSPI module to 1S-1S-1S mode to configure flash device */
    err = R_XSPI_CONTROLLER_SpiProtocolSet(&g_xspi_controller_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_SpiProtocolSet API FAILED \r\n");

    /* Reset flash device by driving OM_RESET pin */
    R_XSPI0->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(XSPI_CONTROLLER_TIME_RESET_PULSE, XSPI_CONTROLLER_TIME_UNIT);
    R_XSPI0->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(XSPI_CONTROLLER_TIME_RESET_SETUP, XSPI_CONTROLLER_TIME_UNIT);

    /* Transfer write enable command */
    err = xspi_controller_write_enable();
    APP_ERR_RETURN(err, "xspi_controller_write_enable FAILED \r\n");

    /* Write to ADDR 00000300H of CR2 to configure dummy cycle */
    transfer = g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_WRITE_CR2_300H_SPI];
    transfer.data_u.data = XSPI_CONTROLLER_DATA_SET_CR2_300H;
    err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

    /* Read back and verify CR2 register data */
    transfer = g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_READ_CR2_300H_SPI];
    err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

    if (XSPI_CONTROLLER_DATA_SET_CR2_300H != (uint8_t)transfer.data_u.data)
    {
        APP_ERR_RETURN(FSP_ERR_ABORTED, "Data mismatched in SPI mode \r\n");
    }

    /* Setup calibrate data */
    err = xspi_controller_setup_calibrate_data();
    APP_ERR_RETURN(err, "xspi_controller_setup_calibrate_data FAILED \r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures ospi to extended spi mode.
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful transition to spi operating mode.
 * @retval      FSP_ERR_ABORTED              Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t xspi_controller_set_protocol_to_spi(void)
{
    fsp_err_t                   err      = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data_u.data    = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };
    bsp_octaclk_settings_t      octaclk  =
    {
        .source_clock  = (bsp_clocks_source_t)RESET_VALUE,
        .divider       = (bsp_clocks_octaclk_div_t)RESET_VALUE
    };

    if (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_controller_ctrl.spi_protocol)
    {
        /* Do nothing */
    }
    else if (SPI_FLASH_PROTOCOL_8D_8D_8D == g_xspi_controller_ctrl.spi_protocol)
    {
        /* Transfer write enable command */
        err = xspi_controller_write_enable();
        APP_ERR_RETURN(err, "xspi_controller_write_enable FAILED \r\n");

        /* Change to SPI mode */
        transfer = g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_WRITE_CR2_000H_OPI];
        transfer.data_u.data = XSPI_CONTROLLER_DATA_SET_SPI_CR2_000H;
        err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

        /* Change the OCTACLK clock to 100 MHz in SDR mode without OM_DQS */
        octaclk.source_clock = (bsp_clocks_source_t)BSP_CLOCKS_SOURCE_CLOCK_PLL1Q;
        octaclk.divider      = (bsp_clocks_octaclk_div_t)BSP_CLOCKS_OCTA_CLOCK_DIV_2;

        R_BSP_OctaclkUpdate(&octaclk);
        /* Switch OSPI module mode to SPI mode */
        err = R_XSPI_CONTROLLER_SpiProtocolSet(&g_xspi_controller_ctrl, SPI_FLASH_PROTOCOL_EXTENDED_SPI);
        APP_ERR_RETURN(err, "R_OSPI_SpiProtocolSet API FAILED \r\n");

        /* Read back and verify CR2 register data */
        transfer = g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_READ_CR2_000H_SPI];
        err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

        if (XSPI_CONTROLLER_DATA_SET_SPI_CR2_000H != (uint8_t)transfer.data_u.data)
        {
            APP_ERR_RETURN(FSP_ERR_ABORTED, "Data mismatched in SPI mode \r\n");
        }
    }
    else
    {
        APP_ERR_RETURN(FSP_ERR_INVALID_MODE, "Flash device do not support this mode\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures xspi controller to opi mode.
 * @param[in]   None
 * @retval      FSP_SUCCESS                  Upon successful transition to opi operating mode.
 * @retval      FSP_ERR_ABORTED              Upon incorrect read data.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t xspi_controller_set_protocol_to_opi(void)
{
    fsp_err_t                   err      = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data_u.data    = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };
    bsp_octaclk_settings_t      octaclk  =
    {
        .source_clock  = (bsp_clocks_source_t)RESET_VALUE,
        .divider       = (bsp_clocks_octaclk_div_t)RESET_VALUE
    };
    if (SPI_FLASH_PROTOCOL_8D_8D_8D == g_xspi_controller_ctrl.spi_protocol)
    {
        /* Do nothing */
    }
    else if (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_controller_ctrl.spi_protocol)
    {
        /* Transfer write enable command */
        err = xspi_controller_write_enable();
        APP_ERR_RETURN(err, "xspi_controller_write_enable FAILED \r\n");

        /* Change to DOPI mode */
        transfer = g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_WRITE_CR2_000H_SPI];
        transfer.data_u.data = XSPI_CONTROLLER_DATA_SET_OPI_CR2_000H;
        err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

        /* Change the OCTACLK clock to 133 MHz in DDR mode with OM_DQS */
        octaclk.source_clock = (bsp_clocks_source_t)BSP_CLOCKS_SOURCE_CLOCK_PLL2P;
        octaclk.divider = (bsp_clocks_octaclk_div_t)BSP_CLOCKS_OCTA_CLOCK_DIV_2;

        R_BSP_OctaclkUpdate(&octaclk);
        /* Switch OSPI module mode to SPI mode */
        err = R_XSPI_CONTROLLER_SpiProtocolSet(&g_xspi_controller_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
        APP_ERR_RETURN(err, "R_OSPI_SpiProtocolSet API FAILED \r\n");

        /* Read back and verify CR2 register data */
        transfer = g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_READ_CR2_000H_OPI];
        err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

        if (XSPI_CONTROLLER_DATA_SET_OPI_CR2_000H != (uint8_t)transfer.data_u.data)
        {
            APP_ERR_RETURN(FSP_ERR_ABORTED, "Data mismatched in OPI mode \r\n");
        }
    }
    else
    {
        APP_ERR_RETURN(FSP_ERR_INVALID_MODE, "Flash device do not support this mode\r\n");
    }
    return err;
}

/**********************************************************************************************************************
 * @brief       This function reads flash device id
 * @param[out]  *p_device_id        Pointer will be used to store device id
 * @retval      FSP_SUCCESS         Upon successful direct transfer operation
 * @retval      FSP_ERR_ABORTED     On incorrect device id read.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t xspi_controller_read_device_id (uint32_t * const p_id)
{
    fsp_err_t                   err             = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer        =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data_u.data    = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };

    /* Read and check flash device ID */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_controller_ctrl.spi_protocol)
                   ? g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_READ_DEVICE_ID_SPI]
                   : g_xspi_controller_direct_transfer[XSPI_CONTROLLER_TRANSFER_READ_DEVICE_ID_OPI];
    err = R_XSPI_CONTROLLER_DirectTransfer(&g_xspi_controller_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

    if (XSPI_CONTROLLER_DEVICE_ID != transfer.data_u.data)
    {
        APP_ERR_RETURN(FSP_ERR_ABORTED, "Device ID is incorrect\r\n");
    }

    /* Get flash device ID */
    *p_id = transfer.data_u.data;
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function performs OSPI operations according to the selections made by the user.
 * @param[in]   *p_address      Pointer to flash device memory
 * @retval      FSP_SUCCESS     Upon successful OSPI operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t xspi_controller_operation (uint8_t * p_address)
{
    fsp_err_t       err                         = FSP_SUCCESS;
    uint32_t        execute_time                = RESET_VALUE;
    char            rtt_read [BUFFER_SIZE_DOWN] = {RESET_VALUE};
    const uint8_t   spi_mode_string []          = "1S-1S-1S Protocol Mode";
    const uint8_t   opi_mode_string []          = "8D-8D-8D Protocol Mode";
    uint8_t const   * p_mode_string             = NULL;

    /* Get OSPI mode name of current OSPI instance */
    p_mode_string = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_controller_ctrl.spi_protocol)
                        ? spi_mode_string
                        : opi_mode_string;

    /* Print Sub Menu option */
    APP_PRINT(SUB_MENU, p_mode_string);

    /* Loop */
    while(true)
    {
        if(APP_CHECK_DATA)
        {
            /* Clean Terminal Application input buffer */
            memset(rtt_read, RTT_NULL_CHAR, BUFFER_SIZE_DOWN);

            /* Get Terminal Application input buffer */
            APP_READ(rtt_read);

            /* Handle Terminal Application input */
            switch(rtt_read[RTT_CHECK_INDEX])
            {
                case RTT_SELECT_WRITE_OPERATION_CHAR:
                    /* Performs OSPI write operation */
                    err = xspi_controller_write_operation (p_address, &execute_time);
                    APP_ERR_RETURN(err, "xspi_controller_write_operation FAILED\r\n");

                    /* Print execution time */
                    APP_PRINT("\r\n%s : Write %d bytes completed successfully in %d nanoseconds\r\n"
                            , p_mode_string, (uint32_t)(XSPI_CONTROLLER_APP_DATA_SIZE), execute_time);
                    break;

                case RTT_SELECT_READ_OPERATION_CHAR:
                    /* Performs OSPI read operation */
                    err = xspi_controller_read_operation (p_address, &execute_time);
                    APP_ERR_RETURN(err, "xspi_controller_read_operation FAILED\r\n");

                    /* Print execution time */
                    APP_PRINT("\r\n%s : Read %d bytes completed successfully in %d nanoseconds\r\n"
                            , p_mode_string, (uint32_t)(XSPI_CONTROLLER_APP_DATA_SIZE), execute_time);

                    /* Compare data read and data written */
                    if(RESET_VALUE == memcmp(&g_read_data, &g_write_data, (size_t)XSPI_CONTROLLER_APP_DATA_SIZE))
                    {
                        APP_PRINT("%s : Data read matched data written\r\n", p_mode_string);
                    }
                    else
                    {
                        APP_PRINT("%s : Data read does not match data written\r\n", p_mode_string);
                    }
                    break;

                case RTT_SELECT_ERASE_OPERATION_CHAR:
                    /* Performs OSPI erase operation */
                    err = xspi_controller_erase_operation(p_address, &execute_time);
                    APP_ERR_RETURN(err, "xspi_controller_erase_operation FAILED\r\n");

                    /* Print execution time */
                    APP_PRINT("\r\n%s : Erase sector completed successfully in %d nanoseconds\r\n",
                              p_mode_string, execute_time);
                    break;

                case RTT_EXIT_SUB_MENU_CHAR:
                    APP_PRINT ("\r\nGo back to Main Menu\r\n");
                    return err;

                default:
                    APP_PRINT ("\r\nInvalid input. Provide a valid input\r\n");
                    break;
            }

            /* Print Sub Menu option */
            APP_PRINT(SUB_MENU, p_mode_string);
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function starts GPT module to measure execution time of an OSPI operation.
 * @param       None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_start_measure(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear timer counter */
    err = R_GPT_Reset(&g_timer_ctrl);
    APP_ERR_RETURN(err, "R_GPT_Reset FAILED\r\n");

    /* Start timer */
    err = R_GPT_Start(&g_timer_ctrl);
    APP_ERR_RETURN(err, "R_GPT_Start FAILED\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This Function measures the timing info by reading the timer.
 * @param[in]   *p_time     Pointer will be used to store the OSPI operation execution time
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_get_measure(uint32_t * p_time)
{
    fsp_err_t       err             = FSP_SUCCESS;
    timer_status_t  timer_status    =
    {
        .counter        = RESET_VALUE,
        .state          = (timer_state_t)RESET_VALUE
    };
    timer_info_t    timer_info      =
    {
        .clock_frequency = RESET_VALUE,
        .count_direction = (timer_direction_t)RESET_VALUE,
        .period_counts   = RESET_VALUE
    };

    /* Get status of timer */
    err = R_GPT_StatusGet(&g_timer_ctrl, &timer_status);
    APP_ERR_RETURN(err, "R_GPT_StatusGet FAILED\r\n");

    /* Get info of timer */
    err = R_GPT_InfoGet(&g_timer_ctrl, &timer_info);
    APP_ERR_RETURN(err, "R_GPT_InfoGet FAILED\r\n");

    /* Stop timer */
    err = R_GPT_Stop(&g_timer_ctrl);
    APP_ERR_RETURN(err, "R_GPT_Stop FAILED\r\n");

    /* Convert count value to nanoseconds unit */
    *p_time = (timer_status.counter * 100) / (timer_info.clock_frequency / 10000000);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions initializes GPT module used to measure OSPI operation execution time.
 * @param       None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t timer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    APP_ERR_RETURN(err, "R_GPT_Open FAILED\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function sets up the auto-calibrate data for the flash.
 * @param       None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t xspi_controller_setup_calibrate_data(void)
{
    fsp_err_t err = FSP_SUCCESS;
    xspi_controller_extended_cfg_t * p_extended_cfg = (xspi_controller_extended_cfg_t *)g_xspi_controller_cfg.p_extend;

    uint32_t g_autocalibration_data[] =
    {
        0xFFFF0000U,
        0x0800FF00U,
        0xFF0000F7U,
        0x00F708F7U
    };

    /* Verify auto-calibration data */
    if (RESET_VALUE != memcmp((uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr,
                              &g_autocalibration_data, sizeof(g_autocalibration_data)))
    {
        /* Erase the flash sector that stores auto-calibration data */
        err = R_XSPI_CONTROLLER_Erase(&g_xspi_controller_ctrl,
                             (uint8_t *)XSPI_CONTROLLER_APP_ADDRESS(XSPI_CONTROLLER_SECTOR_THREE), XSPI_CONTROLLER_SECTOR_SIZE_4K);
        APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_Erase API FAILED \r\n");

        /* Wait until erase operation completes */
        err = xspi_controller_wait_operation(XSPI_CONTROLLER_TIME_ERASE_4K);
        APP_ERR_RETURN(err, "xspi_controller_wait_operation FAILED\r\n");

        /* Write auto-calibration data to the flash */
        err = R_XSPI_CONTROLLER_Write(&g_xspi_controller_ctrl, (uint8_t *)&g_autocalibration_data,
                             (uint8_t *)p_extended_cfg->p_autocalibration_preamble_pattern_addr,
                             sizeof(g_autocalibration_data));
        APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_Write API FAILED \r\n");

        /* Wait until write operation completes */
        err = xspi_controller_wait_operation(XSPI_CONTROLLER_TIME_WRITE);
        APP_ERR_RETURN(err, "xspi_controller_wait_operation FAILED\r\n");
    }
    __NOP();
    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup xspi_controller_ep.c)
 **********************************************************************************************************************/
