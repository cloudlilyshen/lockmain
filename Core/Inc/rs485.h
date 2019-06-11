#ifndef _RS485_H_
#define _RS485_H_

#define RS485_UART_SERIAL BOARD_UART6
#define RS485_DOWNLOAD_WAIT_TIMES  (1000)


extern osMessageQId rs485MsgHandle;


#define RS485_READ_FLAG_MASK BIT_00
#define RS485_OPEN_LOCK_FLAG_MASK BIT_01
#define RS485_CONNFIG_FLAG_MASK BIT_02


#define RS485_RX_BYTES 128
#define RS485_TX_BYTES 128
#define RS485_UART_RS BOARD_UART6

extern uint16_t rs485RxHeader;
extern uint16_t rs485RxTailer;

#define RS485_DE_SEND() \
    HAL_GPIO_WritePin(RS485_DE_GPIO_Port,RS485_DE_Pin,GPIO_PIN_SET) 
#define RS485_DE_REC() \
    HAL_GPIO_WritePin(RS485_DE_GPIO_Port,RS485_DE_Pin,GPIO_PIN_RESET) 

enum
{
  nokey_unlocked,
  havekey_locked,
  nokey_locked,//已上锁，但读不到ID
};


typedef enum Rs485Cmd_e
{
    Rs485Cmd_Start,
    Rs485Cmd_Read,
    Rs485Cmd_Open,
    Rs485Cmd_Config,    


    Rs485Cmd_End
}Rs485Cmd_t;

typedef struct Rs485Comm_s  
{
    uint8_t header;
    uint8_t lock_no;          
    uint8_t cmd;  
    uint8_t key_id[5];         
    uint8_t lock_status;            
    uint8_t err_code;               
    uint8_t chk;  
    uint8_t tailer;         
}Rs485Comm_t,*pRs485Comm_t;    


typedef struct LockMsg_s  
{
    uint8_t key_id[5];             
    uint8_t lock_status;                
    uint8_t err_code;                
}LockMsg_t,*pLockMsg_t;   

typedef enum Rs485MsgId_e
{
  RS485_MSG_Idle = RS485_MSG_BASE,
  RS485_MSG_Init,  
  RS485_MSG_Open,
  RS485_MSG_Record,

  RS485_MSG_End
}Rs485MsgId_t;

typedef struct Rs485Task_s
{
    uint32_t rs485CmdMask;	 
    uint8_t lockId;    
}Rs485Task_t, *pRs485Task_t;

extern uint16_t lock_status;     
extern uint16_t valve_status; 
extern pLockMsg_t keyStatusArray;

void RS485MgrInit(void);
bool Rs485SendWaitForFb(uint8_t cmd,uint8_t lock_id,uint32_t mask);
Rs485Task_t *GetRs485(void);


#endif


