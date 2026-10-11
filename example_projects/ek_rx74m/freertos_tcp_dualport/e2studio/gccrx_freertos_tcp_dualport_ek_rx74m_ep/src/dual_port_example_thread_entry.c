/*******************************************************************************************************************//**
 * File Name    : dual_port_example_thread_entry.c
 * Description  : This file contains the entry function for the dual port Ethernet example thread.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <dual_port_example.h>
#include "common_utils.h"
/* Global Variables */
extern bsp_leds_t g_bsp_leds;

void dual_port_example_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_pack_version_t version = {RESET_VALUE};

    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    fsp_err_t err              = FSP_SUCCESS;

#if (USE_VIRTUAL_COM == 1)
    err = (fsp_err_t) TERM_INIT();
        if (FSP_SUCCESS != err)
        {
            /* Turn ON error LED to indicate uart_init failed */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                              (bsp_io_level_t) BSP_IO_LEVEL_HIGH);
            /* Error trap */
            ERROR_TRAP;
        }
#endif

    /* Example project information printed on the RTT */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);

    APP_PRINT(EP_INFO);

    vInitializeNetworkInterfaces();

    while (1)
    {
        vTaskDelay (1);
    }
}
