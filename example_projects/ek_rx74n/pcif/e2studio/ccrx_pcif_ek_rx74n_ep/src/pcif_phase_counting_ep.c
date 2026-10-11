/***********************************************************************************************************************
 * File Name    : pcif_phase_counting_ep.c
 * Description  : Contains functions for PCIF phase counting mode operations including counter configuration,
 *                stop control, and callback handling for encoder position tracking.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "pcif_phase_counting_ep.h"

/* Global variables */
_Bool g_error_flag   = false;
_Bool g_success_flag = false;
pcif_error_t pcif_error_status = PCIF_ERROR_NONE;
pcif_phc_counter_info_t  g_counter_data = { .new_position = 0, .edge_count = 500};

/*******************************************************************************************************************//**
 *  @brief       Stops the PCIF phase counting operation and closes the PCIF and ELC modules.
 *  @param[in]   p_ctrl   Pointer to the PCIF control structure.
 *  @retval      FSP_SUCCESS upon successful operation, error code otherwise.
 **********************************************************************************************************************/
fsp_err_t stop_counter(pcif_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* If no error occurs, stop PCIF operation. */
    if (!g_error_flag)
    {
        /* Stop PCIF operation if no error occurs */
        err = R_PCIF_Stop(p_ctrl, PCIF_STOP_MODE_IMMEDIATE);
        if (err != FSP_SUCCESS)
        {
            return err;
        }

        /* Close PCIF module */
        err = R_PCIF_Close(p_ctrl);
        if (err != FSP_SUCCESS)
        {
            return err;
        }

        /* Close ELC module */
        err = R_ELC_Close(&g_elc_ctrl);
        if (err != FSP_SUCCESS)
        {
            return err;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Sets the counter data and starts PCIF phase counting operation.
 *
 *  @note        PCIF-specific error conditions reported by the callback are converted to the closest
 *               matching FSP error codes before being returned by this API.
 *
 *  @param[in]   p_ctrl         Pointer to the PCIF control structure.
 *  @param[in]   p_cfg          Pointer to the PCIF configuration structure.
 *  @param[in]   counter_data   Pointer to the counter data to be applied.
 *
 *  @retval      FSP_SUCCESS                   Successful operation.
 *  @retval      FSP_ERR_TIMEOUT               Timeout waiting for callback completion.
 *  @retval      FSP_ERR_OVERFLOW              PCIF edge-count overflow detected.
 *  @retval      FSP_ERR_INVALID_HW_CONDITION  PCIF hardware-related error detected.
 *  @retval      Other fsp_err_t               Error returned by underlying FSP APIs.
 **********************************************************************************************************************/
fsp_err_t set_counter_data(pcif_ctrl_t * const p_ctrl, 
                           pcif_cfg_t const * const p_cfg,
                           pcif_phc_counter_info_t const * const counter_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t timeout_ms = PCIF_DELAY_MS;
    pcif_instance_ctrl_t * p_instance_ctrl = (pcif_instance_ctrl_t *) p_ctrl;

    /* Initialize error flag to false. This gets set to true in PCIF callback upon error occurs. */
    g_error_flag   = false;
    g_success_flag = false;

    /* Update counter data */
    g_counter_data.new_position = counter_data->new_position;
    g_counter_data.edge_count = counter_data->edge_count;

    if (p_instance_ctrl->state == PCIF_STATE_STOPPED)
    {
        /* PCIF module Open */
        err = R_PCIF_Open(p_ctrl, p_cfg);
        if (err != FSP_SUCCESS)
        {
            return err;
        }

        /* PCIF module Start */
        err = R_PCIF_Start(p_ctrl);
        if (err != FSP_SUCCESS)
        {
            return err;
        }

        /* ELC module Open */
        err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
        if (err != FSP_SUCCESS)
        {
            return err;
        }

        /* ELC module Enable */
        err = R_ELC_Enable(&g_elc_ctrl);
        if (err != FSP_SUCCESS)
        {
            return err;
        }
    }

    /* Since there is nothing else to do, block until the callback triggers */
    while (!g_success_flag && !g_error_flag && timeout_ms)
    {
        R_BSP_SoftwareDelay(PCIF_SOFTWARE_DELAY_MS, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;
    }

    /* If timeout */
    if (0U == timeout_ms)
    {
        return FSP_ERR_TIMEOUT;
    }
    else if (g_error_flag)
    {

        switch (pcif_error_status)
        {
            case PCIF_ERROR_EDGE_COUNTING_OVERFLOW:
            {
                return FSP_ERR_OVERFLOW;
            }

            case PCIF_ERROR_CARRIER_PERIOD:
            case PCIF_ERROR_INTERNAL_EDGE_COUNTING:
            case PCIF_ERROR_OLD_EDGE_COUNTING:
            {
                return FSP_ERR_INVALID_HW_CONDITION;
            }

            default:
                return FSP_ERR_INTERNAL;
        }
    }
    else
    {
        /* Do next loop */
        g_success_flag = false;
        g_error_flag   = false;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 *  @brief       Callback function for PCIF phase counting mode carrier period event.
 *  @param[in]   p_args   Pointer to the PCIF callback arguments.
 *  @retval      None
 **********************************************************************************************************************/
void pcif_phase_counting_callback (pcif_callback_args_t * p_args)
{
    if (PCIF_ERROR_NONE == p_args->error)
    {
        R_PCIF_SetCounter(&g_pcif_phase_counting_mode_ctrl, &g_counter_data, true);
        g_success_flag = true;
        /* Do something with p_context if any */
    }
    else /* Error occurs */
    {
        g_error_flag = true;
        /* Check the error type here for additional handling if required. */
        pcif_error_status = p_args->error;
    }
}
