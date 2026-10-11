/***********************************************************************************************************************
 * File Name    : can_fdg_ep.c
 * Description  : This file implements the CANFDG functionality for the example project. It handles CANFDG operations,
 *                write, read, and data comparison.
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "can_fdg_ep.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup can_fd_ep
 * @{
 **********************************************************************************************************************/
 can_frame_t g_canfdg_tx_frame;                      /* CAN transmit frame*/
 can_frame_t g_canfdg_rx_frame;

 /* Variable to store rx frame status info*/
 can_info_t g_can_rx_info =
 {
  .error_code  = RESET_VALUE,
  .error_count_receive = RESET_VALUE,
  .error_count_transmit = RESET_VALUE,
  .rx_fifo_status = RESET_VALUE,
  .rx_mb_status = RESET_VALUE,
  .status = RESET_VALUE,
 };

/* Data to be loaded in Classic CAN and FD frames for transmission and acknowledgment */
uint8_t g_tx_data[SIZE_64] = "TX_MESG";
uint8_t g_rx_data[SIZE_64] = "RX_MESG";
uint8_t g_tx_fd_data[SIZE_64];
uint8_t g_rx_fd_data[SIZE_64];

extern bool g_canfdg_tx_complete;
extern bool g_canfdg_rx_complete;
extern bool g_canfdg_err_status;
extern uint32_t g_time_out;

/* User defined functions */
static void can_write_operation(can_frame_t can_transmit_frame);
static void can_fdg_data_update(void);
static void can_data_check_operation(void);
static void led_update(led_state_t led_state);

/*******************************************************************************************************************//**
 * @brief       This function is to transmit data after receiving user input and it performs subsequent operations
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
void canfdg_operation(void)
{
    /* Update transmit frame parameters */
    g_canfdg_tx_frame.id = CAN_ID;
    g_canfdg_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    g_canfdg_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_canfdg_tx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
    g_canfdg_tx_frame.options = ZERO;

    /* Update transmit frame data with message */
    memcpy((uint8_t*)&g_canfdg_tx_frame.data[ZERO], (uint8_t*)&g_tx_data[ZERO], CAN_FDG_DATA_LENGTH_CODE);

    APP_PRINT("\r\nTransmission of data over classic CAN Frame");

    /* Transmission of data over classic CAN frame */
    can_write_operation(g_canfdg_tx_frame);

    APP_PRINT("\r\nClassic CAN transmission is successful");
}

/*******************************************************************************************************************//**
 * @brief       This function is to transmit data on classic CAN or CANFDG frame
 * @param[in]   can_transmit_frame        Data frame to be transmitted
 * @return      None
 **********************************************************************************************************************/
static void can_write_operation(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Transmit the data from mail box #0 with tx_frame */
    err = R_CANFDG_Write(&g_canfdg0_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCANFDG Write API FAILED");
        led_update(red);
        canfdg_deinit();
        APP_ERR_TRAP(err);
    }
    led_update(blue);

    /* Wait here for an event from callback */
    while ((true != g_canfdg_tx_complete) && (--g_time_out));
    if (RESET_VALUE == g_time_out)
    {
        APP_ERR_PRINT("\r\nCAN transmission failed due to timeout");
        led_update(red);
        APP_ERR_TRAP(true);
    }
    led_update(green);

    /* Reset flag bit */
    g_canfdg_tx_complete = false;
}

