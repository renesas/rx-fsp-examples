/***********************************************************************************************************************
 * File Name    : pcif_incremental_ep.c
 * Description  : Contains functions for PCIF incremental mode operations including resolution ratio
 *                configuration and software reset functionality.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "pcif_incremental_ep.h"

/*******************************************************************************************************************//**
 *  @brief       Sets the resolution ratio for PCIF incremental mode by reconfiguring and restarting the module.
 *  @param[in]   p_ctrl             Pointer to the PCIF control structure.
 *  @param[in]   p_cfg              Pointer to the PCIF configuration structure.
 *  @param[in]   resolution_ratio   Resolution ratio value to be applied.
 *  @retval      FSP_SUCCESS upon successful operation, error code otherwise.
 **********************************************************************************************************************/
fsp_err_t set_resolution_ratio(pcif_ctrl_t * const p_ctrl,
                               pcif_cfg_t const * const p_cfg,
                               uint8_t resolution_ratio)
{
    fsp_err_t err = FSP_SUCCESS;
    static pcif_extended_cfg_t extend;
    static pcif_cfg_t cfg;

    /* Close PCIF module */
    err = R_PCIF_Close(p_ctrl);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* Update resolution ratio in incremental mode */
    cfg = *p_cfg;
    extend = *(pcif_extended_cfg_t *) p_cfg->p_extend;
    extend.resolution_ratio_setting = resolution_ratio;
    cfg.p_extend = &extend;

    /* Open PCIF module */
    err = R_PCIF_Open(p_ctrl, &cfg);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* Start PCIF module */
    err = R_PCIF_Start(p_ctrl);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       Resets the PCIF module by generating a software event.
 *  @param[in]   p_ctrl   Pointer to the PCIF control structure.
 *  @retval      FSP_SUCCESS upon successful operation, error code otherwise.
 **********************************************************************************************************************/
fsp_err_t reset_pcif(pcif_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Reset PCIF mode */
    err = R_PCIF_SoftwareEventGenerate(p_ctrl);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    return err;
}
