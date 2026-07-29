#include "device_driver.h"
#include "led8.h"
#include <stdio.h>
#include <string.h>

#define BAUD_RATE   (115200U)

static void Sys_Init(int baud)
{
	SCB->CPACR |= (0x3 << 10*2)|(0x3 << 11*2);
	Clock_Init();
	Uart2_Init(baud);
	setvbuf(stdout, NULL, _IONBF, 0);
	LED_Init();
}

void Main(void)
{
    Sys_Init(BAUD_RATE);

    RGB_LED_Init();
    LED8_Init();

    for(;;)
    {
    }
}
