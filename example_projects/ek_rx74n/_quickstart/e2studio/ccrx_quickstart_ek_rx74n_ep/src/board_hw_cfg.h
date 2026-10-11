/*
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**********************************************************************************************************************
 * File Name    : board_hw_cfg.h
 * Version      : 1.0
 * Description  : Board hardware helper functions and definitions
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2026 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "hal_data.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef BOARD_HW_CFG_H_
#define BOARD_HW_CFG_H_

#ifndef BOARD_INIT_H
#include "rx74n_ek\board_leds.h"
#endif

/* LEDs defined in the FSP Board Support file board_leds.c */
extern const bsp_leds_t g_bsp_leds;

/* Switch state OFF Configuration */
#define OFF_CFG_       (BSP_IO_LEVEL_LOW)

/* Switch state ON Configuration */
#define ON_CFG_        (BSP_IO_LEVEL_HIGH)

/* BLUE LED Configuration */
#define BLUE_CFG_      (BSP_LED_LED1)
/* GREEN LED Configuration */
#define GREEN_CFG_     (BSP_LED_LED2)
/* RED LED Configuration */
#define RED_CFG_       (BSP_LED_LED3)

/* TURN RED LED ON Macro */
#define TURN_RED_ON_CFG_    do {\
                            R_BSP_PinAccessEnable(); \
                            R_BSP_PinWrite(g_bsp_leds.p_leds[RED_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();\
                            } while(0)
/* TURN RED LED OFF Macro */
#define TURN_RED_OFF_CFG_   do {\
                            R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[RED_CFG_], OFF_CFG_);\
                            R_BSP_PinAccessDisable();\
                            } while(0)
/* TURN GREEN LED ON Macro */
#define TURN_GREEN_ON_CFG_  do {\
                            R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();\
                            } while(0)
/* TURN GREEN LED OFF Macro */
#define TURN_GREEN_OFF_CFG_ do {\
                            R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN_CFG_], OFF_CFG_); \
                            R_BSP_PinAccessDisable();\
                            } while(0)
/* TURN BLUE LED ON Macro */
#define TURN_BLUE_ON_CFG_   do {\
                            R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();\
                            } while(0)

/* TURN BLUE LED OFF Macro */
#define TURN_BLUE_OFF_CFG_  do {\
                            R_BSP_PinAccessEnable(); \
                            R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE_CFG_], OFF_CFG_);\
                            R_BSP_PinAccessDisable();\
                            } while(0)

/* TURN GREEN LED ON to represent system OK Macro */
#define SYSTEM_OK_CFG_      do {\
                            TURN_GREEN_ON_CFG_; \
                            TURN_RED_OFF_CFG_;\
                            } while(0)

/* TURN RED LED ON to represent system ERROR Macro
 * Note GCC compile time __asm used */
#define SYSTEM_ERROR_CFG_   do {\
                            TURN_GREEN_OFF_CFG_; \
                            TURN_RED_ON_CFG_; \
                            __BKPT();\
                            } while(0)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* BOARD_HW_CFG_H_ */
