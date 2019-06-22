/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "gprs.h"
#include "uart.h"
#include "panic.h"
#include "rs485.h"
#include "msg.h"
#include "util.h"
#include "para.h"
#include "debug.h"
#define COMPID "rs485"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
osThreadId_t rs485TaskHandle;
osThreadId_t rs485PollTaskHandle;
osThreadId_t rs485RxPollTaskHandle;
osMessageQueueId_t rs485MsgHandle;
SysMsg_t rs485Msg;
osMutexId_t rs485MutexHandle;
osEventFlagsId_t rs485EventFlagsHandle;

static void Rs485UartMsgHandle(SysMsgUart_t* msgUart);
static void Rs485ThreadHandle(SysMsgThread_t* msgThread);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



uint8_t rs485TxBuffer[RS485_TX_BYTES] = {0};
uint16_t rs485TxHeader = 0;
uint16_t rs485TxTailer = 0;

uint8_t rs485RxBuffer[RS485_RX_BYTES] = {0};
uint16_t rs485RxHeader;
uint16_t rs485RxTailer;

static Rs485Task_t rs485;


static Rs485Comm_t txRs485;
static Rs485Comm_t rxRs485;


pLockMsg_t keyStatusArray;

uint16_t lock_status=0;     
uint16_t valve_status=0; 

osTimerId_t rs485TimerHandle;
//static void Rs485WorkDoRecFullPacket(void * argument);

Rs485Task_t *GetRs485(void)
{
    return &rs485;
}


void Rs485Task(void *argument);

static void MemAllocForKey(uint16_t nums)
{
	keyStatusArray = malloc(sizeof(LockMsg_t)*nums);
}

void RS485MgrInit(void)
{
  //rs485主任务
  const osThreadAttr_t rs485Task_attributes = {
    .name = NAME_TASK_RS485,
    .priority = (osPriority_t) PRIO_TASK_RS485,
    .stack_size = STACK_SIZE_TASK_RS485
  };
  rs485TaskHandle = osThreadNew(Rs485Task,NULL,&rs485Task_attributes);  	

	const osMessageQueueAttr_t rs485Msg_attributes = {
    .name = NAME_MSG_RS485
  };
  rs485MsgHandle = osMessageQueueNew(RS485_MSG_NUM,sizeof(SysMsg_t),&rs485Msg_attributes);  

  //rs485接受数据处理任务
  const osThreadAttr_t rs485RxPollTask_attributes = {
    .name = NAME_TASK_RS485_RX_POLL,
    .priority = (osPriority_t) PRIO_TASK_RS485_RX_POLL,
    .stack_size = STACK_SIZE_TASK_RS485_RX_POLL
  };
  rs485RxPollTaskHandle = osThreadNew(Rs485RxPollTask,NULL,&rs485RxPollTask_attributes); 

  //rs485发送数据处理任务
  const osThreadAttr_t rs485TxPollTask_attributes = {
    .name = NAME_TASK_RS485_TX_POLL,
    .priority = (osPriority_t) PRIO_TASK_RS485_TX_POLL,
    .stack_size = STACK_SIZE_TASK_RS485_TX_POLL
  };
  rs485PollTaskHandle = osThreadNew(Rs485TxPollTask,NULL,&rs485TxPollTask_attributes);  

  //rs485通道Mutex
  const osMutexAttr_t rs485Mutex_attributes = {
    .name = "rs485Mutex"
  };
  rs485MutexHandle = osMutexNew(&rs485Mutex_attributes);	  

  //rs485事件标志
  const osEventFlagsAttr_t rs485EventFlags_attributes = {
    .name = "rs485EventFlags"
  };
  rs485EventFlagsHandle = osEventFlagsNew(&rs485EventFlags_attributes);  	
}

bool Rs485SendWaitForFb(uint8_t cmd,uint8_t lock_id,uint32_t mask)
{
    uint32_t ulValue;
    bool result=TRUE;

		osMutexAcquire(rs485MutexHandle,RS485_DOWNLOAD_WAIT_TIMES);
		Rs485CreatePackage((uint8_t *)&txRs485,lock_id,cmd,0,0);

    RS485_DE_SEND();
    BoardRSSerialSendBuffer(RS485_UART_RS,(uint8_t *)&txRs485,sizeof(txRs485)); 
    RS485_DE_REC();
        
    ulValue=osEventFlagsWait(rs485EventFlagsHandle,mask,osFlagsWaitAll,200);     
		if((ulValue&osFlagsError)||((ulValue&mask)==0))
    {
     LOG("rs485 not received!!!!");
    result = FALSE;
    }

    osMutexRelease(rs485MutexHandle);    
    return result;  
}

static void Rs485RecvMsgInit(void)
{
  rs485RxHeader = 0;
  rs485RxTailer = 0;  
  memset(rs485RxBuffer,0,RS485_RX_BYTES);
  BoardRSSerialRecIT(RS485_UART_SERIAL);
}

