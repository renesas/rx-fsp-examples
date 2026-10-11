/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "sci_i2c_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup sci_i2c_ep
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);

extern bsp_leds_t g_bsp_leds;

/*******************************************************************************************************************//**
 * The RX Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t          err        = FSP_SUCCESS;
    fsp_pack_version_t version    = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                       (bsp_io_level_t) BSP_IO_LEVEL_LOW);
#if (USE_VIRTUAL_COM == 1)
    err = (fsp_err_t) TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON error LED to indicate uart_init failed */
        R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                           (bsp_io_level_t) BSP_IO_LEVEL_HIGH);
        /* Error trap */
        ERROR_TRAP;
    }
#endif
    /* Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
            version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /*  SCI_B_I2C master and IIC slave driver initialization */
    err = init_i2c_driver();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* SCI_B_I2C Master and IIC Slave initialization failed  */
        APP_ERR_PRINT("\r\n ** IIC driver initialization FAILED ** \r\n");

        /* Turn ON LED */
        set_led(LED_ON);
        APP_ERR_TRAP(err);
    }
    while(true)
    {
        /* Performs SCI_B_I2C write/read operation */
        err = process_master_WriteRead();
        /*
         * Perform clean_up - On SCI_B_I2C Master write/read operation failure
         * Note: For Demonstration Purpose the failure ends up with TRAP.
         * Note: This can be handled in many ways as per the Application needs
         */

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* Turn on LED */
            set_led(LED_ON);

            /* Print Terminal Application message */
            APP_ERR_PRINT ("\r\n **SCI_B_I2C Master transaction Failed, closing open modules ** \r\n");

            /* De-initialize opened SCI_B_I2C master and IIC slave modules */
            deinit_i2c_driver();

            APP_ERR_TRAP(err);
        }
		/* 1 seconds Wait time to recognize LED toggling between successive Write and Read operations. */
        R_BSP_SoftwareDelay(DELAY_OPERATION, BSP_DELAY_UNITS_SECONDS);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup sci_i2c_ep)
 **********************************************************************************************************************/
