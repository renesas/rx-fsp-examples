#include "bsp_api.h"

#if BSP_PZ_SECURE_BUILD
const bsp_ssc_handler_t g_bsp_pz_ssc_table[BSP_PZ_CFG_SSC_NUM] =
{
    [BSP_SSCID_SYSTEM_CLOCK_SET] = R_BSP_SystemCoreClockSet
};
#endif