static void Rs485RecvDataHandle(uint8_t recvDat)
{
  rs485RxBuffer[rs485RxTailer] = recvDat;
  rs485RxTailer = (rs485RxTailer + 1)%RS485_RX_BYTES;  
  BoardRSSerialRecIT(RS485_UART_RS);
}

static void Rs485ErrHandle(void)
{

}

static void RS485GPIOMspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_DeInit(GPIOC,RS485_DE_Pin);
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = RS485_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RS485_DE_GPIO_Port, &GPIO_InitStruct);	
}

void Rs485Task(void *argument)
{ 
  SysMsg_t msg;
  for(;;)
  {
    if(osMessageQueueGet(rs485MsgHandle,&msg,NULL,osWaitForever)==osOK)
    {     
      if (msg.src == SysMsgSrc_Uart)
      {
        Rs485UartMsgHandle(&msg.uart);		
      }
      else if(msg.src == SysMsgSrc_Thread)
      {
        Rs485ThreadHandle(&msg.thread);	    	
      }
    } 
  }
}

static USER_ERROR_CODE Rs485ThreadHandleIdle(SysMsgThread_t* msgThread);
static USER_ERROR_CODE Rs485ThreadHandleInit(SysMsgThread_t* msgThread);
static USER_ERROR_CODE Rs485ThreadHandleOpen(SysMsgThread_t* msgThread);
//定义函数数组,用于网口处理线程回调
static const TaskHandlePtrWithThread Rs485ThreadHandleArray[] = {
	Rs485ThreadHandleIdle,
	Rs485ThreadHandleInit,
	Rs485ThreadHandleOpen,
};
#define RS485_THREAD_HANDLE_COUNT_MAX      (sizeof(Rs485ThreadHandleArray)/sizeof(Rs485ThreadHandleArray[0]))

USER_ERROR_CODE Rs485ThreadHandleIdle(SysMsgThread_t* msgThread)
{
  USER_ERROR_CODE errorCode = USER_ERROR_NONE;
  return errorCode;
}

USER_ERROR_CODE Rs485ThreadHandleInit(SysMsgThread_t* msgThread)
{
  pAppTask_t app = GetApp();
  USER_ERROR_CODE errorCode = USER_ERROR_NONE;

  MemAllocForKey(app->Lock_Total_Nums);

  RS485GPIOMspInit();
  BoardRSSerialInit(RS485_UART_RS,Rs485RecvDataHandle,Rs485ErrHandle);
  Rs485RecvMsgInit();

  osThreadFlagsSet(appTaskHandle,APP_FLAG_MASK_RS485_INIT); 
  rs485.lockId = 1;//从1开始后轮询
  return errorCode;
}

USER_ERROR_CODE Rs485ThreadHandleOpen(SysMsgThread_t* msgThread)
{
	pAppTask_t app = GetApp();
	pGprsTask_t gprs = GetGprs();

  USER_ERROR_CODE errorCode = USER_ERROR_NONE;
  return errorCode;
}



void Rs485ThreadHandle(SysMsgThread_t* msgThread)
{
  uint8_t index;
  uint8_t msgId;
	USER_ERROR_CODE errorCode;

  msgId = msgThread->id;
  index = msgId - RS485_MSG_BASE;

	if(index > RS485_THREAD_HANDLE_COUNT_MAX)
	{
		errorCode = USER_ID_NO_SUPPORT;
       
    return;
	}
	errorCode = Rs485ThreadHandleArray[index](msgThread);	
	// LOGA("errorCode is %x",errorCode);
}

void Rs485UartMsgHandle(SysMsgUart_t* msgUart)
{

}

void Rs485TxPollTask(void *argument)
{ 
  pAppTask_t app = GetApp();
 
  for(;;)
  {
    osDelay(20);
    if(app->AppStatus < hardwareReady)//必须等到硬件准备好后发送才有意义
    continue;

    rs485.rs485CmdMask=RS485_READ_FLAG_MASK;
    // app->Lock_Total_Nums=16;
    app->Lock_Total_Nums=1;
    if(rs485.lockId>app->Lock_Total_Nums)
    rs485.lockId = 1;   
		// osMutexAcquire(gprsMutexHandle,GPRS_UPLOAD_WAIT_TIMES);    
   // rs485.lockId = 16;   
    if(Rs485SendWaitForFb(Rs485Cmd_Read,rs485.lockId,rs485.rs485CmdMask))//读数据正确
    {

    }   
    rs485.lockId++;	 
    // osMutexRelease(gprsMutexHandle);	
  }
}