/*******************************************************************************************************************//**
 * @brief       This function is to compare transmitted/received data and to send ACK accordingly
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void can_data_check_operation(void)
{
    /*Update data to be compared with data transmitted/received over FD frame */
    can_fdg_data_update();

    if(RESET_VALUE == strncmp((char*)&g_canfdg_rx_frame.data[ZERO], (char*)&g_tx_data[ZERO],\
    CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        APP_PRINT("\r\nReceived 'TX__MESG' on classic frame, responding with 'RX__MESG' using classic CAN frame\r\n");
        /* Update the receive frame parameters*/
        g_canfdg_rx_frame.id = CAN_ID;
        g_canfdg_rx_frame.type = CAN_FRAME_TYPE_DATA;
        g_canfdg_rx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
        g_canfdg_rx_frame.options = ZERO;

        /* Update receive frame data with message */
        memcpy(&g_canfdg_rx_frame.data, &g_rx_data, CAN_CLASSIC_FRAME_DATA_BYTES);

        /* Transmission of data as acknowledgment */
        can_write_operation(g_canfdg_rx_frame);

        APP_PRINT("\r\nCAN transmission after receive is successful. Sent back the ACK using classic CAN frame\r\n");

    }
    else if(RESET_VALUE 
        == strncmp((char*)&g_canfdg_rx_frame.data[ZERO], (char*)&g_rx_data[ZERO], CAN_CLASSIC_FRAME_DATA_BYTES))
    {
        APP_PRINT("\r\nReceived Acknowledgment for Classic CAN Frame transmission.\r\n\
        CAN operation Successful length = %d\r\n",
            g_canfdg_rx_frame.data_length_code);
        APP_PRINT("\r\nData transmission over FD frame\r\n");
        /* Updating FD frame parameters */
        g_canfdg_rx_frame.options = CANFDG_FRAME_OPTION_FD | CANFDG_FRAME_OPTION_BRS;
        g_canfdg_rx_frame.data_length_code = CAN_FDG_DATA_LENGTH_CODE;

        /* Fill frame data that is to be sent in FD frame */
        for( uint16_t j = 0; j < SIZE_64; j++)
        {
            g_canfdg_rx_frame.data[j] = (uint8_t) (j + 1);
        }

        /* Transmission of data as over FD frame */
        can_write_operation(g_canfdg_rx_frame);

        APP_PRINT("\r\nCAN transmission on FD Frame after receiving classic frame ACK is successful\r\n");
    }
    /* Impact to Board 2: Compare received data with transmitted CANFDG data from Board 1 */
    else if(RESET_VALUE 
        == strncmp((char*)&g_canfdg_rx_frame.data[ZERO], (char*)&g_tx_fd_data[ZERO], CAN_FDG_DATA_LENGTH_CODE))
    {
        APP_PRINT("\r\nReceived data over FD Frame.\nCAN operation Successful, Data length = %d\r\n",
            g_canfdg_rx_frame.data_length_code);
        led_update(green);

        APP_PRINT("\r\nSending modified data over FD Frame now as acknowledgment for received FD data.\r\n");

        g_canfdg_rx_frame.data_length_code = CAN_FDG_DATA_LENGTH_CODE;
        g_canfdg_rx_frame.options = CANFDG_FRAME_OPTION_FD | CANFDG_FRAME_OPTION_BRS;

        /* Fill frame data that is to be sent in FD frame */
        for( uint16_t j = 0; j < SIZE_64; j++)
        {
            g_canfdg_rx_frame.data[j] = (uint8_t) (j + 5);
        }

        /* Transmission of data as acknowledgment */
        can_write_operation(g_canfdg_rx_frame);

        APP_PRINT("\r\nCAN transmission on FD Frame as acknowledgment is successful\r\n");
    }
    /* Impact to Board 1: Compare received data with the responded CANFDG data from Board 2 */
    else if(RESET_VALUE 
        == strncmp((char*)&g_canfdg_rx_frame.data[ZERO], (char*)&g_rx_fd_data[ZERO], CAN_FDG_DATA_LENGTH_CODE))
    {
        APP_PRINT("\r\nReceived Acknowledgment for FD Frame.\r\nCAN operation Successful, Data length = %d\r\n",
            g_canfdg_rx_frame.data_length_code);

        led_update(green);

        APP_PRINT("\r\nPlease enter any key on Terminal Application to initiate CAN transmission.\r\n");
    }
    else /* Wrong MSG Received */
    {
        APP_ERR_PRINT("\r\nCAN data mismatch\nCAN operation failed\r\n");
        led_update(red);
        APP_ERR_TRAP(true);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is to read Channel status info and read data.
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
void can_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Get the status information for CAN transmission */
    err = R_CANFDG_InfoGet(&g_canfdg0_ctrl, &g_can_rx_info);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCAN InfoGet API FAILED");
        led_update(red);
        canfdg_deinit();
        APP_ERR_TRAP(err);
    }

    /* Check if the data is received in FIFO */
    if(g_can_rx_info.rx_mb_status)
    {
        /* Read the input frame received */
        err = R_CANFDG_Read(&g_canfdg0_ctrl, ZERO, &g_canfdg_rx_frame);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nCAN Read API FAILED");
            led_update(red);
            canfdg_deinit();
            APP_ERR_TRAP(err);
        }

        /* Check if the transmitted and received data are same and send ACK accordingly */
        can_data_check_operation();
    }
    else
    {
        /* Do Nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is to update data buffer that is to be compared with transmitted FD frame data
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void can_fdg_data_update(void)
{
    /* Fill frame data to be compared with data transmitted on CANFDG frame */
    for( uint16_t i = 0; i < SIZE_64; i++)
    {
        g_tx_fd_data[i]          = (uint8_t) (i + 1);
    }
    for(uint16_t j = 0; j < SIZE_64; j++)
    {
        g_rx_fd_data[j] = (uint8_t) (j + 5);
    }
}

