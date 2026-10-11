/***********************************************************************************************************************
 * File Name    : pcif_example.c
 * Description  : This file contains data structures and entry functions used in hal_entry().
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "pcif_incremental_ep.h"
#include "pcif_phase_counting_ep.h"
#include "pcif_example.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * Function Declaration Section
 **********************************************************************************************************************/
static int32_t get_user_input(void);
static void print_pcif_status(pcif_ctrl_t * const p_ctrl, pcif_status_t * const phase_counting_status);
static void pcif_init(pcif_ctrl_t * const p_ctrl, pcif_cfg_t const * const p_cfg);
static void pcif_deinit(pcif_ctrl_t * const p_ctrl);
static void handle_error(fsp_err_t err, char *err_str);

/*******************************************************************************************************************//**
 * Global variable Section
 **********************************************************************************************************************/
extern bsp_leds_t g_bsp_leds;
_Bool incremental_mode_status = false;

/*******************************************************************************************************************//**
 *  @brief       Main entry function for PCIF example project. Initializes PCIF, displays menu,
 *               and handles user selection to execute different phase counting modes.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void pcif_operation_entry(void)
{
    fsp_err_t            err;

    /* Version information */
    fsp_pack_version_t version;

    /* Fetch FSP version to display on console */
    R_FSP_VersionGet(&version);

    /* User option */
    uint32_t user_option   = RESET_VALUE;

    /* Read buffer */
    char rByte[BUFF_SIZE]  = {RESET_VALUE};
    _Bool b_exit_demo = false;

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

    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(PCIF_EP_INFO);

    APP_PRINT(EP_MENU_PCIF_MENU);
    APP_PRINT(ENTER_MENU_OPTION);

    while (!b_exit_demo)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte); /* Read user input from terminal */

            /* Convert ASCII input to integer option */
            user_option = (uint32_t)(atoi((char *)rByte));
            switch (user_option)
            {
                case PHASE_COUNTING_MODE:
                    /* Phase counting mode demo */
                    phase_counting_entry();
                    break;

                case INCREMENTAL_MODE:
                    /* Incremental mode demo */
                    incremental_entry();
                    break;

                case EXIT:
                    APP_PRINT("\r\n");
                    APP_PRINT("Exiting the Example project...\r\n");
                    b_exit_demo = true; /* Exit loop */
                    break;

                default:
                    APP_PRINT("Invalid menu option!!\r\n");
                    APP_PRINT(EP_MENU_PCIF_MENU);
                    APP_PRINT(ENTER_MENU_OPTION);
                    break;
            }

            /* PRINT PCIF menu */
            APP_PRINT(EP_MENU_PCIF_MENU);
            APP_PRINT(ENTER_MENU_OPTION);
        }
    }

    /* De-initialize the PCIF module */
    pcif_deinit(&g_pcif_phase_counting_mode_ctrl);
    pcif_deinit(&g_pcif_incremental_mode_ctrl);

    /* Close the driver */
    APP_PRINT("\r\nPCIF module has been closed!\r\n");
    APP_PRINT("Demo end here...\r\n");
    TERM_DEINIT(); /* De-initialize terminal */
}

