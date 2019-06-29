/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include "includes.h"
#include <string.h>
#include <stdio.h> 
#include <stdlib.h> 
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
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum {FALSE = 0, TRUE = !FALSE} bool;

#define  BIT_00                                     0x01u
#define  BIT_01                                     0x02u
#define  BIT_02                                     0x04u
#define  BIT_03                                     0x08u
#define  BIT_04                                     0x10u
#define  BIT_05                                     0x20u
#define  BIT_06                                     0x40u
#define  BIT_07                                     0x80u

#define  BIT_08                                   0x0100u
#define  BIT_09                                   0x0200u
#define  BIT_10                                   0x0400u
#define  BIT_11                                   0x0800u
#define  BIT_12                                   0x1000u
#define  BIT_13                                   0x2000u
#define  BIT_14                                   0x4000u
#define  BIT_15                                   0x8000u

#define  BIT_16                               0x00010000u
#define  BIT_17                               0x00020000u
#define  BIT_18                               0x00040000u
#define  BIT_19                               0x00080000u
#define  BIT_20                               0x00100000u
#define  BIT_21                               0x00200000u
#define  BIT_22                               0x00400000u
#define  BIT_23                               0x00800000u

#define  BIT_24                               0x01000000u
#define  BIT_25                               0x02000000u
#define  BIT_26                               0x04000000u
#define  BIT_27                               0x08000000u
#define  BIT_28                               0x10000000u
#define  BIT_29                               0x20000000u
#define  BIT_30                               0x40000000u
#define  BIT_31                               0x80000000u

#define BIT_ALL                              0xFFFFFFFFu
#define BIT_NONE                           0x00000000u
/*!
Message identifier type.
*/
typedef uint32_t MsgId;
/*!
Message type.
*/
typedef const void *Msg;

typedef struct     MsgId_s
{
    MsgId id;
    Msg    msg;
}MsgId_t,*pMsgId_t;



#define APP_MSG_BASE                      0x0000
#define RS485_MSG_BASE                    0x0100
#define GPRS_MSG_BASE                     0x0200
#define LOCK_MSG_BASE                     0x0300
#define HEART_MSG_BASE                    0x0400
#define RETURN_MSG_BASE                   0x0500




void MXUsart1UartInit(void);
void MXUsart3UartInit(void);
void MXUsart4UartInit(void);
void MXUsart5UartInit(void);
void MXUsart6UartInit(void);
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
typedef struct SysMsgUart_s
{
  MsgId id;
  Msg    msg;
}SysMsgUart_t;

typedef struct SysMsgThread_s
{
  MsgId id;
  Msg    msg;
}SysMsgThread_t;

typedef enum SysMsgSrc_e
{
    SysMsgSrc_Uart,
    SysMsgSrc_Thread,    
}SysMsgSrc_t;

typedef struct SysMsg_s
{
    SysMsgSrc_t src;
    SysMsgUart_t uart;
    SysMsgThread_t thread;
}SysMsg_t;




#define NAME_TASK_APP "appTask"
#define PRIO_TASK_APP osPriorityNormal
#define STACK_SIZE_TASK_APP 256
void AppTask(void * argument);
extern osThreadId_t appTaskHandle;

#define NAME_MSG_APP "appMsg"
#define APP_MSG_NUM 8
extern osMessageQueueId_t appMsgHandle;
extern SysMsg_t appMsg;

#define NAME_TASK_GPRS "gprsTask"
#define PRIO_TASK_GPRS osPriorityNormal
#define STACK_SIZE_TASK_GPRS 256
void GprsTask(void * argument);
extern osThreadId_t gprsTaskHandle;


#define NAME_MSG_GPRS "gprsMsg"
#define GPRS_MSG_NUM 100
extern osMessageQueueId_t gprsMsgHandle;
extern SysMsg_t gprsMsg;



#define NAME_TASK_HEART "heartTask"
#define PRIO_TASK_HEART osPriorityNormal
#define STACK_SIZE_TASK_HEART 256
void HeartTask(void * argument);
extern osThreadId_t heartTaskHandle;

extern osMutexId_t gprsMutexHandle;


#define NAME_MSG_HEART "heartMsg"
#define HEART_MSG_NUM 8
extern osMessageQueueId_t heartMsgHandle;



#define NAME_TASK_LOCK "lockTask"
#define PRIO_TASK_LOCK osPriorityNormal
#define STACK_SIZE_TASK_LOCK 256
void LockTask(void * argument);
extern osThreadId_t lockTaskHandle;


#define NAME_MSG_LOCK "lockMsg"
#define LOCK_MSG_NUM 8
extern osMessageQueueId_t lockMsgHandle;



#define NAME_TASK_RETURN "returnTask"
#define PRIO_TASK_RETURN osPriorityNormal
#define STACK_SIZE_TASK_RETURN 256
void ReturnTask(void * argument);
extern osThreadId_t returnTaskHandle;


#define NAME_MSG_RETURN "returnMsg"
#define RETURN_MSG_NUM 8
extern osMessageQueueId_t returnMsgHandle;