/*******************************************************************************************************************//**
 * @brief This function updates led state as per operation status
 * @param[in]  led_state      Selects which led has to be made high
 * @retval     None
 **********************************************************************************************************************/
static void led_update(led_state_t led_state)
{
    switch(led_state)
    {
        case red:
        {
            /* Red LED state is made high to show error state */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_RED, BSP_IO_LEVEL_HIGH);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_GREEN, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_BLUE, BSP_IO_LEVEL_LOW);
            /* Delay */
            R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
            break;
        }
        case green:
        {
            /* Green LED state is made high to show successful state */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_RED, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_GREEN, BSP_IO_LEVEL_HIGH);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_BLUE, BSP_IO_LEVEL_LOW);
            /* Delay */
            R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
            break;
        }
        case blue:
        {
            /* Blue LED state is made high to show operation is in progress */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_RED, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_GREEN, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) LED_BLUE, BSP_IO_LEVEL_HIGH);
            /* Delay */
            R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
            break;
        }
        default:
        {
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief CAN FD callback function.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void canfdg0_callback(can_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case CAN_EVENT_TX_COMPLETE:
        {
            g_canfdg_tx_complete = true;         /* Set flag bit */
            break;
        }
        /* Currently driver don't support this. This is unreachable code for now. */
        case CAN_EVENT_RX_COMPLETE:
        {
            g_canfdg_rx_complete = true;
            break;
        }
        case CAN_EVENT_ERR_WARNING:             /* Error warning event */
        case CAN_EVENT_ERR_PASSIVE:             /* Error passive event */
        case CAN_EVENT_ERR_BUS_OFF:             /* Error Bus Off event */
        case CAN_EVENT_BUS_RECOVERY:            /* Bus recovery error event */
        case CAN_EVENT_MAILBOX_MESSAGE_LOST:    /* Overwrite/overrun error event */
        case CAN_EVENT_ERR_BUS_LOCK:            /* Bus lock detected (32 consecutive dominant bits) */
        case CAN_EVENT_ERR_CHANNEL:             /* Channel error has occurred */
        case CAN_EVENT_TX_ABORTED:              /* Transmit abort event */
        case CAN_EVENT_ERR_GLOBAL:              /* Global error has occurred */
        case CAN_EVENT_FIFO_MESSAGE_LOST:       /* Receive FIFO overrun */
        case CAN_EVENT_TX_FIFO_EMPTY:           /* Transmit FIFO is empty */
        {
            g_canfdg_err_status = true;          /* Set flag bit */
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function is to de-initializes the CANFDG module
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
void canfdg_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close CANFDG channel */
    err = R_CANFDG_Close(&g_canfdg0_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n **CANFDG Close API failed**");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup can_fd_ep)
 **********************************************************************************************************************/
