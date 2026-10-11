/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains variables and functions used in adc_ep.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "adc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup adc_ep
 * @{
 **********************************************************************************************************************/

/* Flag to notify that ADC scan is started, so start reading ADC */
volatile bool g_ready_to_read = false;

static uint16_t g_adc_b_data;

extern const adc_b_extended_cfg_t g_adc_cfg_extend;

/* Open the ADC module, configure and initiate the scan */
static fsp_err_t adc_scan_start(void);

/* Stop the ADC scan if the ADC is in continuous scan mode and then close the module */
static fsp_err_t adc_scan_stop(void);

static fsp_err_t adc_start_calibration(void);

/*******************************************************************************************************************//**
 * @brief     This function reads the command (input) from RTT and processes the command (input).
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful action, ADC start or ADC stop takes place
 * @retval Any Other Error code apart from FSP_SUCCESS  Unsuccessful start or stop
 **********************************************************************************************************************/
fsp_err_t read_process_input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    char readBuff[BUFF_SIZE] =  { RESET_VALUE };

    uint32_t rByte = APP_READ(readBuff);

    /* Function returns the converted integral number as an int value */
    int32_t inputRead = atoi((char *)readBuff);

    if (rByte > RESET_VALUE)
    {

        switch (inputRead)
        {
            case SCAN_START:
            {
               err = adc_scan_start();
            }
            break;
            case SCAN_STOP:
            {
               err = adc_scan_stop();
            }
            break;
            default:
            {
                /* Menu for User Selection */
                APP_PRINT(MENUOPTION1);
                APP_PRINT(MENUOPTION2);
                APP_PRINT(MENUOPTION3);
                APP_PRINT(MENUOPTION4);
            }
            break;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief    This function opens the ADC, configures and starts the scan
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful open, configure ADC, and start ADC scan or calibration
 * @retval Any Other Error code apart from FSP_SUCCESS  Unsuccessful open or configure or start
 **********************************************************************************************************************/
static fsp_err_t adc_scan_start(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    if (false == g_ready_to_read)
    {
        /* Open/Initialize ADC module */
        err = R_ADC_B_Open (&g_adc_ctrl, &g_adc_cfg);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_B_Open API failed ** \r\n");
            return err;
        }

        /* Configure the ADC scan parameters */
        err = R_ADC_B_ScanCfg (&g_adc_ctrl, &g_adc_scan_cfg);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_B_ScanCfg API failed ** \r\n");
            return err;
        };
        /* Calibrate the ADC */
        err = adc_start_calibration();

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** adc_start_calibration function failed ** \r\n");
            return err;
        }

        /* Start the ADC scan */
        err = R_ADC_B_ScanStart(&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_B_ScanStart API failed ** \r\n");
            return err;
        }
        APP_PRINT("\r\nADC Started Scan\r\n");
        /* Indication to start reading the ADC data */
        g_ready_to_read = true;
    }
    else
    {
        APP_PRINT("\r\nADC Scan already in progress\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief    This function stops the scanning of ADC
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successfully stopping the ADC scan and closing the ADC
 * @retval Any Other Error code apart from FSP_SUCCESS Unsuccessful stop or close of ADC
 **********************************************************************************************************************/
static fsp_err_t adc_scan_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Stop the scan if ADC scan is started in continuous scan mode else ignore */
    if((ADC_B_CONVERTER_MODE_SINGLE_SCAN != g_adc_cfg_extend.adc_b_converter_mode[0].mode)
        && (true == g_ready_to_read ))
        {
        err = R_ADC_B_ScanStop (&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_B_ScanStop API failed ** \r\n");
            return err;
        }

        APP_PRINT("\r\nADC Scan stopped\r\n");

        /* Reset to indicate stop reading the ADC data */
        g_ready_to_read = false;

        /* Close the ADC module*/
        deinit_adc_module();
    }
    else
    {
        APP_PRINT("\r\nStop command is not supported in Single Scan mode or User"
                " has not pressed Start Scan in Continuous mode \r\n");
    }
    APP_PRINT("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief    This function reads the ADC output data from the prescribed channel and checks ADC status
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successfully reading ADC data
 * @retval Any Other Error code apart from FSP_SUCCESS  Unsuccessful read or close of ADC
 **********************************************************************************************************************/
fsp_err_t adc_read_data(void)
{
    fsp_err_t err = FSP_SUCCESS;    /* Error status */

    /* Read the result */
    err = R_ADC_B_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_b_data);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_B_Read API failed ** \r\n");
        return err;
    }
    APP_PRINT("\r\nMCU Temperature Reading from ADC: %d\r\n", g_adc_b_data);

    /* In ADC single scan mode after reading the data, it stops. So reset the b_ready_to_read state to
     * avoid reading unnecessarily. Close the ADC module as it gets opened in start scan command. */
    if (ADC_B_CONVERTER_MODE_SINGLE_SCAN == g_adc_cfg_extend.adc_b_converter_mode[0].mode)
    {
        g_ready_to_read = false;

        /* Close the ADC module */
        deinit_adc_module();

        APP_PRINT("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
    }

    /* 1 Second Wait time between successive readings */
    R_BSP_SoftwareDelay (ADC_READ_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    return err;
}
/*******************************************************************************************************************//**
 * @brief    Close the ADC driver and handle the return closing API error to the Application.
 * @param[IN]   None
 * @retval None.
 **********************************************************************************************************************/
void deinit_adc_module(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the ADC driver */
    err = R_ADC_B_Close (&g_adc_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
        APP_ERR_PRINT("** R_ADC_B_Close API failed **  \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief    This function initiates the calibration
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful ADC calibration
 * @retval Any Other Error code apart from FSP_SUCCESS  Unsuccessful calibration failure
 **********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    adc_status_t adc_status;         /* To get ADC status */

    /* Initiate ADC calibration */
    err = R_ADC_B_Calibrate(&g_adc_ctrl, NULL);

    APP_PRINT("\r\nADC Calibration Started \r\n");

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_PRINT("\r\nR_ADC_Calibrate error: %d\r\n", err);
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_Calibrate API failed ** \r\n");
        return err;
    }
   do
    {
        /* Get the ADC status */
        err = R_ADC_B_StatusGet(&g_adc_ctrl, &adc_status);

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status Failure message */
            APP_ERR_PRINT("** R_ADC_StatusGet API failed ** \r\n");
            return err;
        }
        /* Wait here until the calibration. It takes 24msec to 780msec based on clock */
    } while (ADC_STATE_IDLE != adc_status.state);

    APP_PRINT("\r\nADC Calibration Successful \r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup adc_ep)
 **********************************************************************************************************************/
