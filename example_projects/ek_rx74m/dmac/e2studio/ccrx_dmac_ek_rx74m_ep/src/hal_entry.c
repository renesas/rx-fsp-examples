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
#include "dmac_transfers.h"
#include "timer_initialise.h"
#include "transfer_initialise.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

extern bsp_leds_t g_bsp_leds;

/*******************************************************************************************************************//**
 * The RX Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;                /* Variable to help handle error codes from functions */
    fsp_pack_version_t version = { RESET_VALUE };   /* FSP variable to store major/minor versions */

    /* Array to store input from Terminal Application */
    char rtt_input_data[TERM_BUFFER_SIZE] = { RESET_VALUE };
    uint8_t converted_rtt_input = RESET_VALUE;

    /* Fetch FSP version to display on Terminal Application */
    R_FSP_VersionGet (&version);
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                     (bsp_io_level_t)BSP_IO_LEVEL_LOW);
#if (USE_VIRTUAL_COM == 1)
    fsp_err = (fsp_err_t) TERM_INIT();
    if (FSP_SUCCESS != fsp_err)
    {
        /* Turn ON error LED to indicate uart_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                          (bsp_io_level_t) BSP_IO_LEVEL_HIGH);
        /* Error trap */
        ERROR_TRAP;
    }
#endif
    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO,EP_VERSION,version.version_id_b.major, version.version_id_b.minor,
            version.version_id_b.patch);

    APP_PRINT(EP_INFO);

    /* DMAC g_transfer_gpt_timer is configured to transfer 32-bit data
     * from a GPT counter register to the destination array. DMAC
     * transfer happens when 2 is pressed. GPT timer continuous to run. */
    transfer_gpt_timer_operation();

    /* DMAC g_transfer_write_ioport is configured to transfer 32-bit
     * data(in repeat mode) from the source array to port control
     * register(LED). As a  result, when 3 is pressed, LED state
     * changes */
    transfer_write_ioport_operation();

    APP_PRINT("\r\n*** The GPT timer value will be updated in Round-Robin fashion. ***\r\n");
    APP_PRINT(MENU);

    while(true)
    {
        /* Check for input data from Terminal Application, once data is available read. */
        if (APP_CHECK_DATA)
        {
            memset(rtt_input_data, RESET_VALUE , sizeof(rtt_input_data));
            /* Reading complete input instead of 1 byte to avoid processing of
             * successive extra bytes entered */
            APP_READ(rtt_input_data);
            /* Considering both input sending type from Terminal Application and checking for valid input */
            if((NEW_LINE != rtt_input_data[EXPECTED_CHAR_END_INDEX])
               && (CARRIAGE_RETURN != rtt_input_data[EXPECTED_CHAR_END_INDEX])
               && (NULL_CHAR != rtt_input_data[EXPECTED_CHAR_END_INDEX]))
            {
                APP_PRINT("\r\nProvide a valid input.\n");
            }
            else
            {
                converted_rtt_input = (uint8_t)atoi((char *)rtt_input_data);
                if( START_TRANSFER_ON_LED_PORT == converted_rtt_input )
                {
                    /* DMAC g_transfer_led_blink (runs in Normal mode) transfers data
                     * from source array to port control register(LED) for specified
                     * number of counts(60).DMAC g_transfer_led_blink alters the state
                     * of the LED and stops. */
                    transfer_led_blink_operation();

                    /* De-initialize g_transfer_led_blink */
                    dmac_transfer_deinit(&g_transfer_led_blink_ctrl, TRANSFER_LED_BLINK);
                    /* De-initialize CMT */
                    cmt_timer_deinit();
                }
                /* Check for user input to transfer current gpt value to destination array */
                else if(START_TRANSFER_GPT_VALUE == converted_rtt_input)
                {
                    /* Start DMAC transfer from gpt counter register to destination array */
                    fsp_err = dmac_transfer_software_start(&g_transfer_gpt_value_ctrl);
                    /* Handle error in-case of failure */
                    if (FSP_SUCCESS != fsp_err)
                    {
                        dmac_transfer_deinit(&g_transfer_write_ioport_ctrl, TRANSFER_WRITE_IOPORT);
                        gpt_timer_deinit();
                        dmac_transfer_deinit(&g_transfer_gpt_value_ctrl, TRANSFER_GPT_TIMER_VALUE);
                        cmt_timer_deinit();
                        APP_ERR_TRAP(fsp_err);
                    }
                    dmac_transfer_print_data();
                }
                else if(START_TRANSFER_TOGGLE_LED == converted_rtt_input)
                {
                    /* Start the dmac transfer_write_ioport */
                    fsp_err = dmac_transfer_software_start(&g_transfer_write_ioport_ctrl);
                    /* Handle error in-case of failure */
                    if (FSP_SUCCESS != fsp_err)
                    {
                        dmac_transfer_deinit(&g_transfer_write_ioport_ctrl, TRANSFER_WRITE_IOPORT);
                        gpt_timer_deinit();
                        dmac_transfer_deinit(&g_transfer_gpt_value_ctrl, TRANSFER_GPT_TIMER_VALUE);
                        cmt_timer_deinit();
                        APP_ERR_TRAP(fsp_err);
                    }
                    APP_PRINT("\r\n Data successfully transferred on LED PORT\r\n");
                }
                else
                {
                    APP_PRINT("\r\nProvide a valid input.\n");
                }
                APP_PRINT(MENU);
            }
        }
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
         * C runtime initialization, should negate the need for a delay since the initialization will typically
         * take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}
