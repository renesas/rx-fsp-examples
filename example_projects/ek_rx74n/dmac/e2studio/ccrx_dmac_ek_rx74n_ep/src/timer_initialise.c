/***********************************************************************************************************************
 * File Name    : timer_initialise.c
 * Description  : Contains data structures and functions used in timer_initialise.c
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "timer_initialise.h"

/*******************************************************************************************************************//**
 * @addtogroup timer_initialise
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief       GPT timer initialisation function
 *  @param[in]   None
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t gpt_timer_init(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open the gpt timer */
    fsp_err = R_GPT_Open(&g_gpt_ctrl, &g_gpt_cfg);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("\r\nGPT timer open failed. Necessary cleanup is done. \r\n");
        return fsp_err;
    }

    /* Start GPT timer */
    fsp_err = R_GPT_Start(&g_gpt_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        fsp_err_t err = FSP_SUCCESS;
        err = R_GPT_Close(&g_gpt_ctrl);
        /* Handle error in-case of failure */
        if (FSP_SUCCESS != err)
        {
            /* Terminal Application failure message */
            APP_ERR_PRINT("GPT timer close failed.\r\n");
        }
        /* Terminal Application failure message */
        APP_ERR_PRINT("\r\nGPT timer start failed. Necessary cleanup is done. \r\n");
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief       CMT timer initialization function
 *  @param[in]   None
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t cmt_timer_init(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open and Configure the CMT timer */
    fsp_err = R_CMT_Open(&g_cmt_ctrl, &g_cmt_cfg);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Terminal Application failure message */
        APP_ERR_PRINT("CMT timer open failed. Necessary cleanup is done. \n\n");
        return fsp_err;
    }

    /* Start the CMT timer. */
    fsp_err = R_CMT_Start(&g_cmt_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        fsp_err_t err = FSP_SUCCESS;
        err = R_CMT_Close(&g_cmt_ctrl);
        /* Handle error in-case of failure */
        if (FSP_SUCCESS != err)
        {
            /* Terminal Application failure message */
            APP_ERR_PRINT("CMT timer close failed. \r\n");
        }

        /* Terminal Application failure message */
        APP_ERR_PRINT("\r\nCMT timer start failed. Necessary cleanup is done. \r\n");
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief       GPT timer deinitialisation function
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void gpt_timer_deinit(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Close the GPT timer */
    fsp_err = R_GPT_Close(&g_gpt_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Terminal Application failure message */
        APP_ERR_PRINT("GPT timer close failed.\n\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       CMT timer deinitialisation function
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void cmt_timer_deinit(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Close the CMT timer */
    fsp_err = R_CMT_Close(&g_cmt_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Terminal Application failure message */
        APP_ERR_PRINT("CMT timer close failed.\n\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup timer_initialise)
 **********************************************************************************************************************/
