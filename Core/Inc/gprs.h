#ifndef _GPRS_H_
#define _GPRS_H_



#define GPRS_FLAG_WAIT_TIMES  (10000)
#define GPRS_UPLOAD_WAIT_TIMES  (20000)
#define GPRS_UPLOAD_MAX_BYTES  (64)

#define GPRS_FLAG_MASK_CIFSR BIT_00
#define GPRS_FLAG_MASK_NORMAL BIT_01
#define GPRS_FLAG_MASK_CONNECT BIT_02
#define GPRS_FLAG_MASK_SEND_DAT_CMD BIT_03
#define GPRS_FLAG_MASK_SEND_DAT BIT_04

#define GPRS_FLAG_MASK_HEART_ACK BIT_05
#define GPRS_FLAG_MASK_RECORD_ACK BIT_06

// #define GPRS_FLAG_MASK_GPRS_INIT BIT_00
// #define APP_FLAG_MASK_RS485_INIT BIT_01


#define GPRS_RX_BYTES 512
#define GPRS_TX_BYTES 512
#define GPRS_UART_RS BOARD_UART1

extern uint16_t gprsRxHeader;
extern uint16_t gprsRxTailer;

#define GPRS_PWR_ON() \
    HAL_GPIO_WritePin(PWR_EN_GPIO_Port,PWR_EN_Pin,GPIO_PIN_RESET) 
#define GPRS_PWR_OFF() \
    HAL_GPIO_WritePin(PWR_EN_GPIO_Port,PWR_EN_Pin,GPIO_PIN_SET) 

#define GPRS_KEY_ON() \
    HAL_GPIO_WritePin(PWRKEY_CON_GPIO_Port,PWRKEY_CON_Pin,GPIO_PIN_SET) 
#define GPRS_KEY_OFF() \
    HAL_GPIO_WritePin(PWRKEY_CON_GPIO_Port,PWRKEY_CON_Pin,GPIO_PIN_RESET)     

// static const char  GPRS_DEFAULT_IP[]="058.211.065.178:30001";
static const char  GPRS_DEFAULT_IP[]="047.100.112.069:27002";/*at+ip=47.100.112.69:27002 */
  
typedef enum UserErrCode_e
{
  UserErrCode_None,
  UserErrCode_KeySeq,//开锁序号错误
  UserErrCode_Open,//开锁失败
  UserErrCode_No_Key,//无钥匙
  UserErrCode_Read_Card,//读卡失败
  UserErrCode_No_Ack,//下位机无回复  
  UserErrCode_End
}UserErrCode_t;



typedef struct GprsHeader_s
{
    uint8_t header[2];
    uint8_t frame[2];
    uint8_t cabinet_seq[2];   
    uint8_t Lock_Total_Nums;   
    uint8_t cmd;
    uint8_t len[2];      
}GprsHeader_t;
typedef struct GprsTailer_s
{
    uint8_t crc[2];
    uint8_t tailer[2];  
}GprsTailer_t;

typedef struct GprsOpenLock_s
{
    GprsHeader_t header;
    uint8_t lock_no;
    GprsTailer_t  tailer;
}GprsOpenLock_t;
    
typedef struct GprsOpenLockAck_s
{
    GprsHeader_t header;
    uint8_t lock_no;
    uint8_t stauts;
    GprsTailer_t  tailer;
}GprsOpenLockAck_t;

typedef struct GprsOpenLockRec_s
{
    GprsOpenLock_t GprsOpenLock;
    uint8_t bufferTx[GPRS_UPLOAD_MAX_BYTES];
}GprsOpenLockRec_t;



typedef struct GprsReadLock_s
{
    GprsHeader_t header;  
    uint8_t rev;
    GprsTailer_t  tailer;
}GprsReadLock_t;
    
typedef struct GprsReadLockAck_s
{
    GprsHeader_t header;     
    uint8_t lock_status[2];
    uint8_t stauts;    
    GprsTailer_t  tailer; 
}GprsReadLockAck_t;

typedef struct GprsReadLockRec_s
{
    GprsReadLock_t GprsReadLock;
    uint8_t bufferTx[GPRS_UPLOAD_MAX_BYTES];
}GprsReadLockRec_t;


