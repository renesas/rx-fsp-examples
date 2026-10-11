/*
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**********************************************************************************************************************
 * File Name    : menu_ext.c
 * Version      : 1.0
 * Description  : External memory demo implementation.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2026 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "menu_ext.h"
#include "hal_data.h"
#include "board_hw_cfg.h"
#include "usbserial_console.h"
#include "xspi_ep.h"
#include "common_init.h"
#include "common_utils.h"

#define FLASH_DEVICE_SIZE_BYTES     (1024 * 1024 * 64)          /* 64 MB flash device */
#define BLOCK_LIMIT_KB              (64)                        /* 64 kB maximum block size for the test */
#define MAX_BLOCK_SIZE              (BLOCK_LIMIT_KB * 1024)     /* maximum block size in bytes*/
#define GPT_CLOCK_MHZ               (200)                       /* the GPT clock speed in MHz */
#define WRITE_BLOCK_SIZE            (64)                        /* the size of a block when writing to the flash*/
#define TEST_BLOCKS                 (64)

#define WAIT_TIME                   (2000000)

#define TEST_DATA_LENGTH            (0x40U)

#define WAITING_MESSAGE             ("Waiting Results")
#undef MODULE_NAME
#define MODULE_NAME                 "\r\n%d. OCTO-SPI SPEED DEMONSTRATION\r\n"

#define SUB_OPTIONS                 "\r\nCompares the write and read times to and from external Octo-SPI" \
                                    "\r\nflash memory\r\n" \
                                    "\r\n> Enter the text block size " \
                                    "\r\n(in multiples of 2 KB, max 64 KB) and press tab to continue : "

/* The RX74N OSPI flash is MXIC MX25LW51245GXD2100-T */
/* The device is 512M bit (64M Byte) */

static fsp_err_t write_test_opi (uint32_t num_blocks);
static fsp_err_t read_test_opi (uint32_t num_blocks);

static uint32_t validate_user_input (char_t * p_input);
static uint32_t value_validation (uint32_t value);

static void ospi_performance_test (uint32_t data_size,
        uint32_t * ospi_performance_write_result, uint32_t * ospi_performance_read_result);

static void ospi_read_write_print (uint32_t block_size_actual);
static void ospi_menu_print (void);

static bool_t check_input_value (uint32_t value);

/* Cast to uint8_t *: 0x30000000 is the fixed memory-mapped base address of the OSPI CS0
 * peripheral region as defined in the hardware memory map. This address is guaranteed
 * to be a valid aligned memory-mapped address for the OSPI flash device */
static char_t   s_print_buffer[BUFFER_LINE_LENGTH];
static uint8_t  s_read_block[MAX_BLOCK_SIZE];
static uint8_t  s_g_test_data[TEST_DATA_LENGTH];
static char_t   s_block_sz_str[INPUT_BUFFER];

static uint32_t s_test_remain = WRITE_BLOCK_SIZE;

static uint32_t s_test_current_block = 0;
static uint32_t s_test_current_sub   = 0;
static uint32_t s_g_counter          = 0;


/**********************************************************************************************************************
 * Function Name: ext_display_menu
 * Description  : Implements the external flash memory menu
 * Return Value : 0
 *********************************************************************************************************************/
