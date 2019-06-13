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
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "gprs.h"
#include "uart.h"
#include "panic.h"
#include "para.h"
#include "util.h"
#include "rs485.h"
#include "debug.h"
/* USER CODE END Includes */

#define COMPID "GPRS"

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
osThreadId_t gprsTaskHandle;
osMessageQueueId_t gprsMsgHandle;
SysMsg_t gprsMsg;



osTimerId_t gprsTimerHandle;

osThreadId_t heartTaskHandle;
osMessageQueueId_t heartMsgHandle;
osThreadId_t lockTaskHandle;
osMessageQueueId_t lockMsgHandle;
osThreadId_t returnTaskHandle;
osMessageQueueId_t returnMsgHandle;
osMutexId_t gprsMutexHandle;
osEventFlagsId_t gprsEventFlagsHandle;


static GprsTask_t gprs;

static void GprsUartMsgHandle(SysMsgUart_t* msgUart);
static void GprsThreadHandle(SysMsgThread_t* msgThread);
static void GprsWorkDoRecFullPacket(void * argument);


static void GprsRecvDataHandle(uint8_t recvDat);
static void GprsErrHandle(void);
static bool GprsSendStringWaitForFbUart(const char *string,uint32_t mask);
// static void LoadLenForSendMyNetWrite(uint16_t len);
// static bool GprsSendDataWaitForFb(uint8_t  *dat,uint16_t len,uint32_t mask,uint32_t delay);
static void GprsConfigData(void);
static void sim8xxGPIOMspInit(void);
static void GprsRecvMsgInit(void);



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */




uint8_t gprsTxBuffer[GPRS_TX_BYTES] = {0};


uint8_t gprsRxBuffer[GPRS_RX_BYTES] = {0};
uint16_t gprsRxHeader;
uint16_t gprsRxTailer;

static uint8_t bufferGprs[GPRS_RX_BYTES];

static uint16_t gprsCabinetSeq;


static  const char *Gprs_ECHO="ATE0\r\n";
static  const char *Gprs_CPIN="AT+CPIN?\r\n";
static  const char *Gprs_CSQ="AT+CSQ\r\n";
static  const char *Gprs_CREG="AT+CREG?\r\n";
static  const char *Gprs_CGATT="AT+CGATT?\r\n";
static  const char *Gprs_CSTT="AT+CSTT=\"CMNET\"\r\n";
static  const char *Gprs_CIICR="AT+CIICR\r\n";
static  const char *Gprs_CIFSR="AT+CIFSR\r\n";
static char Gprs_CIPSTART[]="AT+CIPSTART=\"TCP\",\"221.178.130.170\",\"20009\"\r\n";/*at+ipconfig=221.178.130.170:20009*/
// static char Gprs_CIPSTART[]="AT+CIPSTART=\"TCP\",\"117.080.255.121\",\"40089\"\r\n";/*at+ipconfig=117.80.255.121:40089*/
// static char Gprs_CIPSTART[]="AT+CIPSTART=\"TCP\",\"www.judoinfo.com.cn\",\"40089\"\r\n";
static char Gprs_CIPSEND[]= "AT+CIPSEND=0005\r\n";

char str_AT[] = "AT";
char str_OK[] = "OK";
char str_CONNETCT_OK[] = "CONNECT OK";
char str_SEND_OK[] = "SEND OK";
char str_enter[] = "\r\n";    
char str_large[] = ">";    
char str_point[] = ".";  

static GprsHeader_t GprsHeader;
static GprsUploadHeart_t GprsUploadHeart;



GprsTask_t *GetGprs(void)
{
    return &gprs;
}


