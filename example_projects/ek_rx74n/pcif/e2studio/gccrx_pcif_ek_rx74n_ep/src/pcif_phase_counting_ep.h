/***********************************************************************************************************************
 * File Name    : pcif_phase_counting_ep.h
 * Description  : This file contains the function declarations and definitions for PCIF phase counting mode operations.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PCIF_PHASE_COUNTING_EP_H_
#define PCIF_PHASE_COUNTING_EP_H_

#include "common_utils.h"

/* Timeout (ms) to wait for the phase counting status to stabilize. */
#define PCIF_DELAY_MS               (100U)

/* Software delay step (ms) used while polling for the callback result. */
#define PCIF_SOFTWARE_DELAY_MS      (1U)


/* Global functions prototype */
fsp_err_t set_counter_data(pcif_ctrl_t * const p_ctrl,
                           pcif_cfg_t const * const p_cfg,
                           pcif_phc_counter_info_t const * const counter_data);
fsp_err_t stop_counter(pcif_ctrl_t * const p_ctrl);

#endif /* PCIF_PHASE_COUNTING_EP_H_ */
