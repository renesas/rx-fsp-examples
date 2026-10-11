/*
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/***********************************************************************************************************************
 * File Name    : xspi_ep.c
 * Version      : 1.0
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2026 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_init.h"
#include "common_utils.h"
#include "xspi_commands.h"
#include "xspi_ep.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
extern spi_flash_direct_transfer_t g_xspi_direct_transfer[XSPI_TRANSFER_MAX];

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static fsp_err_t setup_calibrate_data (void);
static fsp_err_t set_protocol_to_opi (void);
static fsp_err_t set_dummy_cycle (void);
static fsp_err_t write_enable (void);

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: xspi_init
 * Description  : This functions initializes XSPI module and Flash device.
 * Argument     :
 * Return Value : FSP_SUCCESS     Upon successful initialization of XSPI module and Flash device
 *                Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 *********************************************************************************************************************/
fsp_err_t xspi_init()
{
    fsp_err_t              err      = FSP_SUCCESS;
    bsp_octaclk_settings_t octaclk  =
    {
        .source_clock  = (bsp_clocks_source_t)RESET_VALUE,
        .divider       = (bsp_clocks_octaclk_div_t)RESET_VALUE
    };

    octaclk.source_clock = (bsp_clocks_source_t)BSP_CLOCKS_SOURCE_CLOCK_PLL1R;
    octaclk.divider      = (bsp_clocks_octaclk_div_t)BSP_CLOCKS_OCTA_CLOCK_DIV_4;

    R_BSP_OctaclkUpdate(&octaclk);

    do
    {

        err = R_XSPI_CONTROLLER_Open (&g_xspi_ctrl, &g_xspi_cfg);
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Switch XSPI module to 1S-1S-1S mode to configure flash device */
        err = R_XSPI_CONTROLLER_SpiProtocolSet (&g_xspi_ctrl, SPI_FLASH_PROTOCOL_1S_1S_1S);
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Reset flash device by driving OM_RESET pin */
        R_XSPI0->LIOCTL_b.RSTCS0 = 0;
        R_BSP_SoftwareDelay (XSPI_TIME_RESET_SETUP, XSPI_TIME_UNIT);
        R_XSPI0->LIOCTL_b.RSTCS0 = 1;
        R_BSP_SoftwareDelay (XSPI_TIME_RESET_PULSE, XSPI_TIME_UNIT);

        err = set_dummy_cycle ();
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Setup calibrate data */
        err = setup_calibrate_data ();
        if (FSP_SUCCESS != err)
        {
            break;
        }

        err = set_protocol_to_opi ();
        if (FSP_SUCCESS != err)
        {

            break;
        }

    }
    while (0);

    return err;
}
/**********************************************************************************************************************
 End of function xspi_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: xspi_wait_operation
 * Description  : This function wait until OSPI operation completes.
 * Argument     :
 * Return Value : FSP_SUCCESS     Upon successful operation
 *                FSP_ERR_TIMEOUT Upon time out
 *********************************************************************************************************************/