test_fn ext_display_menu(void)
{
    bool_t   valid_block_size  = false;
    int32_t  c                 = -1;
    int32_t  block_sz_ndx      = 0;
    int32_t  block_sz_limit    = (INPUT_BUFFER - 2); /* Allowing for TAB and End of Message */
    uint32_t block_size_actual = 0;
    uint32_t value             = 0;

    ospi_menu_print ();

    /* Keep trying to read a valid text block size
     * complete the loop in one of two ways:
     * [1] Valid block size is entered (2K boundary range 2-64K) followed by TAB
     * [2] Space Bar is pressed at any stage */
    while (!valid_block_size)
    {
        /* reset input state */
        c            = -1;
        block_sz_ndx = 0;
        memset (&s_block_sz_str, 0, INPUT_BUFFER);
        while ((CONNECTION_ABORT_CRTL != c))
        {
            c = input_from_console ();

            if ((block_sz_ndx < block_sz_limit) && (BACK_SPACE != c)
                    && (BACK_SPACE_PUTTY != c) && (CARRAGE_RETURN != c) && (MENU_ENTER_RESPONSE_CRTL != c))
            {
                /* cast to req type */
                s_block_sz_str[block_sz_ndx] = (char_t) c;
                block_sz_ndx++;
            }
            else if (((BACK_SPACE == c) || (BACK_SPACE_PUTTY == c)) && (block_sz_ndx > 0))
            {
                s_block_sz_str[block_sz_ndx - 1] = '\0';
                block_sz_ndx--;
                print_to_console ("\b \b");
            }
            else if ((0 == block_sz_ndx) || (CARRAGE_RETURN == c))
            {
                /* No action required: either buffer is empty or carriage return */
                ;
            }
            else
            {
                /* maximum block size exceeded (4 digits / characters entered) */
                s_block_sz_str[block_sz_ndx] = MENU_ENTER_RESPONSE_CRTL;

                c = MENU_ENTER_RESPONSE_CRTL;
                break;
            }
            if (MENU_EXIT_CRTL == c)
            {
                /* abort the test */
                valid_block_size  = true;
                block_size_actual = 0;
                break;
            }
            if ((MENU_ENTER_RESPONSE_CRTL == c) || (CARRAGE_RETURN == c))
            {
                break;
            }
            if ((CARRAGE_RETURN != c) && (BACK_SPACE != c))
            {
                /* Cast to char_t: value of c is a valid printable ASCII character */
                sprintf (s_print_buffer, "%c", (char_t)c);
                print_to_console (s_print_buffer);
            }
        }

        /* If the input was terminated with a TAB then attempt to process it */
        if ((MENU_ENTER_RESPONSE_CRTL == c) || (CARRAGE_RETURN == c))
        {
            value = validate_user_input (&s_block_sz_str[0]);
        }
        vTaskDelay (10);

        /* Exit the while loop when received space bar pressed */
        if (MENU_EXIT_CRTL == c)
        {
            break;
        }
        value            = value_validation (value);
        valid_block_size = check_input_value (value);
    }
    block_size_actual = value;
    if ((MENU_ENTER_RESPONSE_CRTL == c) && (0 != block_size_actual))
    {
        ospi_read_write_print (value);
    }
    while ((CONNECTION_ABORT_CRTL != c))
    {
        if ((MENU_EXIT_CRTL == c) || (0x00 == c))
        {
            break;
        }
        c = input_from_console ();
    }
    return (0);
}
/**********************************************************************************************************************
 End of function ext_display_menu
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: validate_user_input
 * Description  : parse the user input and convert to integer value
 * Argument     : p_input
 * Return Value : Integer value entered, or 0 if the menu exit character was entered
 *********************************************************************************************************************/
static uint32_t validate_user_input(char_t *p_input)
{
    uint32_t result = INVALID_CHARACTER;
    uint32_t value  = 0;
    uint32_t t      = 0;
    uint32_t c      = 0;

    while (true)
    {
        /* Cast to req type */
        c = (uint32_t) (*p_input);

        p_input++;

        if ((c >= '0') && (c <= '9'))
        {
            /* Cast as compiler will interpret result as int */
            value = (uint32_t)(value * 10);

            /* Cast to req type */
            t = (uint32_t)(c - '0');

            /* Cast as compiler will interpret result as int */
            value = (uint32_t)(value + t);
        }
        else
        {
            if (MENU_ENTER_RESPONSE_CRTL == c)
            {
                result = value;
            }

            if (MENU_EXIT_CRTL == c)
            {
                result = 0;
            }

            break;
        }
    }

    return (result);
}
/**********************************************************************************************************************
 End of function validate_user_input
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ospi_performance_test
 * Description  : write data to the OSPI flash, then read it. Use a timer to measure the durations
 * Argument     : data_size -  the data size to write and read
 * Argument     : ospi_performance_write_result - time taken for the write
 * Argument     : ospi_performance_read_result - time taken for the read
 * Return Value : None
 *********************************************************************************************************************/
