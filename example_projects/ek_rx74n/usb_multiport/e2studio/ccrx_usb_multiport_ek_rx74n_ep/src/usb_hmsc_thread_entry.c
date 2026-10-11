/***********************************************************************************************************************
 * File Name    : usb_hmsc_thread_entry.c
 * Description  : Contains entry function of FreeRTOS Plus FAT with USB HMSC.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_hmsc_thread.h"
#include "common_utils.h"
#include "usb_multiport.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_multiport_ep
 * @{
 **********************************************************************************************************************/

/* Extern global variable */
extern uint8_t g_write_data[WRITE_ITEM_SIZE];
extern bsp_leds_t g_bsp_leds;

/*******************************************************************************************************************//**
 * @brief     Entry function of USB HMSC thread
 * @param[IN] pointer to pvParameters
 * @retval    None
 **********************************************************************************************************************/
void usb_hmsc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };
    BaseType_t err_queue = pdFALSE;

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

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major,\
              version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize file system and USB */
    err = file_system_and_usb_init();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nError in initializing FreeRTOS+FAT with USB_HMSC.\r\n");
        APP_ERR_TRAP (err);
    }

    /* Print USB operation menu */
    APP_PRINT (MENU);

    while (true)
    {
        uint8_t rtt_input_buf[BUFFER_SIZE_DOWN] = { RESET_VALUE };
        uint8_t converted_rtt_input = RESET_VALUE;
        /* Read RTT input from the user */
        if (APP_CHECK_DATA)
        {
            APP_READ(rtt_input_buf);
            converted_rtt_input = (uint8_t) atoi ((char*) rtt_input_buf);
            process_usb_operation (converted_rtt_input);
        }
        /* Check for the USB connection status */
        if (true == check_usb_connected ())
        {
            /* Data received from PCDC thread through queue */
            err_queue = xQueueReceive (g_queue_data_from_pcdc, g_write_data, (TickType_t) (RESET_VALUE));
            if (pdTRUE == err_queue)
            {
                APP_PRINT("\r\nData entered from PCDC is being written to the file.\r\n");
                usb_write_operation ();
                APP_PRINT (MENU);
            }
        }
        vTaskDelay (1);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_multiport_ep)
 **********************************************************************************************************************/
