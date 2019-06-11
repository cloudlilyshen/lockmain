#include "main.h"
#include "panic.h"
#include "msg.h"
#include "para.h"
AppTask_t* app;

void panic(void)
{
    while(1);//panic here,not go ahead
}
 void _Panic(PanicReason_t reason)
 {
	if(actionPara.panic !=reason)	
	{
		actionPara.panic=reason;	
		// Inter_Flash_Program_Reset(FALSE);
	}
   panic();
 }

