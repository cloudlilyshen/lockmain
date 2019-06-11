#ifndef __UTIL_H_
#define __UTIL_H_

uint8_t ModbusCS_CalcForProtocol(uint8_t* dat,uint16_t len);

uint32_t FourBytes2uint32BigEndian(uint8_t* dat);
int32_t FourBytes2int32BigEndian(uint8_t* dat);
uint32_t FourBytes2uint32LittleEndian(uint8_t* dat);
int32_t FourBytes2int32LittleEndian(uint8_t* dat);

uint32_t ThreeBytes2uint32BigEndian(uint8_t* dat);
int32_t ThreeBytes2int32BigEndian(uint8_t* dat);
uint32_t ThreeBytes2uint32LittleEndian(uint8_t* dat);
int32_t ThreeBytes2int32LittleEndian(uint8_t* dat);

uint16_t TwoBytes2uint16BigEndian(uint8_t* dat);
int16_t TwoBytes2int16BigEndian(uint8_t* dat);
uint16_t TwoBytes2uint16LittleEndian(uint8_t* dat);
int16_t TwoBytes2int16LittleEndian(uint8_t* dat);


void uint32ToFourBytesBigEndian(uint32_t dat,uint8_t* dst);
void uint32ToFourBytesLittleEndian(uint32_t dat,uint8_t* dst);

uint8_t GprsCheckHeadTailCRC16(uint8_t* dat,uint16_t len);
uint8_t Rs485CheckHeadTailCS(uint8_t* dat,uint16_t len);
void Rs485CreatePackage(uint8_t* dst,uint8_t id,uint8_t cmd,uint16_t status,uint8_t err);
uint8_t get_chk_bcc(uint8_t *dat, uint32_t len);
uint16_t get_crc16(uint8_t *data, uint32_t len);
bool isEqualcrc16(uint16_t crc16,uint8_t crc_low,uint8_t crc_high);
uint16_t uartCreateRespDataPackage(uint8_t* sendBuffer,uint16_t frame,uint16_t canbit_no,uint16_t lock_num,uint8_t cmd,uint8_t *dat,uint16_t len);

#endif