fsp_err_t xspi_wait_operation(uint32_t timeout)
{
    fsp_err_t          err    = FSP_SUCCESS;
    spi_flash_status_t status =
    { RESET_VALUE };

    status.write_in_progress = true;
    while (status.write_in_progress)
    {
        /* Get device status */
        R_XSPI_CONTROLLER_StatusGet (&g_xspi_ctrl, &status);

        if (RESET_VALUE == timeout)
        {
            err = FSP_ERR_TIMEOUT;
            break;
        }

        R_BSP_SoftwareDelay (1, XSPI_TIME_UNIT);
        timeout--;
    }

    return err;
}
/**********************************************************************************************************************
 End of function xspi_wait_operation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_protocol_to_opi
 * Description  : This function configures ospi to opi mode.
 * Argument     :
 * Return Value : FSP_SUCCESS                  Upon successful transition to opi operating mode.
 *                FSP_ERR_ABORTED              Upon incorrect read data.
 *                Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t set_protocol_to_opi()
{
    fsp_err_t                   err      = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    { RESET_VALUE };
    bsp_octaclk_settings_t      octaclk  =
    {
        .source_clock  = (bsp_clocks_source_t)RESET_VALUE,
        .divider       = (bsp_clocks_octaclk_div_t)RESET_VALUE
    };

    do
    {
        /* Transfer write enable command */
        err = write_enable ();
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Add delay for change DOPI mode */
        R_BSP_SoftwareDelay (500, BSP_DELAY_UNITS_MILLISECONDS);

        transfer             = g_xspi_direct_transfer[XSPI_TRANSFER_WRITE_CR2_000H_SPI];
        transfer.data_u.data = XSPI_DATA_SET_OPI_CR2_000H;

        err = R_XSPI_CONTROLLER_DirectTransfer (&g_xspi_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            break;
        }

        octaclk.source_clock = (bsp_clocks_source_t)BSP_CLOCKS_SOURCE_CLOCK_PLL2P;
        octaclk.divider      = (bsp_clocks_octaclk_div_t)BSP_CLOCKS_OCTA_CLOCK_DIV_1;

        R_BSP_OctaclkUpdate(&octaclk);

        err = R_XSPI_CONTROLLER_SpiProtocolSet (&g_xspi_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
        if (FSP_SUCCESS != err)
        {
            break;
        }

    }
    while (0);

    return err;
}
/**********************************************************************************************************************
 End of function set_protocol_to_opi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: setup_calibrate_data
 * Description  : This function sets up the auto-calibrate data for the flash.
 * Argument     :
 * Return Value : FSP_SUCCESS     Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t setup_calibrate_data(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Cast to xspi_controller_extended_cfg_t * */
    xspi_controller_extended_cfg_t * p_extended_cfg = (xspi_controller_extended_cfg_t *)g_xspi_cfg.p_extend;

    uint32_t g_autocalibration_data[] =
    { 0xFFFF0000U, 0x0800FF00U, 0xFF0000F7U, 0x00F708F7U };

    /* Verify auto-calibration data */
    if (RESET_VALUE != memcmp ((uint8_t*)p_extended_cfg->p_autocalibration_preamble_pattern_addr,
                               &g_autocalibration_data, sizeof(g_autocalibration_data)))
    {

        do
        {

            /* Erase the flash sector that stores auto-calibration data */
            err = R_XSPI_CONTROLLER_Erase (&g_xspi_ctrl,
                                           (uint8_t*)p_extended_cfg->p_autocalibration_preamble_pattern_addr, XSPI_SECTOR_SIZE);
            if (FSP_SUCCESS != err)
            {
                break;
            }

            /* Wait until erase operation completes */
            err = xspi_wait_operation (XSPI_TIME_ERASE_4K);
            if (FSP_SUCCESS != err)
            {
                break;
            }

            /* Write auto-calibration data to the flash */
            err = R_XSPI_CONTROLLER_Write (&g_xspi_ctrl, (uint8_t*)&g_autocalibration_data,
                                           (uint8_t*) p_extended_cfg->p_autocalibration_preamble_pattern_addr,
                                           sizeof(g_autocalibration_data));
            if (FSP_SUCCESS != err)
            {
                break;
            }

            /* Wait until write operation completes */
            err = xspi_wait_operation (XSPI_TIME_WRITE);
            if (FSP_SUCCESS != err)
            {
                break;
            }

        }
        while (0);
    }

    return err;
}
/**********************************************************************************************************************
 End of function setup_calibrate_data
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_dummy_cycle
 * Description  : This function set dummy cycle register of flash device
 * Argument     :
 * Return Value : FSP_SUCCESS     Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t set_dummy_cycle(void)
{
    volatile fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    { RESET_VALUE };

    do
    {
        err = write_enable ();
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Transfer write enable command */
        transfer = (SPI_FLASH_PROTOCOL_1S_1S_1S == g_xspi_ctrl.spi_protocol) ?
                g_xspi_direct_transfer[XSPI_TRANSFER_WRITE_CR2_300H_SPI] :
                g_xspi_direct_transfer[XSPI_TRANSFER_WRITE_CR2_300H_OPI];

        transfer.data_u.data = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_ctrl.spi_protocol) ?
                XSPI_DATA_SET_SPI_CR2_300H :
                XSPI_DATA_SET_OPI_CR2_300H;
        err = R_XSPI_CONTROLLER_DirectTransfer (&g_xspi_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Read Status Register */
        transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_ctrl.spi_protocol) ?
                g_xspi_direct_transfer[XSPI_TRANSFER_READ_CR2_300H_SPI] :
                g_xspi_direct_transfer[XSPI_TRANSFER_READ_CR2_300H_OPI];
        err = R_XSPI_CONTROLLER_DirectTransfer (&g_xspi_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Check dummy cycle bit in Status Register */
        if (SPI_FLASH_PROTOCOL_1S_1S_1S == g_xspi_ctrl.spi_protocol)
        {
            if (XSPI_DATA_SET_SPI_CR2_300H != (transfer.data_u.data & XSPI_MASK_CR2_300H))
            {
                err = FSP_ERR_NOT_ENABLED;
                break;
            }
        }
        else
        {
            if (XSPI_DATA_SET_OPI_CR2_300H != (transfer.data_u.data & XSPI_MASK_CR2_300H))
            {
                err = FSP_ERR_NOT_ENABLED;
                break;
            }
        }

    }
    while (0);

    return err;
}
/**********************************************************************************************************************
 End of function set_dummy_cycle
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: write_enable
 * Description  : This functions enables write and verify the read data.
 * Argument     :
 * Return Value : FSP_SUCCESS     Upon successful operation
 *                FSP_ERR_ABORTED Upon incorrect read data.
 *                Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t write_enable()
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    { RESET_VALUE };

    do
    {
        /* Transfer write enable command */
        transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_ctrl.spi_protocol) ?
                g_xspi_direct_transfer[XSPI_TRANSFER_WRITE_ENABLE_SPI] :
                g_xspi_direct_transfer[XSPI_TRANSFER_WRITE_ENABLE_OPI];
        err = R_XSPI_CONTROLLER_DirectTransfer (&g_xspi_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Read Status Register */
        transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == g_xspi_ctrl.spi_protocol) ?
                g_xspi_direct_transfer[XSPI_TRANSFER_READ_STATUS_SPI] :
                g_xspi_direct_transfer[XSPI_TRANSFER_READ_STATUS_OPI];
        err = R_XSPI_CONTROLLER_DirectTransfer (&g_xspi_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            break;
        }

        /* Check Write Enable bit in Status Register */
        if (!(XSPI_DATA_SR_WRITE_ENABLE & transfer.data_u.data))
        {
            err = FSP_ERR_NOT_ENABLED;
        }
    }
    while (0);

    return err;
}
/**********************************************************************************************************************
 End of function write_enable
 *********************************************************************************************************************/
