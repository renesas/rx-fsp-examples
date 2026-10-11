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

/* Function declarations */
static fsp_err_t xspi_controller_write_enable(void);
static fsp_err_t xspi_controller_wait_operation(uint32_t timeout);
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
        err = R_XSPI_CONTROLLER_StatusGet(&g_xspi_controller_ctrl, &status);
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