typedef struct GprsReadValve_s
{
    GprsHeader_t header;  
    uint8_t rev;
    GprsTailer_t  tailer;
}GprsReadValve_t;
    
typedef struct GprsReadValveAck_s
{
    GprsHeader_t header;     
    uint8_t valve_status[2];
    uint8_t stauts;    
    GprsTailer_t  tailer; 
}GprsReadValveAck_t;

typedef struct GprsReadValveRec_s
{
    GprsReadValve_t GprsReadValve;
    uint8_t bufferTx[GPRS_UPLOAD_MAX_BYTES];
}GprsReadValveRec_t;


typedef struct GprsReadKeyId_s
{
    GprsHeader_t header;    
    uint8_t lock_no;
    GprsTailer_t  tailer;  
}GprsReadKeyId_t;

typedef struct GprsReadKeyIdAck_s
{
    GprsHeader_t header;   
    uint8_t lock_no;     
    uint8_t key_id[5];
    uint8_t stauts;    
    GprsTailer_t  tailer; 
}GprsReadKeyIdAck_t;

typedef struct GprsReadKeyIdRec_s
{
    GprsReadKeyId_t GprsReadKeyId;
    uint8_t bufferTx[GPRS_UPLOAD_MAX_BYTES];
}GprsReadKeyIdRec_t;

    
typedef struct GprsUploadHeart_s
{
    GprsHeader_t header;   
    uint8_t key_status[2];     
    uint8_t valve_status[2];    
    GprsTailer_t  tailer; 
}GprsUploadHeart_t;

typedef struct GprsUploadHeartAck_s
{
    GprsHeader_t header;   
    uint8_t result;       
    GprsTailer_t  tailer; 
}GprsUploadHeartAck_t;


typedef struct GprsUploadRecordDat_s
{
    uint8_t lock_no;
    uint8_t key_id[5];
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
}GprsUploadRecordDat_t;

typedef struct GprsUploadRecord_s
{
    GprsHeader_t header;   
    uint8_t lock_no;
    uint8_t key_id[5];
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    GprsTailer_t  tailer; 
}GprsUploadRecord_t;

typedef struct GprsUploadRecordAck_s
{
    GprsHeader_t header;     
    uint8_t result;
    GprsTailer_t  tailer; 
}GprsUploadRecordAck_t;


typedef struct GprsRecordSend_s
{
    GprsUploadRecordDat_t record_data;
    uint8_t bufferTx[GPRS_UPLOAD_MAX_BYTES];
}GprsRecordSend_t;


typedef struct GprsSendDat_s
{
    uint8_t *dat;
    uint16_t len;
}GprsSendDat_t,*pGprsSendDat_t;

typedef enum GprsCmd_e
{
    GprsCmd_Start,
    GprsCmd_OpenLock,
    GprsCmd_ReadLock,
    GprsCmd_ReadValve,
    GprsCmd_ReadKeyId,
    GprsCmd_Heart,
    GprsCmd_Record,

    GprsCmd_End
}GprsCmd_t;


typedef enum GprsMsgId_e
{
  GPRS_MSG_Idle = GPRS_MSG_BASE,
  GPRS_MSG_Init,  
  GPRS_MSG_OpenLock,      
  GPRS_MSG_ReadLock,      
  GPRS_MSG_ReadValve,      
  GPRS_MSG_ReadKeyId,      
  GPRS_MSG_Record,        

  GPRS_MSG_End
}GprsMsgId_t;



typedef struct GprsTask_s
{
	uint32_t GprsEventFlagMask;	    
    uint16_t GprsSendSeqno;
    UserErrCode_t UserErrCode;
    uint8_t  open_lock_err;
    uint16_t upLoaderFrame;
} GprsTask_t, *pGprsTask_t;


GprsTask_t *GetGprs(void);
void GprsMgrInit(void);
void GprsUploadDataWaitForFbUart(uint8_t  *dat,uint16_t len);
bool GprsCommWaitForFbUart(uint32_t mask);

extern uint8_t gprsTxBuffer[GPRS_TX_BYTES];
extern uint16_t gprsTxHeader;
extern uint16_t gprsTxTailer;
#endif


