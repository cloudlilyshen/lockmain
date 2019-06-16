#include "main.h"
#include "util.h"


static const unsigned aucCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40
};

static const unsigned aucCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
	0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
	0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
	0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
	0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
	0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
	0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
	0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
	0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
	0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
	0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
	0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
	0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
	0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
	0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
	0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
	0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
	0x41, 0x81, 0x80, 0x40
};

uint16_t get_crc16(uint8_t *data, uint32_t len)
{
	uint8_t ucCRCHi = 0xFF;
	uint8_t ucCRCLo = 0xFF;
	uint16_t index = 0;
	int dataIndex = 0;

	while(len--)
	{
              index = ucCRCLo ^ data[dataIndex];
              ucCRCLo = (unsigned char)(ucCRCHi ^ aucCRCHi[index]);
              ucCRCHi = aucCRCLo[index];
              dataIndex++;
	}
	return (uint16_t)( ucCRCHi << 8 | ucCRCLo );
}


bool isEqualcrc16(uint16_t crc16,uint8_t crc_low,uint8_t crc_high)
{
    if(crc16==(crc_low|(crc_high<<8)))
    return TRUE;
    return FALSE;
}


void Rs485CreatePackage(uint8_t* dst,uint8_t id,uint8_t cmd,uint16_t status,uint8_t err)
{
	dst[0] = '#';
	dst[1] = (uint8_t)id;		
	dst[2] = cmd;
	memset(&dst[3],0,5);
	dst[8] = status;
	dst[9] = err;		
	dst[10] = ModbusCS_CalcForProtocol(dst,10);	
	dst[11] = '*'; 	
}


uint8_t ModbusCS_CalcForProtocol(uint8_t* dat,uint16_t len)
{
    uint16_t csData = 0;
	uint8_t csDataMod=0;

    
	while(len--)
	{
		csData   += *(dat++);
	}
	csDataMod = csData%256;
	return csDataMod;	
}

uint8_t ModbusCS_CalcNegForProtocol(uint8_t* dat,uint16_t len)
{
    uint16_t csData = 0;
	uint8_t csDataMod=0;

    
	while(len--)
	{
		csData += *(dat++);
	}
	csDataMod = csData%256;
	csDataMod = ~csDataMod;
	return csDataMod;	
}


uint8_t get_chk_bcc(uint8_t *dat, uint32_t len)
{
    uint8_t temp=0;
    for(uint8_t i=0;i<len;i++)
    {
        temp=dat[i]^temp;
    }
    return (temp);
}



uint8_t Rs485CheckHeadTailCS(uint8_t* dat,uint16_t len)
{
	if((dat[0] != '#')||(dat[len-1] != '*'))
	{
		return 0;
	}
	if(ModbusCS_CalcForProtocol(dat,(len-2)) != (dat[len-2]))
	{
		return 0;
	}
	return 1;
}

uint8_t GprsCheckHeadTailCRC16(uint8_t* dat,uint16_t len)
{
	uint16_t crc16;

    //header,tail
	if((dat[0] != '#')||(dat[1] != '#'))
	return 0;

	if((dat[len-1] != '*')||(dat[len-2] != '*'))
	return 0;

	crc16 = get_crc16(dat,(len-4));
	if(!isEqualcrc16(crc16,dat[len-3],dat[len-4]))  
	return 0; 	

	return 1;
}


uint32_t FourBytes2uint32BigEndian(uint8_t* dat)
{
	return (dat[0]<<24)+(dat[1]<<16)+(dat[2]<<8)+dat[3];
}

int32_t FourBytes2int32BigEndian(uint8_t* dat)
{
	return (dat[0]<<24)+(dat[1]<<16)+(dat[2]<<8)+dat[3];
}

uint32_t FourBytes2uint32LittleEndian(uint8_t* dat)
{
	return (dat[3]<<24)+(dat[2]<<16)+(dat[1]<<8)+dat[0];
}

int32_t FourBytes2int32LittleEndian(uint8_t* dat)
{
	return (dat[3]<<24)+(dat[2]<<16)+(dat[1]<<8)+dat[0];
}


uint32_t ThreeBytes2uint32BigEndian(uint8_t* dat)
{
	return (dat[0]<<16)+(dat[1]<<8)+dat[2];
}

int32_t ThreeBytes2int32BigEndian(uint8_t* dat)
{
	return (dat[0]<<16)+(dat[1]<<8)+dat[2];
}

uint32_t ThreeBytes2uint32LittleEndian(uint8_t* dat)
{
	return (dat[2]<<16)+(dat[1]<<8)+dat[0];
}

int32_t ThreeBytes2int32LittleEndian(uint8_t* dat)
{
	return (dat[2]<<16)+(dat[1]<<8)+dat[0];
}


uint16_t TwoBytes2uint16BigEndian(uint8_t* dat)
{
	return ((dat[0]<<8)+dat[1]);
}

int16_t TwoBytes2int16BigEndian(uint8_t* dat)
{
	return ((dat[0]<<8)+dat[1]);
}

uint16_t TwoBytes2uint16LittleEndian(uint8_t* dat)
{
	return ((dat[1]<<8)+dat[0]);
}

int16_t TwoBytes2int16LittleEndian(uint8_t* dat)
{
	return ((dat[1]<<8)+dat[0]);
}

void uint32ToFourBytesBigEndian(uint32_t dat,uint8_t* dst)
{
	*dst++ = (dat>>24);
	*dst++ = (dat>>16);
	*dst++ = (dat>>8);
	*dst = dat;
}


void uint32ToFourBytesLittleEndian(uint32_t dat,uint8_t* dst)
{
	*dst++ = dat;
	*dst++ = (dat>>8);
	*dst++ = (dat>>16);
	*dst = (dat>>24);
}

uint16_t  uartCreateRespDataPackage(uint8_t* sendBuffer,uint16_t frame,uint16_t cabinet_no,uint16_t Lock_Total_Nums,uint8_t cmd,uint8_t *dat,uint16_t len)
{
	uint16_t crc16;
    sendBuffer[0] = '#';
	sendBuffer[1] = '#';
	sendBuffer[2] = (uint8_t)(frame>>8);
	sendBuffer[3] = (uint8_t)frame;
	sendBuffer[4] = (uint8_t)(cabinet_no>>8);
	sendBuffer[5] = (uint8_t)cabinet_no;
	sendBuffer[6] = Lock_Total_Nums;
    sendBuffer[7] = cmd;
    sendBuffer[8] = (uint8_t)(len>>8);
    sendBuffer[9] = (uint8_t)(len);	
	memcpy(&sendBuffer[10],dat,len);

 	crc16 = get_crc16(sendBuffer,(10+len));	
	sendBuffer[10+len] = (uint8_t)(crc16>>8);
	sendBuffer[10+len+1] = (uint8_t)crc16;
    sendBuffer[10+len+2] = '*';
    sendBuffer[10+len+3] = '*';
	return (len+14);
}