#define NAME_TASK_RS485 "rs485Task"
#define PRIO_TASK_RS485 osPriorityNormal
#define STACK_SIZE_TASK_RS485 256
void Rs485Task(void * argument);
extern osThreadId_t rs485TaskHandle;


#define NAME_MSG_RS485 "rs485Msg"
#define RS485_MSG_NUM 8
extern osMessageQueueId_t rs485MsgHandle;
extern SysMsg_t rs485Msg;

#define NAME_TASK_RS485_TX_POLL "rs485TxPollTask"
#define PRIO_TASK_RS485_TX_POLL osPriorityNormal
#define STACK_SIZE_TASK_RS485_TX_POLL 256
void Rs485TxPollTask(void * argument);
extern osThreadId_t rs485TxPollTaskHandle;

#define NAME_TASK_RS485_RX_POLL "rs485RxPollTask"
#define PRIO_TASK_RS485_RX_POLL osPriorityNormal
#define STACK_SIZE_TASK_RS485_RX_POLL 256
void Rs485RxPollTask(void * argument);
extern osThreadId_t rs485RxPollTaskHandle;






#define NAME_TASK_MSG "msgTask"
#define PRIO_TASK_MSG osPriorityNormal





#if 0

#define PRIO_TASK_GPRS osPriorityNormal
#define INSTANCES_SIZE_TASK_GPRS 0
#define STACK_SIZE_TASK_GPRS 128
void GprsTask(void const * argument);
extern osThreadId gprsTaskHandle;




#define PRIO_TASK_RS485 osPriorityNormal
#define INSTANCES_SIZE_TASK_RS485 0
#define STACK_SIZE_TASK_RS485 128
void RS485Task(void const * argument);
extern osThreadId rs485TaskHandle;

extern osMessageQId rs485MsgHandle;
#define RS485_QUEUE_NUM (10u)


#define PRIO_TASK_LOCK osPriorityNormal
#define INSTANCES_SIZE_TASK_LOCK 0
#define STACK_SIZE_TASK_LOCK 128
void LockTask(void const * argument);
extern osThreadId lockTaskHandle;

extern osMessageQId lockMsgHandle;
#define LOCK_QUEUE_NUM (10u)


#define PRIO_TASK_HEART osPriorityNormal
#define INSTANCES_SIZE_TASK_HEART 0
#define STACK_SIZE_TASK_HEART 128
void HeartTask(void const * argument);
extern osThreadId heartTaskHandle;

extern osMessageQId heartMsgHandle;
#define HEART_QUEUE_NUM (10u)


#define PRIO_TASK_RETURN osPriorityNormal
#define INSTANCES_SIZE_TASK_RETURN 0
#define STACK_SIZE_TASK_RETURN 128
void ReturnTask(void const * argument);
extern osThreadId returnTaskHandle;

extern osMessageQId returnMsgHandle;
#define RETURN_QUEUE_NUM (10u)

#define PRIO_TASK_PACKET osPriorityNormal
#define INSTANCES_SIZE_TASK_PACKET 0
#define STACK_SIZE_TASK_PACKET 128
void PacketTask(void const * argument);
extern osThreadId packetTaskHandle;

extern osMessageQId appMsgHandle;
#endif

#define APP_FLAG_MASK_GPRS_INIT BIT_00
#define APP_FLAG_MASK_RS485_INIT BIT_01


typedef enum USER_ERROR_CODE
{
    USER_ERROR_NONE = 0X00000000,
    USER_ID_NO_SUPPORT = 0X00000001,
                     
}USER_ERROR_CODE;


typedef enum AppMsgId_e
{
  APP_MSG_Idle = APP_MSG_BASE,
  APP_MSG_Start,  
  APP_MSG_SysInit,
 


  APP_MSG_InitFinsh,
  APP_MSG_End
}AppMsgId_t;

typedef enum AppStatus_e
{
    poweringOff,
    poweringOn,    
    initialising,       
    initialised,  
    taskStarted,      
    hardwareReady,         


}AppStatus_t;


typedef struct AppTaskData_s
{
    AppMsgId_t AppMsgId:16;  

    uint16_t Panic_Reason;
    bool gprsConnected;
    uint8_t version;
    AppStatus_t AppStatus;
    uint16_t Cabinet_No;
    uint8_t Lock_Total_Nums;
		
} AppTask_t, *pAppTask_t;




typedef USER_ERROR_CODE(*TaskHandlePtrWithUart)(SysMsgUart_t* msgUart);
typedef USER_ERROR_CODE(*TaskHandlePtrWithThread)(SysMsgThread_t* msgThread);




/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef rtcTime;
extern RTC_DateTypeDef rtcDate;
AppTask_t *GetApp(void);
SysMsg_t* SysMsgMallocThread(void);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_A_Pin GPIO_PIN_2
#define LED_A_GPIO_Port GPIOE
#define LED_B_Pin GPIO_PIN_3
#define LED_B_GPIO_Port GPIOE
#define PWRKEY_CON_Pin GPIO_PIN_6
#define PWRKEY_CON_GPIO_Port GPIOG
#define PWR_EN_Pin GPIO_PIN_7
#define PWR_EN_GPIO_Port GPIOG
#define RS485_DE_Pin GPIO_PIN_8
#define RS485_DE_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