static void ospi_performance_test(uint32_t data_size, uint32_t *ospi_performance_write_result,
        uint32_t *ospi_performance_read_result)
{
    fsp_err_t err          = FSP_SUCCESS;
    uint32_t  erase_blocks = 0;

    err = xspi_init ();
    if (FSP_SUCCESS != err)
    {
        print_to_console ("** XSPI Initialization failed ** \r\n");
        SYSTEM_ERROR_CFG_;
    }

    /* calculate the number of blocks to erase */
    erase_blocks = data_size / XSPI_SECTOR_SIZE;

    if ((erase_blocks * XSPI_SECTOR_SIZE) < data_size)
    {
        erase_blocks++;
    }

    /* erase the flash data area before we write to it */
    for (uint32_t block = 0; block < erase_blocks; block++)
    {
        /* Cast to uint8_t *: sp_gp_ospi_cs0 is the base address of the OSPI memory region
         * and the offset (erase_size * block) calculates the start address of each erase block.
         * The resulting address is guaranteed to be within the valid OSPI CS0 memory-mapped
         * region as erase_blocks is derived from data_size which does not exceed the
         * available OSPI memory space */
        err = R_XSPI_CONTROLLER_Erase (&g_xspi_ctrl,(uint8_t *)(XSPI_CS0_START_ADDRESS + (XSPI_SECTOR_SIZE * block)),
                                       XSPI_SECTOR_SIZE);
        if (FSP_SUCCESS != err)
        {
            print_to_console ("** R_XSPI_CONTROLLER_Erase API failed ** \r\n");
            SYSTEM_ERROR_CFG_;
        }
        xspi_wait_operation (WAIT_TIME);
    }

    print_to_console ("\r\nWriting the text block to external Octo-SPI flash memory ...\r\n");

    write_test_opi (data_size / WRITE_BLOCK_SIZE);

    *ospi_performance_write_result = s_g_counter;

    print_to_console ("\r\nWriting to flash completed\r\n");

    print_to_console ("\r\nReading the text block from external Octo-SPI flash memory ...\r\n");

    read_test_opi (data_size / WRITE_BLOCK_SIZE);

    *ospi_performance_read_result = s_g_counter;

    print_to_console ("\r\nReading from flash completed\r\n");

    R_XSPI_CONTROLLER_Close (&g_xspi_ctrl);
}
/**********************************************************************************************************************
 End of function ospi_performance_test
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: write_test_opi
 * Description  : write data to the OSPI flash and use a timer to time the write duration
 * Argument     : num_blocks - the number of blocks
 * Return Value : FSP_SUCCESS on success or an error code on failure
 *********************************************************************************************************************/
