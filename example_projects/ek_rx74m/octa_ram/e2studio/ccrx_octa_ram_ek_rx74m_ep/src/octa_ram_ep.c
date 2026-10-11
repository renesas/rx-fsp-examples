/***********************************************************************************************************************
 * File Name    : octa_ram_ep.c.
 * Description  : Contains data structures and functions used in octa_ram_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "octa_ram_commands.h"
#include "octa_ram_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup octa_ram_ep.c
 * @{
 **********************************************************************************************************************/
/* External variables */
extern spi_flash_direct_transfer_t g_octa_ram_direct_transfer [OCTA_RAM_TRANSFER_MAX];
extern bsp_leds_t g_bsp_leds;

/* Global variables */
uint8_t g_read_data [OCTA_RAM_APP_DATA_SIZE]  = {RESET_VALUE};

uint8_t g_write_data [OCTA_RAM_APP_DATA_SIZE] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
    0x01, 0x00, 0x03, 0x02, 0x05, 0x04, 0x07, 0x06, 0x09, 0x08, 0x0B, 0x0A, 0x0D, 0x0C, 0x0F, 0x0E,
    0x11, 0x10, 0x13, 0x12, 0x15, 0x14, 0x17, 0x16, 0x19, 0x18, 0x1B, 0x1A, 0x1D, 0x1C, 0x1F, 0x1E,
    0x21, 0x20, 0x23, 0x22, 0x25, 0x24, 0x27, 0x26, 0x29, 0x28, 0x2B, 0x2A, 0x2D, 0x2C, 0x2F, 0x2E,
    0x31, 0x30, 0x33, 0x32, 0x35, 0x34, 0x37, 0x36, 0x39, 0x38, 0x3B, 0x3A, 0x3D, 0x3C, 0x3F, 0x3E,
    0x41, 0x40, 0x43, 0x42, 0x45, 0x44, 0x47, 0x46, 0x49, 0x48, 0x4B, 0x4A, 0x4D, 0x4C, 0x4F, 0x4E,
    0x51, 0x50, 0x53, 0x52, 0x55, 0x54, 0x57, 0x56, 0x59, 0x58, 0x5B, 0x5A, 0x5D, 0x5C, 0x5F, 0x5E,
    0x61, 0x60, 0x63, 0x62, 0x65, 0x64, 0x67, 0x66, 0x69, 0x68, 0x6B, 0x6A, 0x6D, 0x6C, 0x6F, 0x6E,
    0x71, 0x70, 0x73, 0x72, 0x75, 0x74, 0x77, 0x76, 0x79, 0x78, 0x7B, 0x7A, 0x7D, 0x7C, 0x7F, 0x7E,
    0x81, 0x80, 0x83, 0x82, 0x85, 0x84, 0x87, 0x86, 0x89, 0x88, 0x8B, 0x8A, 0x8D, 0x8C, 0x8F, 0x8E,
    0x91, 0x90, 0x93, 0x92, 0x95, 0x94, 0x97, 0x96, 0x99, 0x98, 0x9B, 0x9A, 0x9D, 0x9C, 0x9F, 0x9E,
    0xA1, 0xA0, 0xA3, 0xA2, 0xA5, 0xA4, 0xA7, 0xA6, 0xA9, 0xA8, 0xAB, 0xAA, 0xAD, 0xAC, 0xAF, 0xAE,
    0xB1, 0xB0, 0xB3, 0xB2, 0xB5, 0xB4, 0xB7, 0xB6, 0xB9, 0xB8, 0xBB, 0xBA, 0xBD, 0xBC, 0xBF, 0xBE,
    0xC1, 0xC0, 0xC3, 0xC2, 0xC5, 0xC4, 0xC7, 0xC6, 0xC9, 0xC8, 0xCB, 0xCA, 0xCD, 0xCC, 0xCF, 0xCE,
    0xD1, 0xD0, 0xD3, 0xD2, 0xD5, 0xD4, 0xD7, 0xD6, 0xD9, 0xD8, 0xDB, 0xDA, 0xDD, 0xDC, 0xDF, 0xDE,
    0xE1, 0xE0, 0xE3, 0xE2, 0xE5, 0xE4, 0xE7, 0xE6, 0xE9, 0xE8, 0xEB, 0xEA, 0xED, 0xEC, 0xEF, 0xEE,
    0xF1, 0xF0, 0xF3, 0xF2, 0xF5, 0xF4, 0xF7, 0xF6, 0xF9, 0xF8, 0xFB, 0xFA, 0xFD, 0xFC, 0xFF, 0xFE,
    0x02, 0x03, 0x00, 0x01, 0x06, 0x07, 0x04, 0x05, 0x0A, 0x0B, 0x08, 0x09, 0x0E, 0x0F, 0x0C, 0x0D,
    0x12, 0x13, 0x10, 0x11, 0x16, 0x17, 0x14, 0x15, 0x1A, 0x1B, 0x18, 0x19, 0x1E, 0x1F, 0x1C, 0x1D,
    0x22, 0x23, 0x20, 0x21, 0x26, 0x27, 0x24, 0x25, 0x2A, 0x2B, 0x28, 0x29, 0x2E, 0x2F, 0x2C, 0x2D,
    0x32, 0x33, 0x30, 0x31, 0x36, 0x37, 0x34, 0x35, 0x3A, 0x3B, 0x38, 0x39, 0x3E, 0x3F, 0x3C, 0x3D,
    0x42, 0x43, 0x40, 0x41, 0x46, 0x47, 0x44, 0x45, 0x4A, 0x4B, 0x48, 0x49, 0x4E, 0x4F, 0x4C, 0x4D,
    0x52, 0x53, 0x50, 0x51, 0x56, 0x57, 0x54, 0x55, 0x5A, 0x5B, 0x58, 0x59, 0x5E, 0x5F, 0x5C, 0x5D,
    0x62, 0x63, 0x60, 0x61, 0x66, 0x67, 0x64, 0x65, 0x6A, 0x6B, 0x68, 0x69, 0x6E, 0x6F, 0x6C, 0x6D,
    0x72, 0x73, 0x70, 0x71, 0x76, 0x77, 0x74, 0x75, 0x7A, 0x7B, 0x78, 0x79, 0x7E, 0x7F, 0x7C, 0x7D,
    0x82, 0x83, 0x80, 0x81, 0x86, 0x87, 0x84, 0x85, 0x8A, 0x8B, 0x88, 0x89, 0x8E, 0x8F, 0x8C, 0x8D,
    0x92, 0x93, 0x90, 0x91, 0x96, 0x97, 0x94, 0x95, 0x9A, 0x9B, 0x98, 0x99, 0x9E, 0x9F, 0x9C, 0x9D,
    0xA2, 0xA3, 0xA0, 0xA1, 0xA6, 0xA7, 0xA4, 0xA5, 0xAA, 0xAB, 0xA8, 0xA9, 0xAE, 0xAF, 0xAC, 0xAD,
    0xB2, 0xB3, 0xB0, 0xB1, 0xB6, 0xB7, 0xB4, 0xB5, 0xBA, 0xBB, 0xB8, 0xB9, 0xBE, 0xBF, 0xBC, 0xBD,
    0xC2, 0xC3, 0xC0, 0xC1, 0xC6, 0xC7, 0xC4, 0xC5, 0xCA, 0xCB, 0xC8, 0xC9, 0xCE, 0xCF, 0xCC, 0xCD,
    0xD2, 0xD3, 0xD0, 0xD1, 0xD6, 0xD7, 0xD4, 0xD5, 0xDA, 0xDB, 0xD8, 0xD9, 0xDE, 0xDF, 0xDC, 0xDD,
    0xE2, 0xE3, 0xE0, 0xE1, 0xE6, 0xE7, 0xE4, 0xE5, 0xEA, 0xEB, 0xE8, 0xE9, 0xEE, 0xEF, 0xEC, 0xED,
    0xF2, 0xF3, 0xF0, 0xF1, 0xF6, 0xF7, 0xF4, 0xF5, 0xFA, 0xFB, 0xF8, 0xF9, 0xFE, 0xFF, 0xFC, 0xFD,
    0x03, 0x02, 0x01, 0x00, 0x07, 0x06, 0x05, 0x04, 0x0B, 0x0A, 0x09, 0x08, 0x0F, 0x0E, 0x0D, 0x0C,
    0x13, 0x12, 0x11, 0x10, 0x17, 0x16, 0x15, 0x14, 0x1B, 0x1A, 0x19, 0x18, 0x1F, 0x1E, 0x1D, 0x1C,
    0x23, 0x22, 0x21, 0x20, 0x27, 0x26, 0x25, 0x24, 0x2B, 0x2A, 0x29, 0x28, 0x2F, 0x2E, 0x2D, 0x2C,
    0x33, 0x32, 0x31, 0x30, 0x37, 0x36, 0x35, 0x34, 0x3B, 0x3A, 0x39, 0x38, 0x3F, 0x3E, 0x3D, 0x3C,
    0x43, 0x42, 0x41, 0x40, 0x47, 0x46, 0x45, 0x44, 0x4B, 0x4A, 0x49, 0x48, 0x4F, 0x4E, 0x4D, 0x4C,
    0x53, 0x52, 0x51, 0x50, 0x57, 0x56, 0x55, 0x54, 0x5B, 0x5A, 0x59, 0x58, 0x5F, 0x5E, 0x5D, 0x5C,
    0x63, 0x62, 0x61, 0x60, 0x67, 0x66, 0x65, 0x64, 0x6B, 0x6A, 0x69, 0x68, 0x6F, 0x6E, 0x6D, 0x6C,
    0x73, 0x72, 0x71, 0x70, 0x77, 0x76, 0x75, 0x74, 0x7B, 0x7A, 0x79, 0x78, 0x7F, 0x7E, 0x7D, 0x7C,
    0x83, 0x82, 0x81, 0x80, 0x87, 0x86, 0x85, 0x84, 0x8B, 0x8A, 0x89, 0x88, 0x8F, 0x8E, 0x8D, 0x8C,
    0x93, 0x92, 0x91, 0x90, 0x97, 0x96, 0x95, 0x94, 0x9B, 0x9A, 0x99, 0x98, 0x9F, 0x9E, 0x9D, 0x9C,
    0xA3, 0xA2, 0xA1, 0xA0, 0xA7, 0xA6, 0xA5, 0xA4, 0xAB, 0xAA, 0xA9, 0xA8, 0xAF, 0xAE, 0xAD, 0xAC,
    0xB3, 0xB2, 0xB1, 0xB0, 0xB7, 0xB6, 0xB5, 0xB4, 0xBB, 0xBA, 0xB9, 0xB8, 0xBF, 0xBE, 0xBD, 0xBC,
    0xC3, 0xC2, 0xC1, 0xC0, 0xC7, 0xC6, 0xC5, 0xC4, 0xCB, 0xCA, 0xC9, 0xC8, 0xCF, 0xCE, 0xCD, 0xCC,
    0xD3, 0xD2, 0xD1, 0xD0, 0xD7, 0xD6, 0xD5, 0xD4, 0xDB, 0xDA, 0xD9, 0xD8, 0xDF, 0xDE, 0xDD, 0xDC,
    0xE3, 0xE2, 0xE1, 0xE0, 0xE7, 0xE6, 0xE5, 0xE4, 0xEB, 0xEA, 0xE9, 0xE8, 0xEF, 0xEE, 0xED, 0xEC,
    0xF3, 0xF2, 0xF1, 0xF0, 0xF7, 0xF6, 0xF5, 0xF4, 0xFB, 0xFA, 0xF9, 0xF8, 0xFF, 0xFE, 0xFD, 0xFC
};

