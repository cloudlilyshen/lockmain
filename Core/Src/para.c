#include "main.h"
#include "para.h"
#include "gprs.h"
#include "util.h"


System_Action_Para_t actionPara;

void SystemLoadDefaultParamOnce(void)
{
    pAppTask_t app;          
    app = GetApp();
    memset((uint8_t*)&actionPara,0,SYSTEM_ACTION_PARA_SIZE);
    memcpy(actionPara.IP0,(uint8_t *)GPRS_DEFAULT_IP,strlen(GPRS_DEFAULT_IP));	   

    actionPara.Cabinet_No[0]=0;
    actionPara.Cabinet_No[1]=1;

    actionPara.Lock_Total_Nums=16;

    actionPara.version = app->version;
    actionPara.isConfig = 0xab;  

    actionPara.chk = ModbusCS_CalcForProtocol((uint8_t*)&actionPara,(SYSTEM_ACTION_PARA_SIZE-1));
    SystemSaveActionParamOnce(); 
}

void SystemLoadActionParamOnce(void)
{
    pAppTask_t app;          
    app = GetApp();
    memcpy((uint8_t*)&actionPara,(uint8_t*)SYSTEM_ACTION_PARA_BASE_ADDR,SYSTEM_ACTION_PARA_SIZE); 
    if(actionPara.isConfig != 0xab)
    {
        SystemLoadDefaultParamOnce();      
    }
    else if(actionPara.version != app->version)
    {
        actionPara.version = app->version;
        actionPara.chk = ModbusCS_CalcForProtocol((uint8_t*)&actionPara,(SYSTEM_ACTION_PARA_SIZE-1));        
        SystemSaveActionParamOnce();         
    }
    app->Cabinet_No = actionPara.Cabinet_No[0]<<8 + actionPara.Cabinet_No[1];
    app->Lock_Total_Nums = actionPara.Lock_Total_Nums;
}

void SystemSaveActionParamOnce(void)
{
    uint32_t PageError,dst;
    uint32_t *src;
	FLASH_EraseInitTypeDef FlashEraseInit;
	FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
	FlashEraseInit.Sector = FLASH_SECTOR_11;
	FlashEraseInit.NbSectors = 1;
    FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    dst = SYSTEM_ACTION_PARA_BASE_ADDR;
    src = (uint32_t *)&actionPara;

	HAL_FLASH_Unlock();
	if(HAL_OK != HAL_FLASHEx_Erase(&FlashEraseInit, &PageError))
    {
        __asm("nop");        
    }


    for(uint8_t i=0;i<SYSTEM_ACTION_PARA_SIZE/4;i++)
    {
	    if(HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dst, *src))
        {
            dst += 4;
            src++;
        }   
        else
        {
            __asm("nop");
        }
    }
    HAL_FLASH_Lock();
}







