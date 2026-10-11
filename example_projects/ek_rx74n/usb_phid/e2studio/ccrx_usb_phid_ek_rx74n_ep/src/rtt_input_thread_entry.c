/***********************************************************************************************************************
* File Name    : rtt_input_thread_entry.c
* Description  : Contains functions from the terminal input thread.
***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "rtt_input_thread.h"
#include "common_utils.h"
#include "usb_phid_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_phid_ep
 * @{
 **********************************************************************************************************************/

/* Global variable */
bool g_write_flag = false;
extern bsp_leds_t g_bsp_leds;

/*******************************************************************************************************************//**
 * @brief       Terminal input thread entry.
 * @param[IN]   pvParameters
 * @retval      None
 **********************************************************************************************************************/
void rtt_input_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    char rtt_buff[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    fsp_pack_version_t version      = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

#if (USE_VIRTUAL_COM == 1)
    fsp_err_t err                   = FSP_SUCCESS;
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

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);

    /* Prints EP info on terminal */
    APP_PRINT(EP_INFO);

    APP_PRINT("\r\nEnter any key from terminal to write data\r\n");

    while (true)
    {
        /* Check for user input */
        if (APP_CHECK_DATA)
        {
            APP_READ(rtt_buff);

            APP_PRINT("\r\nNOTE: Please open Notepad\r\n");

            /* Delay for user to open Notepad */
            vTaskDelay(DELAY_3_SEC / portTICK_PERIOD_MS);

            /* Set write flag */
            g_write_flag = true;

            APP_PRINT("\r\nEnter any key from terminal to write data\r\n");
        }
        vTaskDelay(TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_phid_ep)
 **********************************************************************************************************************/
