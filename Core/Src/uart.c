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
#include "uart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
MCU_UartRecvIntProcFunc uart1RecvFuncPtr = NULL;
uint8_t uart1SingleData;
MCU_UartErrProcFunc uart1ErrFuncPtr = NULL;

MCU_UartRecvIntProcFunc uart3RecvFuncPtr = NULL;
uint8_t uart3SingleData;
MCU_UartErrProcFunc uart3ErrFuncPtr = NULL;

MCU_UartRecvIntProcFunc uart4RecvFuncPtr = NULL;
uint8_t uart4SingleData;
MCU_UartErrProcFunc uart4ErrFuncPtr = NULL;

MCU_UartRecvIntProcFunc uart5RecvFuncPtr = NULL;
uint8_t uart5SingleData;
MCU_UartErrProcFunc uart5ErrFuncPtr = NULL;

MCU_UartRecvIntProcFunc uart6RecvFuncPtr = NULL;
uint8_t uart6SingleData;
MCU_UartErrProcFunc uart6ErrFuncPtr = NULL;


osMutexId_t uart1MutexHandle;
osMutexId_t uart3MutexHandle;
osMutexId_t uart4MutexHandle;
osMutexId_t uart5MutexHandle;
osMutexId_t uart6MutexHandle;



void BoardRSSerialInit(Board_RS_Serial_t serialNo,MCU_UartRecvIntProcFunc rxCallBack,MCU_UartErrProcFunc errCallBack)
{
  switch(serialNo)
  {
    case BOARD_UART1:
        {
          HAL_UART_DeInit(&huart1);
          MXUsart1UartInit();
          uart1RecvFuncPtr = rxCallBack;
          uart1ErrFuncPtr = errCallBack;

          const osMutexAttr_t uart1Mutex_attributes = {
            .name = "uart1Mutex"
          };
          uart1MutexHandle = osMutexNew(&uart1Mutex_attributes);        
        }
        break;
    case BOARD_UART3:
        {
          HAL_UART_DeInit(&huart3);    
          MXUsart3UartInit();
          uart3RecvFuncPtr = rxCallBack;    
          uart3ErrFuncPtr = errCallBack; 

          const osMutexAttr_t uart3Mutex_attributes = {
            .name = "uart3Mutex"
          };
          uart3MutexHandle = osMutexNew(&uart3Mutex_attributes);                    
        }
        break;
    case BOARD_UART4:
        {
          HAL_UART_DeInit(&huart4);    
          MXUsart4UartInit();
          uart4RecvFuncPtr = rxCallBack;    
          uart4ErrFuncPtr = errCallBack; 

          const osMutexAttr_t uart4Mutex_attributes = {
            .name = "uart4Mutex"
          };
          uart4MutexHandle = osMutexNew(&uart4Mutex_attributes);                    
        }
        break;
    case BOARD_UART5:
        {
          HAL_UART_DeInit(&huart5);    
          MXUsart5UartInit();
          uart5RecvFuncPtr = rxCallBack;    
          uart5ErrFuncPtr = errCallBack;   

          const osMutexAttr_t uart5Mutex_attributes = {
            .name = "uart5Mutex"
          };
          uart5MutexHandle = osMutexNew(&uart5Mutex_attributes);                  
        }
        break;                
    case BOARD_UART6:
        {
          HAL_UART_DeInit(&huart6);    
          MXUsart6UartInit();
          uart6RecvFuncPtr = rxCallBack;    
          uart6ErrFuncPtr = errCallBack;  

          const osMutexAttr_t uart6Mutex_attributes = {
            .name = "uart6Mutex"
          };
          uart6MutexHandle = osMutexNew(&uart6Mutex_attributes);                   
        }
        break;
  }  
}

void BoardRSSerialDeInit(Board_RS_Serial_t serialNo)
{
  switch(serialNo)
  {
    case BOARD_UART1:
        HAL_UART_DeInit(&huart1);
        break;
    case BOARD_UART3:
        HAL_UART_DeInit(&huart3);           
        break;
    case BOARD_UART4:
        HAL_UART_DeInit(&huart4);
        break;
    case BOARD_UART5:
        HAL_UART_DeInit(&huart5);           
        break;
    case BOARD_UART6:
        HAL_UART_DeInit(&huart6);
        break;				
  }
}


