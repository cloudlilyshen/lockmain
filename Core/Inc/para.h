#ifndef __PARA_H_
#define __PARA_H_



#define  STM32F407ZG_FLASH_SIZE  (0x100000)
#define  STM32F407ZG_FLASH_BASE  (0x8000000)
#define  STM32F407ZG_PAGE_SIZE (0x4000)
#define  STM32F407ZG_PAGE_NUM (STM32F407ZG_FLASH_SIZE/STM32F407ZG_PAGE_SIZE)

#define  SYSTEM_ACTION_PARA_CONFIG_PAGE  (STM32F407ZG_PAGE_NUM - 1)
// #define  SYSTEM_ACTION_PARA_BASE_ADDR  (STM32F407ZG_FLASH_BASE + (STM32F407ZG_PAGE_SIZE * SYSTEM_ACTION_PARA_CONFIG_PAGE))
#define  SYSTEM_ACTION_PARA_BASE_ADDR  0x080E0000



typedef struct System_Action_Para_s
{
    uint8_t  IP0[4];
    uint8_t  IP1[4];
    uint8_t  IP2[4];
    uint8_t  IP3[4];
		
    uint8_t  Port[5];
    uint8_t  Cabinet_No[2];   
    uint8_t  rev01[1];    

    uint8_t  Lock_Total_Nums; 
    uint8_t  version;   
    uint8_t  panic;      
    uint8_t  isConfig; 

    uint8_t rev02[3];
    uint8_t chk;           
}System_Action_Para_t,*pSystem_Action_Para_t;



#define SYSTEM_ACTION_PARA_SIZE (sizeof(System_Action_Para_t)/sizeof(uint8_t))


extern System_Action_Para_t actionPara;

void SystemReadActionParaWithIndex(uint8_t mainIndex, uint8_t subIndex,int32_t* paramValue);


void SystemWriteActionParamWithIndex(uint8_t mainIndex, uint8_t subIndex,int32_t paramValue);


void SystemLoadActionParamOnce(void);
void SystemLoadDefaultParamOnce(void);
void SystemSaveActionParamOnce(void);

#endif



