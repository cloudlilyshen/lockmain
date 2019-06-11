#ifndef _PANIC_H
#define _PANIC_H

typedef enum PanicReason_e
{
/* 0x0*/    PANIC_START,
/* 0x1*/    QUEUE_CREATE_PANIC,
/* 0x2*/    QUEUE_SEND_PANIC,
/* 0x3*/    QUEUE_RECEIVE_PANIC,
/* 0x4*/    QMC5883L_SELFTEST_PANIC,
/* 0x5*/    QMC5883L_READ_PANIC,
/* 0x6*/    INTER_FLASH_PANIC,
/* 0x7*/    LORA_AT_NO_ACK_PANIC,
/* 0x8*/    GPRS_AT_NO_ACK_PANIC,
/* 0x9*/    GPRS_SEND_DAT_NO_ACK_PANIC,
/* 0xa*/    LORA_DEVICE_OVER,
/* 0xb*/    SEMAPHORE_CREATE_PANIC,
/* 0xc*/    DMA_EVENT_INT_SELECT_PANIC,
/* 0xd*/    DMA_CHANNEL_EXEC_PANIC,
/* 0xe*/    CACHE_SYSTEM_CLEAN_PANIC,   
/* 0xf*/     OSQ_PEND_OS_ERR_PANIC,
/*0x10*/    MEM_GET_NO_FREE_BLK_PANIC,
/*0x11*/    PC_TO_ARM_REC_MSG_PANIC,
/*0x12*/    MBOX_PEND_PANIC,
/*0x13*/    OSSEM_CREATE_PANIC,
/*0x14*/    OSMAIL_CREATE_PANIC,
/*0x15*/    OSSEM_POST_PANIC,
/*0x16*/    OSSEM_PEND_PANIC,
/*0x17*/    CACHE_SYSTEM_VALIDATE_PANIC,  
/*0x18*/    DDR_CALIBRATION_PANIC,  
/*0x19*/    RS485_FLAG_NO_ACK_PANIC,

/* */            PANIC_end
} PanicReason_t;

 void _Panic(PanicReason_t reason);
#endif

