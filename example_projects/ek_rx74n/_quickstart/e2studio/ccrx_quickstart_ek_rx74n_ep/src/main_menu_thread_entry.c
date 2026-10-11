/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : main_menu_thread_entry.c
 * Version      : 1.0
 * OS           : FreeRTOS
 * Description  : This file includes the main quickstart example code.
 * Operation    : See doc\readme.txt
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2026 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "main_menu_thread.h"
#include "usbserial_console.h"
#include "common_init.h"
#include "common_utils.h"
#include "menu_main.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static char_t s_print_buffer[BUFFER_LINE_LENGTH] =
{0};

#ifdef USE_TO_MANIPLATE_OPTIONATION_AT_FUNCTION_LEVEL

/* Put function between push n pop */
#pragma GCC push_options
#pragma GCC optimize ("O0")
#pragma GCC pop_options

#endif

/**********************************************************************************************************************
 * Function Name: main_menu_thread_entry
 * Description  : main menu thread entry function called by FSP.
 * Argument     : pvParameters
 * Return Value : .
 *********************************************************************************************************************/
void main_menu_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Initialise user level serial console support using SEGGER serial driver DEBUG1 */
    usbserial_console_init ();

    /* Intentional throttling here to stop welcome message from being ignored */
    vTaskDelay(1000);

    sprintf (s_print_buffer, "%s%s%s", gp_clear_screen, gp_cursor_home, gp_hide_cursor);
    print_to_console(s_print_buffer);

    sprintf (s_print_buffer, "\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!\r\n", FULL_NAME);

    sprintf (s_print_buffer, "Configuring connected devices, please wait\r\n");


    common_init();

    /* Each call to main_display_menu() implements the main menu
     * as the code is organised to return to the start after each sub-demo is run. */
    while (1)
    {
        main_display_menu();
        vTaskDelay(100);
    }
}
/**********************************************************************************************************************
 End of function main_menu_thread_entry
 *********************************************************************************************************************/