void GprsMgrInit(void)
{
  const osThreadAttr_t gprsTask_attributes = {
    .name = NAME_TASK_APP,
    .priority = (osPriority_t) PRIO_TASK_GPRS,
    .stack_size = STACK_SIZE_TASK_GPRS
  };
  gprsTaskHandle = osThreadNew(GprsTask,NULL,&gprsTask_attributes);  	


	const osMessageQueueAttr_t gprsMsg_attributes = {
    .name = NAME_MSG_GPRS
  };
  gprsMsgHandle = osMessageQueueNew(GPRS_MSG_NUM,sizeof(SysMsg_t),&gprsMsg_attributes);  


	const osThreadAttr_t heartTask_attributes = {
    .name = NAME_TASK_HEART,
    .priority = (osPriority_t) PRIO_TASK_HEART,
    .stack_size = STACK_SIZE_TASK_HEART
  };
  heartTaskHandle = osThreadNew(HeartTask,NULL,&heartTask_attributes);  	


    const osTimerAttr_t gprsTimer_attributes = {
    .name = "gprsTimer"
  };
  gprsTimerHandle = osTimerNew(GprsWorkDoRecFullPacket, osTimerPeriodic, NULL, &gprsTimer_attributes);


  const osMutexAttr_t gprsMutex_attributes = {
    .name = "gprsMutex"
  };
  gprsMutexHandle = osMutexNew(&gprsMutex_attributes);		

  const osEventFlagsAttr_t gprsEventFlags_attributes = {
    .name = "gprsEventFlags"
  };
  gprsEventFlagsHandle = osEventFlagsNew(&gprsEventFlags_attributes);
}



static void GprsLoadLenForSendMyNetWrite(uint16_t len)
{
    uint8_t *s;
    s=(uint8_t *)strstr(Gprs_CIPSEND,"=")+1;
    *s++=len/1000%10+0x30;
    *s++=len/100%10+0x30;
    *s++=len/10%10+0x30;
    *s=len%10+0x30;
}

static bool GprsSendStringWaitForFbUart(const char *string,uint32_t mask)
{
    uint32_t ulValue;    
    
    BoardRSSerialSendString(GPRS_UART_RS,string); 
    // ulValue=osThreadFlagsWait(mask,osFlagsWaitAll,10000); 		
    ulValue=osEventFlagsWait(gprsEventFlagsHandle,mask,osFlagsWaitAll,10000); 
		if((ulValue&osFlagsError)||((ulValue&mask)==0))
		{
			return FALSE;
		}
    return TRUE;
}

static bool GprsSendDataWaitForFbUart(uint8_t  *dat,uint16_t len,uint32_t mask)
{
    uint32_t ulValue;    
    
    BoardRSSerialSendBuffer(GPRS_UART_RS,dat,len); 
    // ulValue=osThreadFlagsWait(mask,osFlagsWaitAll,10000); 
    ulValue=osEventFlagsWait(gprsEventFlagsHandle,mask,osFlagsWaitAll,10000); 		
		if((ulValue&osFlagsError)||((ulValue&mask)==0))
		{
			return FALSE;
		}
    return TRUE;
}

void GprsUploadDataWaitForFbUart(uint8_t  *dat,uint16_t len)
{ 
		uint8_t try_times; 

		osMutexAcquire(gprsMutexHandle,GPRS_UPLOAD_WAIT_TIMES);
		GprsLoadLenForSendMyNetWrite(len);
		for(try_times=0;try_times<3;try_times++)
		{
			gprs.GprsEventFlagMask = GPRS_FLAG_MASK_SEND_DAT_CMD;
			if(!GprsSendStringWaitForFbUart(Gprs_CIPSEND,gprs.GprsEventFlagMask))//send is ok
			continue;
			gprs.GprsEventFlagMask = GPRS_FLAG_MASK_SEND_DAT;
			if(!GprsSendDataWaitForFbUart(dat,len,gprs.GprsEventFlagMask))
			{
				continue;
			}
			else
			{
				LOG("dat is send ok");
				break;
			}
		}
		osMutexRelease(gprsMutexHandle);		
		if(try_times>=3)
		NVIC_SystemReset();//此处重新启动	
}

bool GprsCommWaitForFbUart(uint32_t mask)
{
    uint32_t ulValue;    
		ulValue=osEventFlagsWait(gprsEventFlagsHandle,mask,osFlagsWaitAll,10000); 		
		if((ulValue&osFlagsError)||((ulValue&mask)==0))
		{
			return FALSE;
		}
    return TRUE;
}


static void GprsConfigData(void)
{
    memcpy(&Gprs_CIPSTART[19],actionPara.IP0,15);
    memcpy(&Gprs_CIPSTART[37],actionPara.Port,5);
}

