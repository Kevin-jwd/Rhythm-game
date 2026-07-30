#include "device_driver.h"
#include "led8.h"
#include "rgb_led.h"
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

#define CHASE_STEP_MS   (300U)     // LED 하나 켜진 상태 유지 시간
#define CHASE_FULL      (255U)     // RGB Full 밝기

static void Delay_ms(unsigned int msec)
{
    SysTick_Run(msec);
    while(!SysTick_Check_Timeout());
}

void Main(void)
{
    unsigned int i;

    Sys_Init(BAUD_RATE);

    RGB_LED_Init();
    LED8_Init();

    for(;;)
    {
        for (i = 0; i < LED8_COUNT; i++)                // RGB, LED8 같은 타이밍으로 나란히 진행
        {
            RGB_LED_Send_One(i % WS2812_NUM_LEDS, 40, 40, 40);
            LED8_Write(1U << i);
            Delay_ms(CHASE_STEP_MS);
        }
    }
}
