/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/
#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__


#if defined(__cplusplus)
extern "C" {
#endif

/* __cplusplus */
/*!
 * @brief configure all pins for this demo/example
 *
 */

/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/
#include <string.h>
#include <stdio.h> 
#include <math.h> 
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_ll_tim.h"
#include "cmsis_os.h"
#include "SEGGER_RTT.h"
/*  SDK Included Files */
/*BSP */
#include "main.h"
#include "gprs.h"





#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
