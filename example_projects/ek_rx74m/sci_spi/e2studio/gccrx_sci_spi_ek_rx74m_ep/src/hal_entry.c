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

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * @addtogroup sci_spi_ep
 * @{
 **********************************************************************************************************************/

/* Slave Select pin for all boards to enable data transfer */
#define CS_PIN              (BSP_IO_PORT_09_PIN_02)

/* Function macros to assert and de-assert Slave Select pins */
#define CS_ASSERT(x)        R_IOPORT_PinWrite(&g_ioport_ctrl, x, BSP_IO_LEVEL_HIGH);
#define CS_DE_ASSERT(x)     R_IOPORT_PinWrite(&g_ioport_ctrl, x, BSP_IO_LEVEL_LOW);

#define EP_INFO             "\r\nThis Example Project demonstrates the basic usage of SCI_SPI driver."      \
                            "\r\nThe project writes commands to the slave sensor and reads the temperature" \
                            "\r\nvalue using SCI_SPI bus configured as master.The temperature values are"   \
                            "\r\ncontinuously printed on Terminal Application Console.\r\n"

#define CONVERSION_TIME     (200U)                  /* Conversion time for 12-bit resolution */
#define PRINT_DELAY         (2U)                    /* Delay for RTT viewer prints */
static volatile spi_event_t g_master_event_flag;    /* Master Transfer Event completion flag */

/* Wait counter for wait operation monitoring */
static volatile int32_t g_wait_count = INT32_MAX;
void R_BSP_WarmStart(bsp_warm_start_event_t event);
/* Read register address of temperature sensor */
static uint8_t read_temperature_reg[3] = {0x01};
/* Variable to store read temperature values */
static uint8_t temperature_values[3] = {RESET_VALUE};
uint8_t config_read[2] = {0x00};
static char dataBuff[30];
extern bsp_leds_t g_bsp_leds;

/* Function to check occurrence of event after data transfer */
static void sci_spi_event_check(void);
/* Cleanup function for opened module */
static void sci_spi_deinit(void);

/*******************************************************************************************************************//**
 * The RX Configuration tool generates main() and uses it to generate threads if an RTOS is used. This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err              = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };
    float temperature          = RESET_VALUE;

    /* Value for configuration register write. Set resolution as 12-bits*/
    const uint8_t config_sensor[3] =
    {
        0x80, 0x06
    };

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

    /* Initialize sci_spi channel as master */

    err = R_SCI_B_SPI_Open(&g_spi_ctrl, &g_spi_cfg);
    /* Handle Error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n SCI_SPI open failed");
        APP_ERR_TRAP(err);
    }

    /* Assert Slave select pin to start data transfer */
    CS_ASSERT(CS_PIN);

    /* Configure temperature sensor */
    err = R_SCI_B_SPI_Write(&g_spi_ctrl, &config_sensor[0], sizeof(config_sensor), SPI_BIT_WIDTH_8_BITS);
    /* Handle Error */
    if (FSP_SUCCESS != err)
    {
        sci_spi_deinit();
        APP_ERR_PRINT("\r\nSPI Write failed\r\n");
        APP_ERR_TRAP(err);
    }

    /* Check for transfer complete event and handle error in the case of event failure */
    sci_spi_event_check();

    /* De-assert Slave select pin to stop data transfer */
    CS_DE_ASSERT(CS_PIN);

    /* Resetting SPI Master event flag */
    g_master_event_flag = (spi_event_t)RESET_VALUE;

    /* Assert Slave select pin to start data transfer */
    CS_ASSERT(CS_PIN);

    /* Read configured temperature sensor */
    err = R_SCI_B_SPI_Read(&g_spi_ctrl, &config_read[0], sizeof(config_read), SPI_BIT_WIDTH_8_BITS);
    /* Handle Error */
    if (FSP_SUCCESS != err)
    {
        sci_spi_deinit();
        APP_ERR_PRINT("\r\nSPI Write failed\r\n");
        APP_ERR_TRAP(err);
    }

    /* Check for transfer complete event and handle error in the case of event failure */
    sci_spi_event_check();

    /* De-assert Slave select pin to stop data transfer */
    CS_DE_ASSERT(CS_PIN);

    /* Check if sensor is configured as expected */
    if (config_sensor[1] != config_read[1])
    {
        /* Incorrect configuration of temperature sensor */
        err = FSP_ERR_INVALID_HW_CONDITION;
        APP_ERR_TRAP(err);
    }

    /* Adding conversion time as 200ms for the configured 12-bit resolution,
     *  before asserting CS_PIN for reading the temperature values in a loop */
    R_BSP_SoftwareDelay(CONVERSION_TIME, BSP_DELAY_UNITS_MILLISECONDS);

    while (true)
    {
        /* Resetting SPI Master event flag */
        g_master_event_flag = (spi_event_t)RESET_VALUE;

        memset(dataBuff, RESET_VALUE, sizeof(dataBuff));

        /* Assert Slave select pin to start data transfer */
        CS_ASSERT(CS_PIN);
        /* Read the temperature */
        err = R_SCI_B_SPI_WriteRead(&g_spi_ctrl, read_temperature_reg, temperature_values, sizeof(temperature_values), SPI_BIT_WIDTH_8_BITS);
        /* Handle Error */
        if (FSP_SUCCESS != err)
        {
            sci_spi_deinit();
            APP_ERR_PRINT("\r\nSPI ReadWrite failed\r\n");
            APP_ERR_TRAP(err);
        }

        /* Check for transfer complete event and handle error in the case of event failure */
        sci_spi_event_check();

        /* De-assert Slave select pin to stop data transfer */
        CS_DE_ASSERT(CS_PIN);

        /* Manipulating the read temperature values to print for users */
        temperature = (float)((int32_t)(((uint32_t)temperature_values[2] << 4) | ((uint32_t)temperature_values[1] >> 4))) / 16.0f;

        /* Function to print float values */
        int temp_int = (int)(temperature * 1000000);
        snprintf(dataBuff, sizeof(dataBuff), "%d.%06d", temp_int / 1000000, temp_int % 1000000);

        APP_PRINT("\r\nTemperature:  %s *C", dataBuff);

        /* Delay to display temperature values on RTT viewer */
        R_BSP_SoftwareDelay(PRINT_DELAY, BSP_DELAY_UNITS_SECONDS);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function checks occurrence of data transfer complete event until timeout occurs and handles errors.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
static void sci_spi_event_check(void)
{
    while (SPI_EVENT_TRANSFER_COMPLETE != g_master_event_flag)
    {
        g_wait_count--;
        if (RESET_VALUE >= g_wait_count)
        {
            sci_spi_deinit();
            APP_ERR_PRINT("\r\nSPI module blocked in Write/Read operation.\r\n");
            /* Return time out error if SPI operation fails to complete */
            APP_ERR_TRAP(FSP_ERR_TIMEOUT);
        }
        /* All other error events are handled here */
        else if (SPI_EVENT_TRANSFER_ABORTED == g_master_event_flag)
        {
            sci_spi_deinit();
            APP_ERR_PRINT("\r\nSPI module transfer aborted.\r\n");
            APP_ERR_TRAP(FSP_ERR_TRANSFER_ABORTED);
        }
        else
        {
            /* Do nothing */
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened SCI_SPI module before the project ends up in an Error Trap.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
static void sci_spi_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close SCI_SPI module */
    err = R_SCI_B_SPI_Close(&g_spi_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* SPI Close failure message */
        APP_ERR_PRINT("** R_SCI_SPI_Close API for master failed **  \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief      Master SCI_SPI callback function.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void sci_spi_callback(spi_callback_args_t *p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        g_master_event_flag = SPI_EVENT_TRANSFER_COMPLETE;
    }
    else
    {
        /* Updating the flag here to capture and handle all other error events */
        g_master_event_flag = SPI_EVENT_TRANSFER_ABORTED;
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
 * @} (end addtogroup sci_spi_ep)
 **********************************************************************************************************************/
