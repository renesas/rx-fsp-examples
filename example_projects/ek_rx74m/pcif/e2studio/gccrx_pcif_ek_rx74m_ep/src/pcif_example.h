/***********************************************************************************************************************
 * File Name    : pcif_example.h
 * Description  : This file contains data structures and functions used in pcif_example.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


#ifndef PCIF_EXAMPLE_H_
#define PCIF_EXAMPLE_H_


/*******************************************************************************************************************//**
 * Macro definition Section
 **********************************************************************************************************************/
/* This macro is used to display the EP information */
#define PCIF_EP_INFO                "\r\nThis example project demonstrates the PCIF module by generating and   " \
                                    "\r\nreading ABZ encoder signals. Phase Counting mode is used to generate   " \
                                    "\r\nABZ encoder signals and simulate encoder operation. Incremental mode  " \
                                    "\r\nis used to read ABZ encoder signals and generate output waveforms   "\
                                    "\r\nwith configurable resolution ratios.                              \r\n"

/* This macro is used to display the Main menu */
#define EP_MENU_PCIF_MENU           "\r\n-----------------------------------------------------    " \
                                    "\r\n   SELECT PCIF mode     \r\n                             " \
                                    "\r\n   [1] Phase Counting Mode                               " \
                                    "\r\n   [2] Incremental Mode                                  " \
                                    "\r\n   [3] Exit                                              " \
                                    "\r\n-----------------------------------------------------\r\n"
#define ENTER_MENU_OPTION           "\r\nPlease enter your option (Press any other key to print the Main menu):\r\n"
#define PHASE_COUNTING_MODE         (1U)
#define INCREMENTAL_MODE            (2U)
#define EXIT                        (3U)

/* This macro is used to display PCIF status */
#define PRINT_CURRENT_POSITION      "\r\nCurrent position value : %d \r\n"
#define PRINT_POSITION_WRITE        "Position write value   : %d \r\n"
#define PRINT_INPUT_COUNTER         "Input counter value    : %d \r\n"


/* This macro is used to display incremental mode menu */
#define INCREMENTAL_MODE_MENU       "\r\n------------------------------------------------------ " \
                                    "\r\n        SELECT INCREMENTAL MODE OPERATION              " \
                                    "\r\n        [1] SET RESOLUTION RATIO                       " \
                                    "\r\n        [2] GET ENCODER STATUS                         " \
                                    "\r\n        [3] RESET PCIF                                 " \
                                    "\r\n        -----------------------------------------------" \
                                    "\r\n        [4] Go back to select mode                     " \
                                    "\r\n-------------------------------------------------------" \
                                    "\r\nPlease enter your option [1, 2, 3, 4]:             \r\n"
/* Incremental mode menu options */
#define INCR_SET_RESOLUTION_RATIO   (1U)
#define INCR_GET_ENCODER_STATUS     (2U)
#define INCR_RESET_PCIF             (3U)
#define INCR_GO_BACK                (4U)

/* This macro is used to display phase counting mode menu */
#define PHASE_COUNTING_MODE_MENU    "\r\n------------------------------------------------------ " \
                                    "\r\n        SELECT PHASE COUNTING OPERATION                " \
                                    "\r\n        [1] SET ENCODER COUNT                          " \
                                    "\r\n        [2] GET ENCODER STATUS                         " \
                                    "\r\n        [3] STOP ENCODER COUNT                         " \
                                    "\r\n        -----------------------------------------------" \
                                    "\r\n        [4] Go back to select mode                     " \
                                    "\r\n-------------------------------------------------------" \
                                    "\r\nPlease enter your option [1, 2, 3, 4]:             \r\n"
/* Phase counting mode menu options */
#define PHC_SET_ENCODER_COUNT       (1U)
#define PHC_GET_ENCODER_STATUS      (2U)
#define PHC_STOP_ENCODER_COUNT      (3U)
#define PHC_GO_BACK                 (4U)

/* Reset value for user option / buffers */
#define INIT_VALUE                  (0U)


/* This macro is used to indicate the buffer size */
#define BUFF_SIZE                   (15U)

/* Global functions prototype */
void pcif_operation_entry(void);
void phase_counting_entry(void);
void incremental_entry(void);

#endif /* PCIF_EXAMPLE_H_ */