void BoardRSSerialRecIT(Board_RS_Serial_t serialNo)
{
  switch(serialNo)
  {
    case BOARD_UART1:
		      osMutexAcquire(uart1MutexHandle,UART_SEND_WAIT_TIMES);              
          HAL_UART_Receive_IT(&huart1,&uart1SingleData,1); 
 		      osMutexRelease(uart1MutexHandle);		               
        break;
    case BOARD_UART3:
		      osMutexAcquire(uart3MutexHandle,UART_SEND_WAIT_TIMES);       
          HAL_UART_Receive_IT(&huart3,&uart3SingleData,1);
 		      osMutexRelease(uart3MutexHandle);		           
        break;
    case BOARD_UART4:
		      osMutexAcquire(uart4MutexHandle,UART_SEND_WAIT_TIMES);       
          HAL_UART_Receive_IT(&huart4,&uart4SingleData,1);    
 		      osMutexRelease(uart4MutexHandle);		             
        break;
    case BOARD_UART5:
		      osMutexAcquire(uart5MutexHandle,UART_SEND_WAIT_TIMES);       
          HAL_UART_Receive_IT(&huart5,&uart5SingleData,1);
 		      osMutexRelease(uart5MutexHandle);		           
        break;
    case BOARD_UART6:
		      osMutexAcquire(uart6MutexHandle,UART_SEND_WAIT_TIMES);       
          HAL_UART_Receive_IT(&huart6,&uart6SingleData,1);  
 		      osMutexRelease(uart6MutexHandle);		               
        break;             
  }

}

uint8_t  BoardRSSerialGetRXNEFlag(Board_RS_Serial_t serialNo)
{
  uint8_t flag;
  switch(serialNo)
  {
    case BOARD_UART1:
          flag = __HAL_UART_GET_IT_SOURCE(&huart1,UART_IT_RXNE);
        break;
    case BOARD_UART3:
          flag = __HAL_UART_GET_IT_SOURCE(&huart3,UART_IT_RXNE);
        break;
    case BOARD_UART4:
          flag = __HAL_UART_GET_IT_SOURCE(&huart4,UART_IT_RXNE);
        break;
    case BOARD_UART5:
          flag = __HAL_UART_GET_IT_SOURCE(&huart5,UART_IT_RXNE);
        break;
    case BOARD_UART6:
          flag = __HAL_UART_GET_IT_SOURCE(&huart6,UART_IT_RXNE);
        break;
           
  }
  return flag;
}

void BoardRSSerialSendBuffer(Board_RS_Serial_t serialNo, uint8_t* bufferPtr,uint16_t bufferLength)
{
  switch(serialNo)
  {
    case BOARD_UART1:
		      osMutexAcquire(uart1MutexHandle,UART_SEND_WAIT_TIMES);    
          HAL_UART_Transmit(&huart1,bufferPtr,bufferLength,100);
 		      osMutexRelease(uart1MutexHandle);		         
        break;
    case BOARD_UART3:
		      osMutexAcquire(uart3MutexHandle,UART_SEND_WAIT_TIMES);        
          HAL_UART_Transmit(&huart3,bufferPtr,bufferLength,100);
 		      osMutexRelease(uart3MutexHandle);		            
        break;
    case BOARD_UART4:
		      osMutexAcquire(uart4MutexHandle,UART_SEND_WAIT_TIMES);        
          HAL_UART_Transmit(&huart4,bufferPtr,bufferLength,100);
 		      osMutexRelease(uart4MutexHandle);		            
        break;
    case BOARD_UART5:
		      osMutexAcquire(uart5MutexHandle,UART_SEND_WAIT_TIMES);        
          HAL_UART_Transmit(&huart5,bufferPtr,bufferLength,100);
 		      osMutexRelease(uart5MutexHandle);		            
        break;
    case BOARD_UART6:
		      osMutexAcquire(uart6MutexHandle,UART_SEND_WAIT_TIMES);        
          HAL_UART_Transmit(&huart6,bufferPtr,bufferLength,100);
 		      osMutexRelease(uart6MutexHandle);		            
        break;
  }
}

void BoardRSSerialSendString(Board_RS_Serial_t serialNo,const char *string)
{
  switch(serialNo)
  {
      case BOARD_UART1:
		      osMutexAcquire(uart1MutexHandle,UART_SEND_WAIT_TIMES);          
          HAL_UART_Transmit(&huart1,(uint8_t *)string,strlen((char const *)string),100);
 		      osMutexRelease(uart1MutexHandle);		            
          break;
      case BOARD_UART3:
		      osMutexAcquire(uart3MutexHandle,UART_SEND_WAIT_TIMES);          
          HAL_UART_Transmit(&huart3,(uint8_t *)string,strlen((char const *)string),100);
 		      osMutexRelease(uart3MutexHandle);		            
          break;
      case BOARD_UART4:
		      osMutexAcquire(uart4MutexHandle,UART_SEND_WAIT_TIMES);          
          HAL_UART_Transmit(&huart4,(uint8_t *)string,strlen((char const *)string),100);
 		      osMutexRelease(uart4MutexHandle);		            
          break;
      case BOARD_UART5:
		      osMutexAcquire(uart5MutexHandle,UART_SEND_WAIT_TIMES);          
          HAL_UART_Transmit(&huart5,(uint8_t *)string,strlen((char const *)string),100);
 		      osMutexRelease(uart5MutexHandle);		            
          break;
      case BOARD_UART6:
		      osMutexAcquire(uart6MutexHandle,UART_SEND_WAIT_TIMES);          
          HAL_UART_Transmit(&huart6,(uint8_t *)string,strlen((char const *)string),100);
 		      osMutexRelease(uart6MutexHandle);		            
          break;
  }
}






/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