static void sim8xxGPIOMspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_DeInit(GPIOG,PWRKEY_CON_Pin|PWR_EN_Pin);
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PWRKEY_CON_GPIO_Port, PWRKEY_CON_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(PWR_EN_GPIO_Port, PWR_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PWRKEY_CON_Pin PWR_EN_Pin */
  GPIO_InitStruct.Pin = PWRKEY_CON_Pin|PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

void HeartTask(void *argument)
{
  pAppTask_t app = GetApp();

	uint8_t data[4];   
	uint16_t dat_len;
	uint8_t cmd;
  for(;;)
  {
    osDelay(5000);
    // osDelay(60000);
	#if 0
		if(app->AppStatus >= hardwareReady)
		{
			data[0] = (uint8_t )(lock_status<<8);
			data[1] = (uint8_t )lock_status;
			data[2] = (uint8_t )(valve_status<<8);
			data[3] = (uint8_t )valve_status;
			cmd = GprsCmd_Heart;
			dat_len = uartCreateRespDataPackage((uint8_t *)&GprsUploadHeart,gprs.upLoaderFrame,app->Cabinet_No,app->Lock_Total_Nums,cmd,data,sizeof(data));	
			GprsUploadDataWaitForFbUart((uint8_t *)&GprsUploadHeart,dat_len);
			gprs.upLoaderFrame++;		
	
		}
	#endif	
	}
}


static void GprsRecMsgParse(void)
{
	uint8_t cmd;
	uint16_t dat_len,head_tail_len;

	cmd = GprsHeader.cmd;
	dat_len = (GprsHeader.len[0]<<8) + GprsHeader.len[1];
	head_tail_len = sizeof(GprsHeader_t) + sizeof(GprsTailer_t);
	switch(cmd)
	{
		case GprsCmd_OpenLock:
			if(dat_len ==  (sizeof(GprsOpenLock_t) - head_tail_len))
			{
				GprsOpenLockRec_t *GprsOpenLockRec;
				GprsOpenLockRec = malloc(sizeof(GprsOpenLockRec_t));
				// memcpy(GprsOpenLockRec,bufferGprs,sizeof(GprsOpenLock_t));//此处bufferGprs 释放
				GprsOpenLockRec->GprsOpenLock.lock_no = 1;
				//通过gprs进行任务分发
				gprsMsg.src = SysMsgSrc_Thread;
				gprsMsg.thread.id = GPRS_MSG_OpenLock; 					
				gprsMsg.thread.msg = GprsOpenLockRec; 					
				osMessageQueuePut(gprsMsgHandle,&gprsMsg,NULL,osWaitForever);						
			}
		break;
		case GprsCmd_ReadLock:
			if(dat_len ==  (sizeof(GprsReadLock_t) - head_tail_len))
			{
				GprsReadLockRec_t *GprsReadLockRec;
				GprsReadLockRec = malloc(sizeof(GprsReadLockRec_t));
				// memcpy(GprsReadLockRec,bufferGprs,sizeof(GprsReadLock_t));//此处bufferGprs 释放
				// GprsReadLockRec->GprsReadLock.lock_no = 1;
				//通过gprs进行任务分发
				gprsMsg.src = SysMsgSrc_Thread;
				gprsMsg.thread.id = GPRS_MSG_ReadLock; 					
				gprsMsg.thread.msg = GprsReadLockRec; 					
				osMessageQueuePut(gprsMsgHandle,&gprsMsg,NULL,osWaitForever);					
			}
		break;		
		case GprsCmd_ReadValve:
			if(dat_len ==  (sizeof(GprsReadValve_t) - head_tail_len))
			{
				GprsReadValveRec_t *GprsReadValveRec;
				GprsReadValveRec = malloc(sizeof(GprsReadValveRec_t));
				// memcpy(GprsReadValveRec,bufferGprs,sizeof(GprsReadValve_t));//此处bufferGprs 释放
				//通过gprs进行任务分发
				gprsMsg.src = SysMsgSrc_Thread;
				gprsMsg.thread.id = GPRS_MSG_ReadValve; 					
				gprsMsg.thread.msg = GprsReadValveRec; 					
				osMessageQueuePut(gprsMsgHandle,&gprsMsg,NULL,osWaitForever);					
			}
		break;		
		case GprsCmd_ReadKeyId:
			if(dat_len ==  (sizeof(GprsReadKeyId_t) - head_tail_len))
			{
				GprsReadKeyIdRec_t *GprsReadKeyIdRec;
				GprsReadKeyIdRec = malloc(sizeof(GprsReadKeyIdRec_t));
				// memcpy(GprsReadKeyIdRec,bufferGprs,sizeof(GprsReadKeyId_t));//此处bufferGprs 释放
				//通过gprs进行任务分发
				gprsMsg.src = SysMsgSrc_Thread;
				gprsMsg.thread.id = GPRS_MSG_ReadKeyId; 					
				gprsMsg.thread.msg = GprsReadKeyIdRec; 					
				osMessageQueuePut(gprsMsgHandle,&gprsMsg,NULL,osWaitForever);						
			}
		break;		
		case GprsCmd_Heart:
			if(dat_len ==  (sizeof(GprsUploadHeartAck_t) - head_tail_len))
			{
      	osEventFlagsSet(gprsEventFlagsHandle,GPRS_FLAG_MASK_HEART_ACK); 						
			}
		break;			
		case GprsCmd_Record:
			if(dat_len ==  (sizeof(GprsUploadHeartAck_t) - head_tail_len))
			{
      	osEventFlagsSet(gprsEventFlagsHandle,GPRS_FLAG_MASK_RECORD_ACK); 						
			}
		break;			
	}
}


static void GprsWorkDoRecFullPacket(void * argument)
{
	pAppTask_t app;          
	app = GetApp();
  uint16_t total_len;
  uint16_t len_mod,data_len;
  uint8_t min_bytes;


	char *seek=NULL;
	char *result;	  

  if(BoardRSSerialGetRXNEFlag(GPRS_UART_RS) == 0)
  {
    BoardRSSerialRecIT(GPRS_UART_RS);
  }

  if(gprsRxTailer == gprsRxHeader)
  return;

  total_len = (gprsRxTailer >= gprsRxHeader) ?(gprsRxTailer - gprsRxHeader):(GPRS_RX_BYTES-(gprsRxHeader - gprsRxTailer));
  
  if(total_len<2)
  return;


	if((gprsRxBuffer[gprsRxHeader] == '#')&&(gprsRxBuffer[(gprsRxHeader + 1)%GPRS_RX_BYTES] == '#'))
	{	
  	min_bytes = sizeof(GprsHeader_t);
		if(total_len<min_bytes)
		return;
		data_len = gprsRxBuffer[(gprsRxHeader + min_bytes - 2)%GPRS_RX_BYTES]<<8 + gprsRxBuffer[(gprsRxHeader + min_bytes - 1)%GPRS_RX_BYTES];
		if(data_len>=GPRS_RX_BYTES)//invalid len
		{
    	gprsRxHeader = (gprsRxHeader + 1)%GPRS_RX_BYTES; 
			return;  		
		}
		data_len = sizeof(GprsHeader_t) + sizeof(GprsTailer_t) + data_len;//收到实际长度
		if(total_len<data_len)
		return;

		//复制数据到缓存
		if((gprsRxHeader +  data_len) > GPRS_RX_BYTES)
		{
				len_mod = (GPRS_RX_BYTES - gprsRxHeader);
				memcpy(bufferGprs,(gprsRxBuffer + gprsRxHeader),len_mod);
				memcpy((bufferGprs + len_mod),gprsRxBuffer,(data_len - len_mod));
		}
		else
		{
				memcpy(bufferGprs,(gprsRxBuffer + gprsRxHeader),data_len);
		}

		if(GprsCheckHeadTailCRC16(bufferGprs,data_len))
		{
			memcpy((uint8_t *)&GprsHeader,bufferGprs,sizeof(GprsHeader));
			gprsCabinetSeq = GprsHeader.cabinet_seq[0]<<8 + GprsHeader.cabinet_seq[1];
			//判断接受数据的数量
			if((data_len == (sizeof(GprsHeader_t) + sizeof(GprsTailer_t) + (GprsHeader.len[0]<<8 + GprsHeader.len[1])))
					&& (GprsHeader.Lock_Total_Nums==app->Lock_Total_Nums)
					&& (gprsCabinetSeq==app->Cabinet_No))
			GprsRecMsgParse();

			// uint8_t index = gprsRxBuffer[4];
			// memcpy(GprsMsgChannelArray[index].rec,gprsRxBuffer,total_len);
			// GprsMsgChannelArray[index].recHandle();
			//数据处理完毕，
			gprsRxHeader = (gprsRxHeader + data_len)%gprsRxHeader;
		}
		else
		{
			//继续解析下一个数据
    	gprsRxHeader = (gprsRxHeader + 1)%GPRS_RX_BYTES;  				
		}
		memset(bufferGprs,0,GPRS_RX_BYTES);
	}	
	else if((gprsRxBuffer[gprsRxHeader] == '\r')&&(gprsRxBuffer[(gprsRxHeader + 1)%GPRS_RX_BYTES] == '\n'))//AT cmd
	{
		//等待命令数据接受完成
		osDelay(300);
  	data_len = (gprsRxTailer >= gprsRxHeader) ?(gprsRxTailer - gprsRxHeader):(GPRS_RX_BYTES-(gprsRxHeader - gprsRxTailer));		
		//此处可能接受多条信息
		if(data_len>GPRS_RX_BYTES)//数据发生错误，丢弃多出的数据
		data_len = GPRS_RX_BYTES;

		//复制数据到缓存
		if((gprsRxHeader +  data_len) > GPRS_RX_BYTES)
		{
				len_mod = (GPRS_RX_BYTES - gprsRxHeader);
				memcpy(bufferGprs,(gprsRxBuffer + gprsRxHeader),len_mod);
				memcpy((bufferGprs + len_mod),gprsRxBuffer,(data_len - len_mod));
		}
		else
		{
				memcpy(bufferGprs,(gprsRxBuffer + gprsRxHeader),data_len);
		}
		
		//判断数据是否正确,找第一个回车
		result = strstr((char *)bufferGprs,"\r\n");
		if(result==NULL)
		{
    	gprsRxHeader = (gprsRxHeader + 1)%GPRS_RX_BYTES; 
			memset(bufferGprs,0,GPRS_RX_BYTES);
			return;	
		}
		if(gprs.GprsEventFlagMask!=GPRS_FLAG_MASK_SEND_DAT_CMD)
		{
			result = strstr((char *)bufferGprs+2,"\r\n");	//找第二个回车
			// result = strtok(NULL,"\r\n");
			if(result==NULL)
			{
				gprsRxHeader = (gprsRxHeader + 1)%GPRS_RX_BYTES; 
				memset(bufferGprs,0,GPRS_RX_BYTES);			
				return;	
			}
		}
		//数据接受正确		
		switch(gprs.GprsEventFlagMask)
		{
			case GPRS_FLAG_MASK_NORMAL:
				seek=str_OK;
			break;
			case GPRS_FLAG_MASK_CONNECT:
				seek=str_CONNETCT_OK;
			break;
			case GPRS_FLAG_MASK_SEND_DAT_CMD:
				seek=str_large;
			break;
			case GPRS_FLAG_MASK_SEND_DAT:
				seek=str_SEND_OK;
			break;
			case GPRS_FLAG_MASK_CIFSR:
				seek=str_point;
			break;
			default:
			break;	
		}
		if(seek != NULL)
		{
			result = strstr((char *)bufferGprs,seek);
			if(result != NULL) 
			// osThreadFlagsSet(gprsTaskHandle,gprs.GprsEventFlagMask);
      osEventFlagsSet(gprsEventFlagsHandle,gprs.GprsEventFlagMask); 			
		}
    gprsRxHeader = (gprsRxHeader + data_len)%GPRS_RX_BYTES;//数据处理完毕，更新header
		memset(bufferGprs,0,GPRS_RX_BYTES);		
	}
	else
	{
    	gprsRxHeader = (gprsRxHeader + 1)%GPRS_RX_BYTES; 
	}
}


static void GprsRecvMsgInit(void)
{
  gprsRxHeader = 0;
  gprsRxTailer = 0;  
  memset(gprsRxBuffer,0,GPRS_RX_BYTES);
  BoardRSSerialRecIT(GPRS_UART_SERIAL);
}


static void GprsRecvDataHandle(uint8_t recvDat)
{
  gprsRxBuffer[gprsRxTailer] = recvDat;
  gprsRxTailer = (gprsRxTailer + 1)%GPRS_RX_BYTES;  
  BoardRSSerialRecIT(GPRS_UART_RS);
}

static void GprsErrHandle(void)
{

}


void GprsTask(void *argument)
{ 
  SysMsg_t msg;
  for(;;)
  {
    if(osMessageQueueGet(gprsMsgHandle,&msg,NULL,osWaitForever)==osOK)
    {     
      if (msg.src == SysMsgSrc_Uart)
      {
        GprsUartMsgHandle(&msg.uart);		
      }
      else if(msg.src == SysMsgSrc_Thread)
      {
        GprsThreadHandle(&msg.thread);	    	
      }
    } 
  }
}

static USER_ERROR_CODE GprsThreadHandleIdle(SysMsgThread_t* msgThread);
static USER_ERROR_CODE GprsThreadHandleInit(SysMsgThread_t* msgThread);
static USER_ERROR_CODE GprsThreadHandleOpenLock(SysMsgThread_t* msgThread);
static USER_ERROR_CODE GprsThreadHandleReadLock(SysMsgThread_t* msgThread);
static USER_ERROR_CODE GprsThreadHandleReadValve(SysMsgThread_t* msgThread);
static USER_ERROR_CODE GprsThreadHandleReadKeyId(SysMsgThread_t* msgThread);
static USER_ERROR_CODE GprsThreadHandleRecord(SysMsgThread_t* msgThread);

//定义函数数组,用于网口处理线程回调
static const TaskHandlePtrWithThread GprsThreadHandleArray[] = {
	GprsThreadHandleIdle,
	GprsThreadHandleInit,
	GprsThreadHandleOpenLock,
	GprsThreadHandleReadLock,
	GprsThreadHandleReadValve,
	GprsThreadHandleReadKeyId,
	GprsThreadHandleRecord,
};
#define GPRS_THREAD_HANDLE_COUNT_MAX      (sizeof(GprsThreadHandleArray)/sizeof(GprsThreadHandleArray[0]))

USER_ERROR_CODE GprsThreadHandleIdle(SysMsgThread_t* msgThread)
{
  USER_ERROR_CODE errorCode = USER_ERROR_NONE;
  return errorCode;
}

USER_ERROR_CODE GprsThreadHandleInit(SysMsgThread_t* msgThread)
{
  USER_ERROR_CODE errorCode = USER_ERROR_NONE;
	pAppTask_t app = GetApp();

	sim8xxGPIOMspInit();
	GPRS_PWR_OFF();			
	osDelay(2000); 
	GPRS_PWR_ON();
	osDelay(3000); 
	BoardRSSerialInit(GPRS_UART_RS,GprsRecvDataHandle,GprsErrHandle);
	GprsRecvMsgInit();		
	osTimerStart(gprsTimerHandle,1);	
	osDelay(6000); 
	GprsConfigData();	

	gprs.GprsEventFlagMask = GPRS_FLAG_MASK_NORMAL;
	GprsSendStringWaitForFbUart(Gprs_ECHO,gprs.GprsEventFlagMask); 
	LOG("Gprs_ECHO is ok");	
	osDelay(100); 	
	GprsSendStringWaitForFbUart(Gprs_CPIN,gprs.GprsEventFlagMask);
	LOG("Gprs_CPINis ok");	 
	osDelay(100); 
	GprsSendStringWaitForFbUart(Gprs_CSQ,gprs.GprsEventFlagMask); 
	LOG("Gprs_CSQ is ok");	
	osDelay(100); 	
	GprsSendStringWaitForFbUart(Gprs_CREG,gprs.GprsEventFlagMask); 
	LOG("Gprs_CREG is ok");	
	osDelay(100); 	
	GprsSendStringWaitForFbUart(Gprs_CGATT,gprs.GprsEventFlagMask); 
	LOG("Gprs_CGATT is ok");	
	osDelay(100); 	
	GprsSendStringWaitForFbUart(Gprs_CSTT,gprs.GprsEventFlagMask);
	LOG("Gprs_CSTT is ok");	 
	osDelay(100); 		
	GprsSendStringWaitForFbUart(Gprs_CIICR,gprs.GprsEventFlagMask); 
	LOG("Gprs_CIICR is ok");
	osDelay(100); 	
	gprs.GprsEventFlagMask = GPRS_FLAG_MASK_CIFSR;       
	GprsSendStringWaitForFbUart(Gprs_CIFSR,gprs.GprsEventFlagMask); 
	LOG("Gprs_CIFSR is ok");
	osDelay(500); 
	gprs.GprsEventFlagMask = GPRS_FLAG_MASK_CONNECT;       
	if(GprsSendStringWaitForFbUart(Gprs_CIPSTART,gprs.GprsEventFlagMask))
	{
		app->gprsConnected=TRUE;
    LOG("app->gprsConnected is ok");	
		// GprsHeader.cmd = GprsCmd_ReadKeyId;	
		// GprsHeader.len[0] = 0;	
		// GprsHeader.len[1] = 1;	
		// GprsRecMsgParse();		
	}
	else
	{
		app->gprsConnected=FALSE;
	}
	osThreadFlagsSet(appTaskHandle,APP_FLAG_MASK_GPRS_INIT);   
  return errorCode;
}

USER_ERROR_CODE GprsThreadHandleOpenLock(SysMsgThread_t* msgThread)
{
	pAppTask_t app = GetApp();
	pRs485Task_t rs485 = GetRs485();
  USER_ERROR_CODE errorCode = USER_ERROR_NONE;

	GprsOpenLockRec_t *GprsOpenLockRec;
	uint16_t dat_len,frame;
  uint8_t lock_no,cmd;	
//解析消息数据
  GprsOpenLockRec = (GprsOpenLockRec_t *)msgThread->msg; 
  lock_no = GprsOpenLockRec->GprsOpenLock.lock_no;
  cmd = GprsOpenLockRec->GprsOpenLock.header.cmd;
	frame = (GprsOpenLockRec->GprsOpenLock.header.frame[0]<<8) + GprsOpenLockRec->GprsOpenLock.header.frame[1]; 	

	if((lock_no>=1)&&(lock_no<=actionPara.Lock_Total_Nums))
	{
	//执行开锁操作
		rs485->rs485CmdMask = RS485_OPEN_LOCK_FLAG_MASK;
		if(!Rs485SendWaitForFb(Rs485Cmd_Open,lock_no,rs485->rs485CmdMask))//下位机无回复
		gprs.UserErrCode = UserErrCode_No_Ack;
	}
	else
	{
		gprs.UserErrCode = UserErrCode_KeySeq;
	}
	//上传结果给后台	
	dat_len = uartCreateRespDataPackage(GprsOpenLockRec->bufferTx,frame,app->Cabinet_No,app->Lock_Total_Nums,cmd,(uint8_t *)&gprs.UserErrCode,1);
	GprsUploadDataWaitForFbUart(GprsOpenLockRec->bufferTx,dat_len);	
	free(GprsOpenLockRec);
  return errorCode;
}



USER_ERROR_CODE GprsThreadHandleReadLock(SysMsgThread_t* msgThread)
{
	pAppTask_t app = GetApp();
	pRs485Task_t rs485 = GetRs485();
  USER_ERROR_CODE errorCode = USER_ERROR_NONE;

	uint8_t lock[2];

	GprsReadLockRec_t *GprsReadLockRec;
	uint16_t dat_len,frame;
  uint8_t cmd;	
//解析消息数据
  GprsReadLockRec = (GprsReadLockRec_t *)msgThread->msg; 
  cmd = GprsReadLockRec->GprsReadLock.header.cmd;
	frame = (GprsReadLockRec->GprsReadLock.header.frame[0]<<8) + GprsReadLockRec->GprsReadLock.header.frame[1]; 	

	//上传结果给后台
	lock[0] = (uint8_t)(lock_status<<8);
	lock[1] = (uint8_t)lock_status;


	dat_len = uartCreateRespDataPackage(GprsReadLockRec->bufferTx,frame,app->Cabinet_No,app->Lock_Total_Nums,cmd,lock,2);
	GprsUploadDataWaitForFbUart(GprsReadLockRec->bufferTx,dat_len);
	
	free(GprsReadLockRec);
  return errorCode;
}


USER_ERROR_CODE GprsThreadHandleReadValve(SysMsgThread_t* msgThread)
{
	pAppTask_t app = GetApp();
	pRs485Task_t rs485 = GetRs485();
  USER_ERROR_CODE errorCode = USER_ERROR_NONE;

	uint8_t valve[2];

	GprsReadValveRec_t *GprsReadValveRec;
	uint16_t dat_len,frame;
  uint8_t cmd;	
//解析消息数据
  GprsReadValveRec = (GprsReadValveRec_t *)msgThread->msg; 
  cmd = GprsReadValveRec->GprsReadValve.header.cmd;
	frame = (GprsReadValveRec->GprsReadValve.header.frame[0]<<8) + GprsReadValveRec->GprsReadValve.header.frame[1]; 	

	//上传结果给后台
	valve[0] = (uint8_t)(valve_status<<8);
	valve[1] = (uint8_t)valve_status;


	dat_len = uartCreateRespDataPackage(GprsReadValveRec->bufferTx,frame,app->Cabinet_No,app->Lock_Total_Nums,cmd,valve,2);
	GprsUploadDataWaitForFbUart(GprsReadValveRec->bufferTx,dat_len);

	
	free(GprsReadValveRec);
  return errorCode;
}


USER_ERROR_CODE GprsThreadHandleReadKeyId(SysMsgThread_t* msgThread)
{
	pAppTask_t app = GetApp();
	pRs485Task_t rs485 = GetRs485();
  USER_ERROR_CODE errorCode = USER_ERROR_NONE;

	pLockMsg_t keyStatusArray_temp= keyStatusArray;	

	uint8_t dat[6];

	GprsReadKeyIdRec_t *GprsReadKeyIdRec;
	uint16_t dat_len,frame;
  uint8_t lock_no;	
//解析消息数据
  GprsReadKeyIdRec = (GprsReadKeyIdRec_t *)msgThread->msg; 
	lock_no = GprsReadKeyIdRec->GprsReadKeyId.lock_no;
//  cmd = GprsReadKeyIdRec->GprsReadKeyId.header.cmd;
	// if(cmd != GprsCmd_ReadKeyId)
	// return errorCode;
	
	frame = (GprsReadKeyIdRec->GprsReadKeyId.header.frame[0]<<8) + GprsReadKeyIdRec->GprsReadKeyId.header.frame[1]; 

	dat[0]=lock_no;

	keyStatusArray_temp += (lock_no-1);
	memcpy(&dat[1],keyStatusArray_temp->key_id,5); 	

	dat_len = uartCreateRespDataPackage(GprsReadKeyIdRec->bufferTx,frame,app->Cabinet_No,app->Lock_Total_Nums,GprsCmd_ReadKeyId,dat,sizeof(dat));
	GprsUploadDataWaitForFbUart(GprsReadKeyIdRec->bufferTx,dat_len);

	
	free(GprsReadKeyIdRec);
  return errorCode;
}


USER_ERROR_CODE GprsThreadHandleRecord(SysMsgThread_t* msgThread)
{
	pAppTask_t app = GetApp();


  USER_ERROR_CODE errorCode = USER_ERROR_NONE;
  GprsRecordSend_t *GprsRecordSend;
	uint16_t dat_len;

  LOG("GprsThreadHandleRecord()");		
//解析消息数据
  GprsRecordSend = (GprsRecordSend_t *)msgThread->msg; 
//上传结果给后台	
	dat_len = uartCreateRespDataPackage(GprsRecordSend->bufferTx,gprs.upLoaderFrame,app->Cabinet_No,app->Lock_Total_Nums,GprsCmd_Record,
                 (uint8_t *)&GprsRecordSend->record_data,sizeof(GprsUploadRecordDat_t));
	gprs.upLoaderFrame++;
	GprsUploadDataWaitForFbUart(GprsRecordSend->bufferTx,dat_len);
  free(GprsRecordSend);
  gprs.GprsEventFlagMask = GPRS_FLAG_MASK_RECORD_ACK;	
  if(!GprsCommWaitForFbUart(gprs.GprsEventFlagMask))
  {
    LOG("GprsThreadHandleRecord no ACK");	
  }    
  return errorCode;
}

void GprsThreadHandle(SysMsgThread_t* msgThread)
{
  uint8_t index;
  uint8_t msgId;
	USER_ERROR_CODE errorCode;


  msgId = msgThread->id;
  index = msgId - GPRS_MSG_BASE;

	if(index > GPRS_THREAD_HANDLE_COUNT_MAX)
	{
		errorCode = USER_ID_NO_SUPPORT;      
    return;
	}
	errorCode = GprsThreadHandleArray[index](msgThread);   
	// LOGA("errorCode is %x",errorCode);
}
void GprsUartMsgHandle(SysMsgUart_t* msgUart)
{

}





