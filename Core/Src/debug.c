/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "main.h"
#include "util.h"
#include "debug.h"
#include "para.h"
#include "uart.h"

#define COMPID "DEBUG"

static uint8_t debugRxBuffer[DEBUG_RX_BYTES];
//static uint16_t debugRxHeader;
static uint16_t debugRxTailer;


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	while(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_TXE) == RESET){}    
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
	huart4.Instance->DR = (uint8_t) ch;

  /* Loop until the end of transmission */
	while(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_TC) == RESET){}

  return ch;
}



static void IPAddressCopy(uint8_t* src,uint8_t* dst,uint8_t len)
{
    switch(len)
    {
        case 1:
            *dst++='0';
            *dst++='0';
            *dst=*src;
        break;
        case 2:
            *dst++='0';
            *dst++=src[0];
            *dst=src[1];
        break;    
        case 3:
            *dst++=src[0];
            *dst++=src[1];
            *dst=src[2];
        break;    
}
}

static void parse_cmd(uint8_t dat)
{
    char *result = NULL;
 

    if (debugRxTailer > DEBUG_RX_BYTES)
    debugRxTailer = 0;

    if (dat == '\r') 
    {
        printf("UART RX: %s",debugRxBuffer);
        debugRxTailer = 0;
        uint8_t *temp_char = debugRxBuffer;

		if(strstr((char *)temp_char,"at+ip") != NULL)
        {
			printf("\r\nat+ip\r\n");

            result = strtok((char *)temp_char,"=");
            if(result==NULL)
            goto error_handle;
						
						
            result = strtok(NULL,".");
            if(result==NULL||strlen(result)==0||strlen(result)>3)
            goto error_handle;
            IPAddressCopy((uint8_t *)result,actionPara.IP0,strlen(result));


            result = strtok(NULL,".");
            if(result==NULL||strlen(result)==0||strlen(result)>3)
            goto error_handle;
            IPAddressCopy((uint8_t *)result,actionPara.IP1,strlen(result));


            result = strtok(NULL,".");
            if(result==NULL||strlen(result)==0||strlen(result)>3)
            goto error_handle;
            IPAddressCopy((uint8_t *)result,actionPara.IP2,strlen(result));



            result = strtok(NULL,":");
            if(result==NULL||strlen(result)==0||strlen(result)>3)
            goto error_handle;
            IPAddressCopy((uint8_t *)result,actionPara.IP3,strlen(result));

            result = strtok(NULL,"\r");
            if(result==NULL||strlen(result)!=sizeof(actionPara.Port))
            goto error_handle;

            memcpy(actionPara.Port,result,sizeof(actionPara.Port));

	        actionPara.chk=get_chk_bcc((uint8_t*)&actionPara,SYSTEM_ACTION_PARA_SIZE-1);
            goto save_handle;
        }
		else if(strstr((char *)temp_char,"at+uid") != NULL)
	 	{
			printf("\r\nat+uid\r\n");
            result = strtok((char *)temp_char,"=");
            if(result==NULL)
            goto error_handle;			
            result = strtok(NULL,"\r");
            if(result==NULL||strlen(result)==0||strlen(result)>10)
            goto error_handle;
            // uint32_t uid=String2Uint(result,strlen(result));
//            actionPara.Cabinet_No[3]=uid&0xff;	
//            actionPara.Cabinet_No[2]=uid>>8&0xff;	
//            actionPara.Cabinet_No[1]=uid>>16&0xff;	
//            actionPara.Cabinet_No[0]=uid>>24&0xff;	
            goto save_handle;
	 	}        
		else if(strstr((char *)temp_char,"at+rst") != NULL)
	 	{
			printf("\r\nat+rst\r\n");
    		NVIC_SystemReset();
	 	}
		 else if(strstr((char *)temp_char,"at+default") != NULL)
        {
            printf("\r\nat+default\r\n");
            SystemLoadDefaultParamOnce();
            goto save_handle;
        }
		else if(strstr((char *)temp_char,"at+read") != NULL)
	 	{
			printf("\r\nat+read\r\n");

			uint32_t uid=TwoBytes2uint16BigEndian(actionPara.Cabinet_No);
			printf("Cabinet_No is %d\r\n",uid);
	 	}

    }
    return;    
    error_handle:
        memset(debugRxBuffer,0,DEBUG_RX_BYTES);
        printf("Invalid Commands\r\n");
    save_handle:
        SystemSaveActionParamOnce();
 }



static void DebugRecvDataHandle(uint8_t recvDat)
{
    debugRxBuffer[debugRxTailer++] = recvDat;
    if (debugRxTailer > DEBUG_RX_BYTES)
    debugRxTailer = 0;
    parse_cmd(recvDat); 
    BoardRSSerialRecIT(DEBUG_UART_RS);
}

static void DebugErrHandle(void)
{

}

static void DebugRecvMsgInit(Board_RS_Serial_t serialNo)
{
    debugRxTailer = 0;  
    memset(debugRxBuffer,0,DEBUG_RX_BYTES);
    BoardRSSerialRecIT(serialNo);
}
void DebugMgrInit(void)
{
    BoardRSSerialInit(DEBUG_UART_RS,DebugRecvDataHandle,DebugErrHandle);
    DebugRecvMsgInit(DEBUG_UART_RS);
    LOG("DebugMgrInit is finish");
}

