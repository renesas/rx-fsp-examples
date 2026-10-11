
/***********************************************************************************************************************
 * File Name    : pcif_incremental_ep.h
 * Description  : This file contains the function declarations and definitions for PCIF incremental mode operations.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PCIF_INCREMENTAL_EP_H_
#define PCIF_INCREMENTAL_EP_H_

#include "common_utils.h"

/* Global functions prototype */
fsp_err_t set_resolution_ratio(pcif_ctrl_t * const p_ctrl,
                               pcif_cfg_t const * const p_cfg,
                               uint8_t resolution_ratio);
fsp_err_t reset_pcif(pcif_ctrl_t * const p_ctrl);

#endif /* PCIF_INCREMENTAL_EP_H_ */


