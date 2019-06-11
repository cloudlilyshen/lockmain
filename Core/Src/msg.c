/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                          Altera Virtual DEV-5XS1 Cortex-A9 MPCore DEVELOPMENT KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : NB                  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "includes.h"
#include "main.h"
#include "msg.h"
#include "panic.h"

void MsgSend(osMessageQId task, pMsgId_t msg)
{
    // const TickType_t xMaxWaitTime=pdMS_TO_TICKS(300);
    if(osMessagePut(task,(uint32_t ) msg,300) != osOK) 
    {
         _Panic(QUEUE_SEND_PANIC);
    }
}

