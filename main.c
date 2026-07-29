#include "device_driver.h"
#include <stdio.h>
#include <string.h>

static void Sys_Init(int baud) 
{
	SCB->CPACR |= (0x3 << 10*2)|(0x3 << 11*2); 
	Clock_Init();
	Uart2_Init(baud);
	setvbuf(stdout, NULL, _IONBF, 0);
	LED_Init();
}

#if 1

// RGB_LED(WS2812B) 간단 확인용 예제
// 4번째(마지막) LED만 켜서 빨강 -> 초록 -> 파랑 -> 꺼짐 순서로 반복

#define RGB_TEST_SETTLE_MS      (100U)     // 전원 인가 직후 안정화 대기 시간
#define RGB_TEST_STEP_MS        (500U)     // 색상 간 대기 시간
#define RGB_TEST_LED_INDEX      (3U)       // 테스트 대상 LED (0=1번 ~ 3=마지막 4번)
#define RGB_TEST_FULL           (255U)     // Full 밝기 값
#define RGB_TEST_NONE           (0U)       // Off 값
#define BAUD_RATE               (115200U)

static void Delay_ms(unsigned int msec)
{
    SysTick_Run(msec);
    while(!SysTick_Check_Timeout());
}

void Main(void)
{
    Sys_Init(BAUD_RATE);
    printf("RGB LED(WS2812B) Test\n");

    RGB_LED_Init();

    RGB_LED_Send_All(RGB_TEST_NONE, RGB_TEST_NONE, RGB_TEST_NONE);   // 전원 안정화 대기용 Off
    Delay_ms(RGB_TEST_SETTLE_MS);

    for(;;)   // 4번째 LED만 켜고 나머지는 Off (전압 강하 영향 분리 확인용)
    {
        RGB_LED_Send_One(RGB_TEST_LED_INDEX, RGB_TEST_FULL, RGB_TEST_NONE, RGB_TEST_NONE);   // Red
        Delay_ms(RGB_TEST_STEP_MS);

        RGB_LED_Send_One(RGB_TEST_LED_INDEX, RGB_TEST_NONE, RGB_TEST_FULL, RGB_TEST_NONE);   // Green
        Delay_ms(RGB_TEST_STEP_MS);

        RGB_LED_Send_One(RGB_TEST_LED_INDEX, RGB_TEST_NONE, RGB_TEST_NONE, RGB_TEST_FULL);   // Blue
        Delay_ms(RGB_TEST_STEP_MS);

        RGB_LED_Send_One(RGB_TEST_LED_INDEX, RGB_TEST_NONE, RGB_TEST_NONE, RGB_TEST_NONE);   // Off
        Delay_ms(RGB_TEST_STEP_MS);
    }
}

#endif
