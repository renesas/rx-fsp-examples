/***********************************************************************************************************************
 * File Name    : elc_ep.c
 * Description  : Contains declarations of data structures and functions used in elc_ep.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "elc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup elc_ep
 * @{
 **********************************************************************************************************************/

/* Private functions */
static void elc_deinit(void);
static void gpt_deinit(gpt_instance_ctrl_t * p_ctrl);

extern bsp_leds_t g_bsp_leds;

/**********************************************************************************************************************
* Function implementations
**********************************************************************************************************************/
void elc_ep_entry(void)
{
    fsp_err_t err                              = FSP_SUCCESS;
    fsp_pack_version_t version                 = {RESET_VALUE};
    char user_input[TERM_BUFFER_SIZE]          = {NULL_CHAR};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                       (bsp_io_level_t) BSP_IO_LEVEL_LOW);
#if (USE_VIRTUAL_COM == 1)
    err = (fsp_err_t) TERM_INIT();
        if (FSP_SUCCESS != err)
        {
            /* Turn ON error LED to indicate uart_init failed */
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[ERR_LED],
                              (bsp_io_level_t) BSP_IO_LEVEL_HIGH);
            /* Error trap */
            ERROR_TRAP;
        }
#endif

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Open ELC driver */
    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* ELC module initialize failed */
        APP_ERR_PRINT("\r\n** R_ELC_Open API failed **\r\n");
        APP_ERR_TRAP(err);
    }

    /* Enable links between modules:
     *   1. ELC software event is used as the start source for a target GPT instance (e.g., GPTx, GPTy).
     *   2. Counter overflow event of a reference GPT instance (e.g., GPTy) is used as the stop source
     *   for another GPT instance (e.g., GPTx). */
    err = R_ELC_Enable(&g_elc_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();
        APP_ERR_PRINT("\r\n** R_ELC_Enable API failed **\r\n");
        APP_ERR_TRAP(err);
    }

    /* Open Timer GPT in PWM mode */
    err = R_GPT_Open(&g_timer_pwm_ctrl, &g_timer_pwm_cfg);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();
        APP_ERR_PRINT(OPEN_TIMER_PWM_FAIL);
        APP_ERR_TRAP(err);
    }

    /* Open Timer in One-Shot mode */
    err = R_GPT_Open(&g_timer_oneshot_ctrl, &g_timer_oneshot_cfg);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

        gpt_deinit(&g_timer_pwm_ctrl);

        APP_ERR_PRINT(OPEN_TIMER_FAIL);
        APP_ERR_TRAP(err);
    }

    /* Enable start and stop sources for Timer in PWM mode */
    err = R_GPT_Enable(&g_timer_pwm_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

        gpt_deinit(&g_timer_pwm_ctrl);
        gpt_deinit(&g_timer_oneshot_ctrl);
        APP_ERR_PRINT(ENABLE_TIMER_PULSE_FAIL);
        APP_ERR_TRAP(err);
    }

	/* Enable start source for Timer in One-Shot mode */
    err = R_GPT_Enable(&g_timer_oneshot_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

        gpt_deinit(&g_timer_pwm_ctrl);
        gpt_deinit(&g_timer_oneshot_ctrl);

        APP_ERR_PRINT(ENABLE_TIMER_FAIL);
        APP_ERR_TRAP(err);
    }
    /* On successful operations on ELC and GPT modules, the user is asked enter any key as Terminal Application input */
    APP_PRINT(STATUS_DISPLAY);
    /* Wait until data is available in Terminal Application */
    while (true)
    {
        if (APP_CHECK_DATA)
        {
            memset(&user_input[0], NULL_CHAR, BUFFER_SIZE_DOWN);
            APP_READ(user_input);
            err = R_ELC_SoftwareEventGenerate(&g_elc_ctrl, ELC_SOFTWARE_EVENT_0);
            if (FSP_SUCCESS != err)
            {
                elc_deinit();
                APP_ERR_PRINT("\r\n** R_ELC_SoftwareEventGenerate API failed **\r\n");
                APP_ERR_TRAP(err);
            }
            APP_PRINT(STATUS_DISPLAY);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened ELC module before the project ends up in an error trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void elc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ELC module */
    err = R_ELC_Close(&g_elc_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ELC close failure message */
        APP_ERR_PRINT("** R_ELC_Close API failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened GPT module before the project ends up in an error trap.
 * @param[IN]   p_ctrl    Pointer to instance control.
 * @retval      None
 **********************************************************************************************************************/
static void gpt_deinit(gpt_instance_ctrl_t * p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close GPT timer */
    err = R_GPT_Close(p_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* GPT close failure message */
        APP_ERR_PRINT("** R_GPT_Close API failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup elc_ep)
 **********************************************************************************************************************/