static fsp_err_t write_test_opi(uint32_t num_blocks)
{
    fsp_err_t      err = FSP_SUCCESS;

    timer_status_t timer_status;

    srand (0);

    /* Cast to uint8_t: rand() returns int per C standard but only the least significant
     * byte (0-255) is required for the byte-wise pseudo-random test pattern. The cast
     * safely truncates to the lower 8 bits which is the intended behaviour for
     * generating a repeatable single-byte data pattern from the seeded sequence */
    volatile uint8_t data = (uint8_t)rand ();

    R_GPT_Open (g_memory_performance.p_ctrl, g_memory_performance.p_cfg);
    R_GPT_Reset (g_memory_performance.p_ctrl);

    /* write the test data in OPI mode */
    s_g_counter = 0;

    for (s_test_current_block = 0; s_test_current_block < num_blocks; s_test_current_block++)
    {
        s_test_remain      = WRITE_BLOCK_SIZE;
        s_test_current_sub = 0;

        while (s_test_remain > 0)
        {
            s_test_remain--;

            /* Cast to uint8_t: rand() returns int but only the least significant byte
             * is needed to match the byte-wise pseudo-random pattern used during write */
            data = (uint8_t)rand ();

            s_g_test_data[s_test_current_sub++] = data;
        }

        /* use critical section so that interrupts don't affect the time measurements */
        FSP_CRITICAL_SECTION_DEFINE;
        FSP_CRITICAL_SECTION_ENTER;

        /* only time the write function that includes the wait_for_write */
        R_GPT_Start (g_memory_performance.p_ctrl);

        /* Cast to uint8_t *: sp_gp_ospi_cs0 is the base address of the OSPI memory-mapped region
         * and the offset (s_test_current_block * WRITE_BLOCK_SIZE) calculates the destination
         * address for the current block. The resulting address is guaranteed to be within the
         * valid OSPI CS1 memory-mapped region as s_test_current_block is bounded by the
         * total number of test blocks and WRITE_BLOCK_SIZE is a fixed block size constant */
        R_XSPI_CONTROLLER_Write (&g_xspi_ctrl, (s_g_test_data),
                (uint8_t*) (XSPI_CS0_START_ADDRESS + (s_test_current_block * WRITE_BLOCK_SIZE)), WRITE_BLOCK_SIZE);

        err = xspi_wait_operation (WAIT_TIME);

        R_GPT_Stop (g_memory_performance.p_ctrl);
        R_GPT_StatusGet (g_memory_performance.p_ctrl, &timer_status);

        FSP_CRITICAL_SECTION_EXIT;

        if (FSP_SUCCESS != err)
        {
            print_to_console ("** XSPI_CONTROLLER_Write API failed ** \r\n");
            SYSTEM_ERROR_CFG_;
        }

    }

    s_g_counter = timer_status.counter;

    R_GPT_Close (g_memory_performance.p_ctrl);

    return err;
}
/**********************************************************************************************************************
 End of function write_test_opi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: read_test_opi
 * Description  : read data from the OSPI flash and use a timer to time the read duration
 * Argument     : block_size - the size of the block to read
 * Return Value : FSP_SUCCESS on success or an error code on failure
 *********************************************************************************************************************/
static fsp_err_t read_test_opi(uint32_t block_size)
{
    fsp_err_t err = FSP_SUCCESS;

    timer_status_t timer_status;

    uint32_t failing_index = 0;
    uint8_t *p_ptr;

    srand (0);

    /* Cast to uint8_t: rand() returns int but only the least significant byte
     * is needed to match the byte-wise pseudo-random pattern used during write */
    volatile uint8_t  data   = (uint8_t)rand ();
    volatile uint32_t errors = 0;

    /* scale block_size to bytes by multiplying by the block size */
    block_size *= WRITE_BLOCK_SIZE;

    err = R_GPT_Open (g_memory_performance.p_ctrl, g_memory_performance.p_cfg);

    R_GPT_Reset (g_memory_performance.p_ctrl);

    memset (s_read_block, 0, block_size);

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    /* Read the data and time the read operation */
    R_GPT_Start (g_memory_performance.p_ctrl);

    /* Cast to const void */
    memcpy (s_read_block, (const void *)XSPI_CS0_START_ADDRESS, block_size);

    R_GPT_Stop (g_memory_performance.p_ctrl);
    R_GPT_StatusGet (g_memory_performance.p_ctrl, &timer_status);

    s_g_counter = timer_status.counter;
    FSP_CRITICAL_SECTION_EXIT;

    p_ptr = s_read_block;

    /* Verify data */
    for (uint32_t i = 0; i < block_size; i++)
    {
        /* Cast to uint8_t: rand() returns int but only the least significant byte
         * is needed to match the byte-wise pseudo-random pattern used during write */
        data = (uint8_t)rand ();

        if ((*p_ptr) != data)
        {
            if (0 == errors)
            {
                failing_index = i;
            }

            err = FSP_ERR_INVALID_DATA;
            errors++;
        }

        p_ptr++;
    }

    if (errors > 0)
    {
        sprintf (s_print_buffer, "Failed to verify %lu bytes. First verification failure index %lu\r\n", errors,
                 failing_index);
        print_to_console (s_print_buffer);
    }

    R_GPT_Close (g_memory_performance.p_ctrl);

    return (err);
}
/**********************************************************************************************************************
 End of function read_test_opi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ospi_read_write_print
 * Description  : .
 * Argument     : block_size_actual
 * Return Value : .
 *********************************************************************************************************************/