/*******************************************************************************************************************//**
 *  @brief       Incremental mode demo. Displays the incremental menu and handles user selections
 *               (set resolution ratio, get encoder status, reset PCIF).
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void incremental_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Resolution ratio value */
    uint8_t resolution_ratio = RESET_VALUE;

    /* PCIF status  */
    pcif_status_t incremental_status;

    /* User option */
    uint32_t user_option   = RESET_VALUE;

    /* Condition for breaking the loop */
    char rByte[BUFF_SIZE]  = {RESET_VALUE};
    _Bool break_loop       = false;

    APP_PRINT(INCREMENTAL_MODE_MENU);

    /* Initialize pcif module */
    if (!incremental_mode_status)
    {
        pcif_init(&g_pcif_incremental_mode_ctrl, &g_pcif_incremental_mode_cfg);
        incremental_mode_status = true;
    }

    while (!break_loop)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte); /* Read user input from terminal */

            /* Convert ASCII input to integer option */
            user_option = (uint32_t)(atoi((char *)rByte));

            switch (user_option)
            {
                case INCR_SET_RESOLUTION_RATIO:
                    /* set resolution ratio */
                    /* Input to resolution ratio in incremental mode */
                    APP_PRINT("\r\nEnter input for resolution ratio : \r\n");
                    resolution_ratio = (uint8_t) (get_user_input());
                    err = set_resolution_ratio(&g_pcif_incremental_mode_ctrl, 
                                               &g_pcif_incremental_mode_cfg,
                                               resolution_ratio);
                    handle_error(err, "\r\nset_resolution_ratio function failed \r\n");
                    APP_PRINT(INCREMENTAL_MODE_MENU);
                    break;

                case INCR_GET_ENCODER_STATUS:
                    /* get and print PCIF status */
                    print_pcif_status(&g_pcif_incremental_mode_ctrl, &incremental_status);
                    APP_PRINT(INCREMENTAL_MODE_MENU);
                    break;

                case INCR_RESET_PCIF:
                    /* reset PCIF module by software event */
                    err = reset_pcif(&g_pcif_incremental_mode_ctrl);
                    handle_error(err, "\r\nreset_pcif function failed \r\n");
                    APP_PRINT("\r\nReset executed successfully.\r\n");
                    APP_PRINT(INCREMENTAL_MODE_MENU);
                    break;

                case INCR_GO_BACK:
                    break_loop = true;
                    break;

                default:
                    APP_PRINT(INCREMENTAL_MODE_MENU);
                    break;
            }

            /* Reset value */
            user_option = INIT_VALUE;
            memset(rByte, RESET_VALUE, BUFF_SIZE);
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       Phase counting mode demo. Displays the phase counting menu and handles user selections
 *               (set encoder count, get encoder status, stop encoder count).
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void phase_counting_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* PCIF counter data */
    pcif_phc_counter_info_t counter_data;

    /* PCIF status  */
    pcif_status_t phase_counting_status;

    /* User option */
    uint32_t user_option   = RESET_VALUE;

    /* Condition for breaking the loop */
    char rByte[BUFF_SIZE]           = {RESET_VALUE};
    _Bool break_loop                = false;

    APP_PRINT(PHASE_COUNTING_MODE_MENU);

    while (!break_loop)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte); /* Read user input from terminal */

            /* Convert ASCII input to integer option */
            user_option = (uint32_t)(atoi((char *)rByte));

            switch (user_option)
            {
                case PHC_SET_ENCODER_COUNT:
                    /* set new position and edge counter */
                    /* Input to new position in phase counting mode */
                    APP_PRINT("\r\nEnter input for new position : \r\n");
                    counter_data.new_position = (uint16_t) (get_user_input());

                    /* Input to edge count in phase counting mode */
                    APP_PRINT("\r\nEnter input for edge count : \r\n");
                    counter_data.edge_count = (int16_t) (get_user_input());

                    /* Configure New Position and Edge Count in Phase Counting Mode */
                    err = set_counter_data(&g_pcif_phase_counting_mode_ctrl, 
                                           &g_pcif_phase_counting_mode_cfg, 
                                           &counter_data);
                    handle_error(err, "\r\nphase_count_counter_set function failed \r\n");
                    APP_PRINT(PHASE_COUNTING_MODE_MENU);
                    break;

                case PHC_GET_ENCODER_STATUS:
                    /* get and print PCIF status */
                    print_pcif_status(&g_pcif_phase_counting_mode_ctrl, &phase_counting_status);
                    APP_PRINT(PHASE_COUNTING_MODE_MENU);
                    break;

                case PHC_STOP_ENCODER_COUNT:
                    /* stop PCIF immediately */
                    err = stop_counter(&g_pcif_phase_counting_mode_ctrl);
                    handle_error(err, "\r\nStop counter function failed \r\n");
                    APP_PRINT("\r\nStopped encoder successfully.\r\n");
                    APP_PRINT(PHASE_COUNTING_MODE_MENU);
                    break;

                case PHC_GO_BACK:
                    break_loop = true;
                    break;

                default:
                    APP_PRINT(PHASE_COUNTING_MODE_MENU);
                    break;
            }

            /* Reset value */
            user_option = INIT_VALUE;
            memset(rByte, RESET_VALUE, BUFF_SIZE);
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       Blocks until the user enters a value on the terminal and returns it as an integer.
 *  @param[in]   None
 *  @retval      int32_t  Value entered by the user (converted from ASCII).
 **********************************************************************************************************************/
