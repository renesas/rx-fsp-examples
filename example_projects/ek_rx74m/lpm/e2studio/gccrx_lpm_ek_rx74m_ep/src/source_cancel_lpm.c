/***********************************************************************************************************************
 * File Name    : source_cancel_lpm.c
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "source_cancel_lpm.h"
#include "lpm_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup r_lpm_ep
 * @{
 **********************************************************************************************************************/

/**********************************************************************************************************************
* Public global variables declarations
**********************************************************************************************************************/
volatile bool g_source_flag = false;

/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/
static volatile bool g_external_irq_flag = false;
static volatile bool g_timer_flag = false;
static rtc_time_t g_set_time =  {
    .tm_hour    =  RESET_VALUE,
    .tm_isdst   =  RESET_VALUE,
    .tm_mday    =  RESET_VALUE,
    .tm_min     =  RESET_VALUE,
    .tm_mon     =  RESET_VALUE,
    .tm_sec     =  RESET_VALUE,
    .tm_wday    =  RESET_VALUE,
    .tm_yday    =  RESET_VALUE,
    .tm_year    =  RESET_VALUE,
   };


/**********************************************************************************************************************
* Function implementations
**********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief      Timer callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void timer_cancel_lpm_callback(rtc_callback_args_t *p_args)
{
    /* Check null pointer */
    if (NULL == p_args)
    {
        return;
    }

    /* Check source flag state */
    if (true == g_source_flag)
    {
        return;
    }


    /* Set detection flags in other LPM modes */
    g_source_flag = true;
    g_timer_flag = true;
}

/*******************************************************************************************************************//**
 *  @brief      External IRQ callback function
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void external_irq_cancel_lpm_callback(external_irq_callback_args_t *p_args)
{
    /* Check null pointer */
    if (NULL == p_args)
    {
        return;
    }

    /* Check External IRQ channel */
    if (g_external_irq_cancel_lpm_cfg.channel != p_args->channel)
    {
        return;
    }

    /* Check source flag state */
    if (true == g_source_flag)
    {
        return;
    }

    /* Set detection flags */
    g_source_flag = true;
    g_external_irq_flag = true;
}

/*******************************************************************************************************************//**
 *  @brief      This function will initializes modules used to cancel LPM mode
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize ICU External IRQ used to cancel LPM */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_cancel_lpm_ctrl, &g_external_irq_cancel_lpm_cfg);
    APP_ERR_RETURN(err, "R_ICU_ExternalIrqOpen API Failed\r\n");

    /* Initialize RTC module used to cancel LPM */
    err = R_RTC_Open(&g_timer_cancel_lpm_ctrl, &g_timer_cancel_lpm_cfg);
    APP_ERR_RETURN(err, "R_RTC_Open API Failed\r\n");

    g_set_time.tm_sec   = 0;
    g_set_time.tm_min   = 40;
    g_set_time.tm_hour  = 10;
    g_set_time.tm_mday  = 7;
    g_set_time.tm_mon   = 3;      // April = 4 → 3
    g_set_time.tm_year  = 126;    // 2026 - 1900
    g_set_time.tm_wday  = 0;
    g_set_time.tm_yday  = 0;
    g_set_time.tm_isdst = 0;

    err = R_RTC_CalendarTimeSet(&g_timer_cancel_lpm_ctrl, &g_set_time);

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will de-initializes modules used to cancel LPM mode
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
void source_cancel_lpm_deinit(void)
{
    /* Close RTC module used to cancel LPM if it is already open */
    if(g_timer_cancel_lpm_ctrl.open)
    {
        R_RTC_Close(&g_timer_cancel_lpm_ctrl);
    }

    /* Close ICU External IRQ module used to cancel LPM if it is already open */
    if(g_external_irq_cancel_lpm_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_cancel_lpm_ctrl);
    }

}

/*******************************************************************************************************************//**
 *  @brief      This function will enable modules to cancel LPM mode
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_cancel_lpm_ctrl);
    APP_ERR_RETURN(err, "R_ICU_ExternalIrqEnable API Failed\r\n");

    /* Set Periodic Irq Timer  */
    err = R_RTC_PeriodicIrqRateSet(&g_timer_cancel_lpm_ctrl,RTC_PERIODIC_IRQ_SELECT_2_SECOND);
    APP_ERR_RETURN(err, "R_RTC_PeriodicIrqRateSet API Failed\r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      This function will disable modules to cancel LPM mode
 *  @param      None
 *  @retval     FSP_SUCCESS    Upon successful operation
 *  @retval     Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/
fsp_err_t source_cancel_lpm_disable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqDisable(&g_external_irq_cancel_lpm_ctrl);
    APP_ERR_RETURN(err, "R_ICU_ExternalIrqEnable API Failed\r\n");


    /* Set Periodic Irq Timer  */
    err = R_RTC_PeriodicIrqRateSet(&g_timer_cancel_lpm_ctrl,RTC_PERIODIC_IRQ_SELECT_2_SECOND);
    APP_ERR_RETURN(err, "R_RTC_PeriodicIrqRateSet API Failed\r\n");

    return err;
}


/*******************************************************************************************************************//**
 *  @brief      This function will detect which module has canceled LPM mode
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void source_cancel_lpm_detect(void)
{
    if(true == g_timer_flag)
    {
        APP_PRINT(NAME_LPM_CANCEL_SOURCE_TIMER);
    }

    else if(true == g_external_irq_flag)
    {
        APP_PRINT(NAME_LPM_CANCEL_SOURCE_IRQ);
    }

    else
    {
        APP_PRINT(NAME_LPM_CANCEL_SOURCE_NOT_DETECTED);
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function will clears all source detection flags
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void source_cancel_lpm_flag_clear(void)
{
    g_external_irq_flag = false;
    g_timer_flag = false;
    g_source_flag = false;
}

#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
/*******************************************************************************************************************//**
 *  @brief      This function will perform a procedure after Deep SW Standby
 *  @param      None
 *  @retval     None
 **********************************************************************************************************************/
void source_internal_reset_handle(void)
{
    uint16_t external_irq_channel = RESET_VALUE;

    /* Get Deep Standby Interrupt flags from IRQ0 to IRQ15 */
    external_irq_channel = (uint16_t)(R_SYSTEM->DPSIFR1) << 8U;
    external_irq_channel |= (uint16_t)(R_SYSTEM->DPSIFR0);

    /* Check External IRQ Flag */
    if(external_irq_channel & (1U << (g_external_irq_cancel_lpm_cfg.channel)))
    {
        g_timer_flag = false;
        g_external_irq_flag = true;
    }

    /* Check Deep Standby Interrupt flag for  IWDT1 depending on board type */
    else if(R_SYSTEM->DPSIFR3_b.DIWDTIF)
    {
        g_timer_flag = true;
        g_external_irq_flag = false;
    }

    /* Source not detected */
    else
    {
        g_timer_flag = false;
        g_external_irq_flag = false;
    }

    /* Clear the IOKEEP bit after deep software standby */
    R_LPM_IoKeepClear(NULL);


    /* Set source flag to notify internal reset event */
    g_source_flag = true;
}
#endif

/*******************************************************************************************************************//**
 * @} (end addtogroup r_lpm_ep)
 **********************************************************************************************************************/