static void ospi_read_write_print(uint32_t block_size_actual)
{
    uint32_t ospi_performance_write_result = 0;
    uint32_t ospi_performance_read_result  = 0;
    uint32_t ospi_read_result              = 0;
    uint32_t ospi_write_result             = 0;

    char_t res_str[24];

    sprintf (s_print_buffer, "\r\n\r\nGenerated a text block of %2lu KB in SRAM\r\n", block_size_actual);
    print_to_console (s_print_buffer);

    block_size_actual = block_size_actual * 1024;

    ospi_performance_test (block_size_actual, &ospi_performance_write_result, &ospi_performance_read_result);

    ospi_write_result = ospi_performance_write_result / GPT_CLOCK_MHZ;
    ospi_read_result  = ospi_performance_read_result / GPT_CLOCK_MHZ;

    print_to_console ("\r\n-------------------------------------------------");
    print_to_console ("\r\nOperation/Flash          Octo-SPI");
    print_to_console ("\r\n-------------------------------------------------");

    sprintf (s_print_buffer, "\r\nWrite                    %6ld", ospi_write_result);
    print_to_console (s_print_buffer);

    sprintf (s_print_buffer, "\r\nRead                     %6ld", ospi_read_result);
    print_to_console (s_print_buffer);

    print_to_console ("\r\n-------------------------------------------------");
    print_to_console ("\r\nNote: Times are in microseconds");
    print_to_console (MENU_RETURN_INFO);

    sprintf (res_str, "%6ld uS", ospi_write_result);
    sprintf (res_str, "%6ld uS", ospi_read_result);
    sprintf (res_str, "Block Size %6ld Bytes", block_size_actual);

}
/**********************************************************************************************************************
 End of function ospi_read_write_print
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: value_validation
 * Description  : Ensure a provided value is divisible by the defined BLOCK_LIMIT.
 * Argument     : value
 * Return Value : the valid value
 *********************************************************************************************************************/
static uint32_t value_validation(uint32_t value)
{
    if (value < 2)
    {
        value = INVALID_BLOCK_SIZE;
    }
    if ((value > BLOCK_LIMIT_KB) && (value < INVALID_MARKERS))
    {
        value = INVALID_BLOCK_SIZE;
    }
    if ((value > 0) && ((value % 2) != 0) && (value < INVALID_MARKERS))
    {
        value = INVALID_BLOCK_BOUNDARY;
    }
    return value;
}
/**********************************************************************************************************************
 End of function value_validation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: check_input_value
 * Description  : Check the value obtained is a valid block size.
 * Arguments    : value: valid_block_size
 * Return Value : true or false if the block size is valid or not
 *********************************************************************************************************************/
static bool_t check_input_value(uint32_t value)
{
    switch (value)
    {

        case INVALID_CHARACTER:
        {
            sprintf (s_print_buffer,
                     "\r\n> Invalid character in entry, enter the text block size specifying a 2K boundary (eg 24)"
                     " and press tab : ");

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console (s_print_buffer);
            break;
        }

        case INVALID_BLOCK_SIZE:
        {
            sprintf (s_print_buffer, "\r\n> Invalid size, enter the text block size (eg 24) and press tab "
                     ": ");

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console (s_print_buffer);
            break;
        }

        case INVALID_BLOCK_BOUNDARY:
        {
            sprintf (s_print_buffer,
                     "\r\n> Invalid boundary, enter the text block size specifying a 2K boundary (eg 24)"
                     " and press tab : ");

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console (s_print_buffer);
            break;
        }

        default:
        {
            return (true);
        }
    }
    return (false);
}
/**********************************************************************************************************************
 End of function check_input_value
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ospi_menu_print
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void ospi_menu_print(void)
{
    print_to_console ("\x1b[2J\x1b[H");
    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console (s_print_buffer);
    print_to_console (SUB_OPTIONS);
}
/**********************************************************************************************************************
 End of function ospi_menu_print
 *********************************************************************************************************************/

