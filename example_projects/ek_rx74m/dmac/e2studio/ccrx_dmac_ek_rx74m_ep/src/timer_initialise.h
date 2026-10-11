/***********************************************************************************************************************
 * File Name    : timer_initialise.h
 * Description  : Contains data structures and functions used in timer_initialise.c
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef TIMER_INITIALISE_H_
#define TIMER_INITIALISE_H_

/* GPT timer initialization function */
fsp_err_t gpt_timer_init(void);

/* GPT timer deinitialisation function */
void gpt_timer_deinit(void);

/* CMT timer initialization function */
fsp_err_t cmt_timer_init(void);

/* CMT timer deinitialisation function */
void cmt_timer_deinit(void);

#endif
