/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : menu_ns.c
 * Version      : 1.0
 * Description  : The next steps screen display.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2026 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "usbserial_console.h"
#include "common_init.h"
#include "common_utils.h"
#include "menu_ns.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. NEXT STEPS\r\n"

#define SUB_OPTIONS     "\r\nVisit the following URLs to learn about the kit and the RX family of" \
                        "\r\nMCUs, download tools and documentation, and get support." \
                        "\r\n" \
                        "\r\n a) %s Resources:                  renesas.com/%s " \
                        "\r\n b) RX Kits Information:                 renesas.com/rx/kits" \
                        "\r\n c) RX Product Information:              renesas.com/rx" \
                        "\r\n d) RX Product Support Forum:            renesas.com/rx/forum" \
                        "\r\n e) RX Videos:                           renesas.com/rx/videos" \
                        "\r\n f) RX Kit Feedback and Feature Request: renesas.com/rx/kitfeedback" \
                        "\r\n g) Renesas Support:                     renesas.com/support" \

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

/**********************************************************************************************************************
 * Function Name: ns_display_menu
 * Description  : The next steps screen.
 * Return Value : Default 0, used for possible expansion.
 *********************************************************************************************************************/
test_fn ns_display_menu(void)
{
    int8_t c             = -1;
    char_t fullname[]    = FULL_NAME;
    bool_t new_mode      = false;

    volatile bool_t mode = true;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(s_print_buffer);

    tolowerstr(&fullname[0], strlen(FULL_NAME));
    sprintf (s_print_buffer, SUB_OPTIONS, FULL_NAME, fullname);
    print_to_console(s_print_buffer);

    sprintf (s_print_buffer, MENU_RETURN_INFO);
    print_to_console(s_print_buffer);


    start_key_check();

    new_mode = usbserial_set_mode(mode);

    start_key_check();

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if (key_pressed())
        {
            /* Cast, as compiler will assume c is int */
            c = (int8_t)get_detected_key();

            if (0 == c)
            {
                c = -1;
            }

            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                break;
            }
            start_key_check();
        }
        vTaskDelay(10);
    }
    usbserial_restore_mode(new_mode);
    return (0);
}
/**********************************************************************************************************************
 End of function ns_display_menu
 *********************************************************************************************************************/