void Rs485RxPollTask(void *argument)
{ 
  pAppTask_t app = GetApp();
	pGprsTask_t gprs = GetGprs();  

  uint16_t total_len;
  uint16_t len_mod,data_len;
  uint8_t min_bytes;  
  for(;;)
  {
    osDelay(20);

    HAL_GPIO_TogglePin(LED_B_GPIO_Port,LED_B_Pin); 

    if(BoardRSSerialGetRXNEFlag(RS485_UART_RS) == 0)
    {
      BoardRSSerialRecIT(RS485_UART_RS);
    }

    if(rs485RxTailer == rs485RxHeader)
    continue;

    min_bytes = sizeof(Rs485Comm_t);
    total_len = (rs485RxTailer >= rs485RxHeader) ?(rs485RxTailer - rs485RxHeader):(RS485_RX_BYTES-(rs485RxHeader - rs485RxTailer));
    
    if(total_len<min_bytes)
    continue;

    if(rs485RxBuffer[rs485RxHeader] != '#') 
    {
      rs485RxHeader = (rs485RxHeader + 1)%RS485_RX_BYTES;//release the data
      continue;
    }  

    data_len = sizeof(Rs485Comm_t);
    if((rs485RxHeader +  data_len) > RS485_RX_BYTES)
    {
        len_mod = (RS485_RX_BYTES - rs485RxHeader);
        memcpy((uint8_t *)&rxRs485,(rs485RxBuffer + rs485RxHeader),len_mod);
        memcpy(((uint8_t *)&rxRs485 + len_mod),rs485RxBuffer,(data_len - len_mod));
    }
    else
    {
        memcpy((uint8_t *)&rxRs485,(rs485RxBuffer + rs485RxHeader),data_len);
    }
    if(Rs485CheckHeadTailCS((uint8_t *)&rxRs485,data_len))
    {
      switch(rxRs485.cmd)
      {
        uint8_t old_lock_status;
        case Rs485Cmd_Read://数据在这里处理完成，以释放接受缓存
          //处理接受的数据
          if((rxRs485.lock_no <= app->Lock_Total_Nums)&&(rxRs485.lock_no >= 1))
          {
            pLockMsg_t keyStatusArray_temp= keyStatusArray;
            keyStatusArray_temp += (rxRs485.lock_no-1);
            old_lock_status =  keyStatusArray_temp->lock_status;

            memcpy(keyStatusArray_temp->key_id,rxRs485.key_id,5);
            keyStatusArray_temp->lock_status = rxRs485.lock_status;
            keyStatusArray_temp->err_code = rxRs485.err_code;
            if(rxRs485.lock_status == lock_status_havekey_locked)
            {
              lock_status |= (1<<(rxRs485.lock_no - 1));
            }
            else
            {
              lock_status &= ~(1<<(rxRs485.lock_no - 1));
            }

            if(((rxRs485.lock_status==lock_status_havekey_locked)&&(old_lock_status==lock_status_nokey_unlocked))
            ||((rxRs485.lock_status==lock_status_nokey_locked)&&(old_lock_status==lock_status_havekey_locked))
            ||((rxRs485.lock_status==lock_status_havekey_locked)&&(old_lock_status==lock_status_nokey_locked))        
            )
            
            
            
            //上传还钥匙记录
            {
              GprsRecordSend_t *GprsRecordSend;
              GprsRecordSend = malloc(sizeof(GprsRecordSend_t));
              //此处接受缓存释放，为下次做准备 
              GprsRecordSend->record_data.lock_no = rxRs485.lock_no;
              memcpy(GprsRecordSend->record_data.key_id,rxRs485.key_id,5);
              //时间信息
              HAL_RTC_GetTime(&hrtc,&rtcTime,RTC_FORMAT_BCD);
              HAL_RTC_GetDate(&hrtc,&rtcDate,RTC_FORMAT_BCD);            
              GprsRecordSend->record_data.year = rtcDate.Year;
              GprsRecordSend->record_data.month = rtcDate.Month;
              GprsRecordSend->record_data.day = rtcDate.Date;
              GprsRecordSend->record_data.hours = rtcTime.Hours;
              GprsRecordSend->record_data.minutes = rtcTime.Minutes;
              GprsRecordSend->record_data.seconds = rtcTime.Seconds;

              gprsMsg.src = SysMsgSrc_Thread;
              gprsMsg.thread.id = GPRS_MSG_Record; 					
              gprsMsg.thread.msg = GprsRecordSend; 					
              osMessageQueuePut(gprsMsgHandle,&gprsMsg,NULL,osWaitForever);//发送记录到后台
            }
          }
          osEventFlagsSet(rs485EventFlagsHandle,RS485_READ_FLAG_MASK);//读命令的回复		                					
        break;
        case Rs485Cmd_Open:
          //开门回复信息
          gprs->UserErrCode = (UserErrCode_t)rxRs485.err_code;
          memset((uint8_t *)&rxRs485,0,sizeof(rxRs485));
          osEventFlagsSet(rs485EventFlagsHandle,RS485_OPEN_LOCK_FLAG_MASK); 		        				
        break;	
        default :

        break;										
      }
      rs485RxHeader = (rs485RxHeader + data_len)%RS485_RX_BYTES;    
    }
    else
    {
      rs485RxHeader = (rs485RxHeader + 1)%RS485_RX_BYTES;
    }
  }
}
/* USER CODE END Application */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