static int32_t get_user_input(void)
{
    /* Converted user input value */
    int32_t result        = RESET_VALUE;

    /* Condition for breaking the loop */
    char rByte[BUFF_SIZE]           = {RESET_VALUE};
    _Bool break_loop                = false;

    while (!break_loop)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte); /* Read user input from terminal */

            /* get value */
            result = (int32_t)(atoi((char *)rByte));

            /* Reset value */
            memset(rByte, RESET_VALUE, BUFF_SIZE);
            break_loop = true;
        }
    }

    return result;
}

/*******************************************************************************************************************//**
 *  @brief       Retrieve and print the current PCIF status (position, counter, rotation direction).
 *  @param[in]   p_ctrl                  Pointer to the PCIF control structure.
 *  @param[in]   phase_counting_status   Pointer to the PCIF status structure to be filled.
 *  @retval      None
 **********************************************************************************************************************/
static void print_pcif_status(pcif_ctrl_t * const p_ctrl, pcif_status_t * const phase_counting_status)
{
    fsp_err_t err = FSP_SUCCESS;
    pcif_instance_ctrl_t * p_instance_ctrl = (pcif_instance_ctrl_t *) p_ctrl;

    /* Re-open PCIF module in phase counting mode */
    if (p_instance_ctrl->p_cfg->mode == PCIF_MODE_PHASE_COUNTING 
        && p_instance_ctrl->state == PCIF_STATE_STOPPED)
    {
        err = R_PCIF_Open(&g_pcif_phase_counting_mode_ctrl, &g_pcif_phase_counting_mode_cfg);
        handle_error(err, "\r\nR_PCIF_Open failed \r\n");
    }

    /* Get PCIF status */
    err = R_PCIF_StatusGet(p_ctrl, phase_counting_status);
    handle_error(err, "\r\nR_PCIF_StatusGet failed \r\n");

    /* Print current position value */
    APP_PRINT(PRINT_CURRENT_POSITION, (uint16_t)phase_counting_status->current_position_value);

    /* if phase counting mode, print position write value */
    if (p_instance_ctrl->p_cfg->mode == PCIF_MODE_PHASE_COUNTING)
    {
        APP_PRINT(PRINT_POSITION_WRITE, (uint16_t)phase_counting_status->position_write_value);
    }
    else /* if not, input counter value in incremental mode */
    {
        APP_PRINT(PRINT_INPUT_COUNTER, (uint8_t)phase_counting_status->input_counter_value);
    }

    /* Print rotation direction */
    if (phase_counting_status->rotation_direction == PCIF_ROTATION_DIRECTION_CW)
    {
        APP_PRINT("Rotation direction is Clockwise \r\n");
    }
    else
    {
        APP_PRINT("Rotation direction is Counter-Clockwise \r\n");
    }

     /* Close PCIF module in phase counting mode */
    if (p_instance_ctrl->p_cfg->mode == PCIF_MODE_PHASE_COUNTING 
        && p_instance_ctrl->state == PCIF_STATE_STOPPED)
    {
        err = R_PCIF_Close(&g_pcif_phase_counting_mode_ctrl);
        handle_error(err, "\r\nR_PCIF_Close failed \r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       Initialize PCIF module and open the PCIF driver.
 *  @param[in]   p_ctrl   Pointer to the PCIF control structure.
 *  @param[in]   p_cfg    Pointer to the PCIF configuration structure.
 *  @retval      None
 **********************************************************************************************************************/
static void pcif_init(pcif_ctrl_t * const p_ctrl, pcif_cfg_t const * const p_cfg)
{
    /* Open PCIF module */
    fsp_err_t err = R_PCIF_Open(p_ctrl, p_cfg);
    handle_error(err, "\r\n** PCIF initialization failed **\r\n");
}

/*******************************************************************************************************************//**
 *  @brief       De-initialize the PCIF module and close the PCIF driver.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
static void pcif_deinit(pcif_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    pcif_instance_ctrl_t * p_instance_ctrl = (pcif_instance_ctrl_t *) p_ctrl;

    /* Close PCIF module */
    if (p_instance_ctrl->p_cfg->mode == PCIF_MODE_INCREMENTAL)
    {
        err = R_PCIF_Close(p_ctrl);
        handle_error(err, "\r\nR_PCIF_Close API failed \r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       Handles an error if one occurs: closes all opened modules, prints the message, and traps the error.
 *  @param[in]   status    error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