uint8_t g_octa_ram_address[OCTA_RAM_APP_DATA_SIZE] BSP_PLACE_IN_SECTION (BSP_UNINIT_SECTION_PREFIX ".xspi1_cs0_noinit");

/* Function declarations */
static fsp_err_t octa_ram_init(void);
static fsp_err_t octa_ram_read_device_id(uint32_t * const p_id);
static fsp_err_t octa_ram_write_operation(uint8_t * p_address, uint32_t size, uint32_t * const p_time);
static fsp_err_t octa_ram_read_operation(uint8_t * const p_address, uint32_t size, uint32_t * const p_time);
static void print_read_buffer(uint8_t * p_address, uint32_t size);
static uint32_t get_data_size(void);
static fsp_err_t timer_init(void);
static fsp_err_t timer_start_measure(void);
static fsp_err_t timer_get_measure(uint32_t * p_time);
void handle_error(fsp_err_t err, char *err_str);

/*******************************************************************************************************************//**
 * @brief       This function is used to start OctaRAM example operation.
 * @param       None
 * @retval      None
 **********************************************************************************************************************/
void octa_ram_ep_entry(void)
{
    fsp_err_t           err                     = FSP_SUCCESS;
    fsp_pack_version_t  version                 = {RESET_VALUE};
    uint32_t            octalram_id             = RESET_VALUE;
    uint32_t            user_option             = RESET_VALUE;
    char                rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    uint32_t            execute_time            = RESET_VALUE;
    uint32_t            data_size               = RESET_VALUE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

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

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize timer to measure OSPI operation */
    err = timer_init();
    handle_error(err, "timer_init FAILED\r\n");

    /* Initialize OSPI driver module and OctaRAM device in DOPI mode */
    err = octa_ram_init();
    handle_error(err, "octa_ram_init FAILED\r\n");

    /* Read OctaRAM device ID */
    err = octa_ram_read_device_id(&octalram_id);
    handle_error(err, "octa_ram_read_device_id FAILED\r\n");

    /* Print OctaRAM device ID */
    APP_PRINT("\r\nDevice ID read successfully");
    APP_PRINT("\r\nManufacturer ID = 0x%x (Bit 0-3 of Identification (ID) Register - Value 0011)\r\n", (uint8_t)octalram_id);

    /* Print Main Menu option */
    APP_PRINT(EP_MENU);

    /* Perform XSPI operations */
    while(true)
    {
        /* Check new input data from Debug Virtual Console */
        if(APP_CHECK_DATA)
        {
            APP_READ(rByte); /* Read user input from terminal */

            /* Convert ASCII input to integer option */
            user_option = (uint32_t)(atoi((char *)rByte));

            switch(user_option)
                {
                    case SELECT_WRITE_OPERATION_CHAR:
                        /* Reset the data_size value */
                        data_size = RESET_VALUE;

                        /* Get target data size to transfer */
                        data_size = get_data_size();

                        /* Performs OSPI write operation */
                        APP_PRINT("\r\nWrite data on 0x%x:\r\n", &g_octa_ram_address[0]);
                        err = octa_ram_write_operation (&g_octa_ram_address[0], data_size, &execute_time);
                        handle_error(err, "octa_ram_write_operation FAILED\r\n");

                        /* Print execution time */
                        APP_PRINT("%s: Write %d bytes completed successfully in %d nanoseconds\r\n"
                                , PROTOCOL_MODE, data_size, execute_time);
                        break;

                    case SELECT_READ_OPERATION_CHAR:
                        if (RESET_VALUE == data_size)
                        {
                            APP_PRINT("\r\n No previous write operation was detected, or the test region has been reset."\
                                      "\r\n The 1 KB RAM test region will be displayed for inspection only. Data comparison is skipped!!\r\n");
                            /* Performs OSPI read operation */
                            err = octa_ram_read_operation (&g_octa_ram_address[0],OCTA_RAM_APP_DATA_SIZE , &execute_time);
                            handle_error(err, "octa_ram_read_operation FAILED\r\n");
                            /* Print read data in array */
                            APP_PRINT("\r\nRead data on 0x%x:\r\n", &g_octa_ram_address[0]);
                            print_read_buffer(&g_octa_ram_address[0], OCTA_RAM_APP_DATA_SIZE);
                        }
                        else
                        {
                            /* Performs OSPI read operation */
                            err = octa_ram_read_operation (&g_octa_ram_address[0],data_size , &execute_time);
                            handle_error(err, "octa_ram_read_operation FAILED\r\n");

                            /* Print execution time */
                            APP_PRINT("\r\n%s: Read %d bytes completed successfully in %d nanoseconds\r\n"
                                    , PROTOCOL_MODE, (uint32_t)(data_size), execute_time);

                            /* Print read data in array */
                            APP_PRINT("\r\nRead data on 0x%x:\r\n", &g_octa_ram_address[0]);
                            print_read_buffer(&g_octa_ram_address[0], data_size);

                            /* Compare data read and date written */
                            if(RESET_VALUE == memcmp(&g_read_data, &g_write_data, data_size))
                            {
                                APP_PRINT("\r\n%s: Data read matched data written\r\n", PROTOCOL_MODE);
                            }
                            else
                            {
                                APP_PRINT("%s: Data read does not match data written\r\n", PROTOCOL_MODE);
                            }
                        }
                        break;

                    case SELECT_RESET_OPERATION_CHAR:
                        memset(&g_octa_ram_address, RESET_VALUE, OCTA_RAM_APP_DATA_SIZE);
                        APP_PRINT("\r\n1KB data has been reset\r\n");

                        /* Reset the data_size value */
                        data_size = RESET_VALUE;
                        break;

                    default:
                        APP_PRINT ("\r\nInvalid input. Please select a valid input\r\n");
                        break;
                }
            /* Print Main Menu option of OSPI operation.*/
            APP_PRINT(EP_MENU);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions initializes OSPI module and OctaRAM device.
 * @param       None
 * @retval      FSP_SUCCESS     Upon successful initialization of OSPI module and OctaRAM device
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
static fsp_err_t octa_ram_init(void)
{
    /* By default, the OctaRAM device is in SPI mode, so it is necessary to open the OSPI module in SPI mode */
    fsp_err_t                   err      = FSP_SUCCESS;
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
    err = R_XSPI_CONTROLLER_Open(&g_octa_ram_ctrl, &g_octa_ram_cfg);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_Open API FAILED \r\n");

    /* Switch DOPI module to 8D-8D-8D mode to configure OctalRAM device */
    err = R_XSPI_CONTROLLER_SpiProtocolSet(&g_octa_ram_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_SpiProtocolSet API FAILED \r\n");

    /* Reset OctaRAM device by driving OM_RESET pin */
    R_XSPI1->LIOCTL_b.RSTCS0 = 0;
    R_BSP_SoftwareDelay(OCTA_RAM_TIME_RESET_PULSE, OCTA_RAM_TIME_UNIT);
    R_XSPI1->LIOCTL_b.RSTCS0 = 1;
    R_BSP_SoftwareDelay(OCTA_RAM_TIME_RESET_SETUP, OCTA_RAM_TIME_UNIT);

    /* Write dummy data to Configuration Register (CR) */
    transfer = g_octa_ram_direct_transfer[OCTA_RAM_TRANSFER_WRITE_OCTALRAM_CR_OPI];
    err = R_XSPI_CONTROLLER_DirectTransfer(&g_octa_ram_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function reads OctaRAM device id
 * @param[out]  *p_device_id        Pointer will be used to store device id
 * @retval      FSP_SUCCESS         Upon successful direct transfer operation
 * @retval      FSP_ERR_ABORTED     On incorrect device id read.
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t octa_ram_read_device_id (uint32_t * const p_id)
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

    /* Read and check OctalRAM device ID */
    transfer = g_octa_ram_direct_transfer[OCTA_RAM_TRANSFER_READ_OCTALRAM_ID_OPI];
    err = R_XSPI_CONTROLLER_DirectTransfer(&g_octa_ram_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    APP_ERR_RETURN(err, "R_XSPI_CONTROLLER_DirectTransfer API FAILED \r\n");

    if (OCTA_RAM_DEVICE_ID != (((transfer.data_u.data << 8) | (transfer.data_u.data >> 8)) & 0x0F)) /* Bit 0-3 of Identification Register (ID) */
    {
        APP_ERR_RETURN(FSP_ERR_ABORTED, "Device ID is incorrect\r\n");
    }

    /* Get OctaRAM device ID */
    *p_id = (((transfer.data_u.data << 8) | (transfer.data_u.data >> 8)) & 0x0F);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function performs a write operation on the OctaRAM device.
 * @param[in]   size        Target data size to write
 * @param[in]   *p_address  Pointer to OctaRAM device memory address
 * @param[out]  *p_time     Pointer will be used to store execute time
 * @retval      FSP_SUCCESS Upon successful write operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t octa_ram_write_operation(uint8_t * p_address, uint32_t size, uint32_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start measure */
    err = timer_start_measure();
    APP_ERR_RETURN(err, "timer_start_measure FAILED\r\n");

    /* Write data to OctaRAM device */
    memcpy(p_address, &g_write_data[0], size);

    /* Get execution time */
    err = timer_get_measure(p_time);
    APP_ERR_RETURN(err, "timer_get_measure FAILED\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function performs a read operation on the OctaRAM device.
 * @param[in]   size        Target data size to read
 * @param[in]   *p_address  Pointer to OctaRAM device memory address
 * @param[out]  *p_time     Pointer will be used to store execute time
 * @retval      FSP_SUCCESS Upon successful read operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t octa_ram_read_operation(uint8_t * p_address, uint32_t size, uint32_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clean read buffer */
    memset(&g_read_data[0], RESET_VALUE, OCTA_RAM_APP_DATA_SIZE);

    /* Start measure */
    err = timer_start_measure();
    APP_ERR_RETURN(err, "timer_start_measure FAILED\r\n");

    /* Read data from OctaRAM device */
    memcpy(&g_read_data[0], p_address, size);

    /* Get execution time */
    err = timer_get_measure(p_time);
    APP_ERR_RETURN(err, "timer_get_measure FAILED\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function is used to print out the read buffer.
 * @param[in]   size    Target data size
 * @param       None
 * @retval      None
 **********************************************************************************************************************/
static void print_read_buffer(uint8_t * p_address, uint32_t size)
{
    APP_PRINT("\t");
    for (uint32_t i = 0; i < size; i++)
    {
        APP_PRINT("%02X ", p_address[i]);

        if ((i % 16) == 15)
        {
            APP_PRINT("\r\n\t");
        }
    }
    APP_PRINT("\r\n");
}

/*******************************************************************************************************************//**
 * @brief       This function is used to select data size for read and write operation.
 * @param       None
 * @retval      None
 **********************************************************************************************************************/
static uint32_t get_data_size(void)
{
    uint32_t            user_option             = RESET_VALUE;
    char                rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    /* Print Sub Menu option */
    APP_PRINT(EP_SUB_MENU);

    while(true)
    {
        /* Check new input data from Debug Virtual Console */
        if(APP_CHECK_DATA)
        {
            APP_READ(rByte); /* Read user input from terminal */

            /* Convert ASCII input to integer option */
            user_option = (uint32_t)(atoi((char *)rByte));
            switch(user_option)
            {
                case SUB_MENU_64_BYTES:
                    return 64U;
                case SUB_MENU_128_BYTES:
                    return 128U;
                case SUB_MENU_256_BYTES:
                    return 256U;
                case SUB_MENU_512_BYTES:
                    return 512U;
                case SUB_MENU_1024_BYTES:
                    return 1024U;
                default:
                    APP_PRINT ("\r\nInvalid input. Please select a valid input\r\n");
                    APP_PRINT(EP_SUB_MENU);
                    break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function starts GPT module to measure execution time of an OSPI operation.
 * @param       None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_start_measure(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear timer counter */
    err = R_GPT_Reset (&g_timer_ctrl);
    APP_ERR_RETURN(err, "R_GPT_Reset FAILED\r\n");

    /* Start timer */
    err = R_GPT_Start (&g_timer_ctrl);
    APP_ERR_RETURN(err, "R_GPT_Start FAILED\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This Function measures the timing info by reading the timer.
 * @param[in]   *p_time     Pointer will be used to store the OSPI operation execution time
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_get_measure(uint32_t * p_time)
{
    fsp_err_t       err             = FSP_SUCCESS;
    timer_status_t  timer_status    =
    {
     .counter        = RESET_VALUE,
     .state          = (timer_state_t)RESET_VALUE
    };
    timer_info_t    timer_info      =
    {
     .clock_frequency = RESET_VALUE,
     .count_direction = (timer_direction_t)RESET_VALUE,
     .period_counts   = RESET_VALUE
    };

    /* Get status of timer */
    err = R_GPT_StatusGet (&g_timer_ctrl, &timer_status);
    APP_ERR_RETURN(err, "R_GPT_StatusGet FAILED\r\n");

    /* Get info of timer */
    err = R_GPT_InfoGet (&g_timer_ctrl, &timer_info);
    APP_ERR_RETURN(err, "R_GPT_InfoGet FAILED\r\n");

    /* Stop timer */
    err = R_GPT_Stop(&g_timer_ctrl);
    APP_ERR_RETURN(err, "R_GPT_Stop FAILED\r\n");

    /* Convert count value to nanoseconds unit */
    *p_time = (uint32_t)(((uint64_t)timer_status.counter * 1000000000ULL) / timer_info.clock_frequency);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions initializes GPT module used to measure OSPI operation execution time.
 * @param       None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    APP_ERR_RETURN(err, "R_GPT_Open FAILED\r\n");
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function handle error if error occurred, closes all opened module, print and traps error.
 *  @param[in]   status    error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, char *err_str)
{
    if(FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* Close opened OSPI module*/
        if(0U != g_octa_ram_ctrl.open)
        {
            if(FSP_SUCCESS != R_XSPI_CONTROLLER_Close(&g_octa_ram_ctrl))
            {
                APP_ERR_PRINT("R_XSPI_CONTROLLER_Close FAILED\r\n");
            }
        }

        /* Close opened GPT module*/
        if(0U != g_timer_ctrl.open)
        {
            if(FSP_SUCCESS != R_GPT_Close(&g_timer_ctrl))
            {
                APP_ERR_PRINT("R_GPT_Close FAILED\r\n");
            }
        }

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/*******************************************************************************************************************//**
 * @} (end addtogroup octa_ram_ep.c)
 **********************************************************************************************************************/
