#include "device_driver.h"
#include "rgb_led.h"

// 하드웨어 계층 : TIM2 CH2(PA1) PWM + DMA1_Stream1(TIM2_UP)로 CCR2 자동 갱신
// 색상/버퍼 인코딩은 ws2812.c에서 담당, 여기는 레지스터 제어만

extern volatile int WS2812_DMA_Busy;

void RGB_LED_Init(void)
{
    Macro_Set_Bit(RCC->AHB1ENR, 0);                                    // GPIOA Clock

    Macro_Write_Block(GPIOA->MODER, 0x3, 0x2, WS2812_GPIO_PIN * 2U);   // PA1 AF 고정 (TIM2_CH2)
    Macro_Clear_Bit(GPIOA->OTYPER, WS2812_GPIO_PIN);                   // Push-Pull
    Macro_Write_Block(GPIOA->PUPDR, 0x3, 0x0, WS2812_GPIO_PIN * 2U);   // No pull
    Macro_Write_Block(GPIOA->OSPEEDR, 0x3, 0x3, WS2812_GPIO_PIN * 2U); // Very High Speed

    Macro_Write_Block(GPIOA->AFR[0], 0xf, 0x1, WS2812_GPIO_PIN * 4U);  // AF01 (TIM2_CH2)

    Macro_Set_Bit(RCC->APB1ENR, 0);                                    // TIM2 Clock

    TIM2->PSC = WS2812_TIM_PSC;
    TIM2->ARR = WS2812_ARR;

    Macro_Write_Block(TIM2->CCMR1, 0xff, 0x68, 8);                     // CH2 PWM mode1 + preload
    TIM2->CCER = (0 << 5) | (1 << 4);                                  // CH2 Enable, Active High
    TIM2->CCR2 = 0;                                                    // Idle Low (Duty 0)

    Macro_Set_Bit(TIM2->EGR, 0);
    Macro_Clear_Bit(TIM2->SR, 0);

    Macro_Clear_Bit(TIM2->CR1, 0);                                     // Counter는 미시작
    Macro_Clear_Bit(RCC->APB1ENR, 0);                                  // 평소엔 Clock Off
}

void RGB_LED_Enable(void)
{
    Macro_Set_Bit(RCC->APB1ENR, 0);                                    // TIM2 Clock

    TIM2->CCR2 = 0;
    Macro_Set_Bit(TIM2->EGR, 0);                                       // Counter 초기화
    Macro_Clear_Bit(TIM2->SR, 0);
}

void RGB_LED_Disable(void)
{
    Macro_Clear_Bit(TIM2->CR1, 0);                                     // Counter Stop
    Macro_Clear_Bit(TIM2->DIER, 8);                                    // UDE Off

    Macro_Clear_Bit(RCC->APB1ENR, 0);                                  // TIM2 Clock Off
}

int RGB_LED_Is_Busy(void)
{
    return WS2812_DMA_Busy;
}

// buf(len개, unsigned int) -> DMA1_Stream1로 TIM2->CCR2에 자동 전송, 즉시 리턴(non-blocking)
void RGB_LED_Transfer(unsigned int *buf, unsigned int len)
{
    RGB_LED_Enable();

    Macro_Set_Bit(RCC->AHB1ENR, 21);                                   // DMA1 Clock

    DMA1->LIFCR = 0x3F << 6;                                           // Stream1 flag clear

    DMA1_Stream1->PAR  = (unsigned int)&TIM2->CCR2;
    DMA1_Stream1->M0AR = (unsigned int)buf;
    DMA1_Stream1->NDTR = len;
    DMA1_Stream1->FCR  = 0;

    // CHSEL=3(TIM2_UP), MSIZE/PSIZE=32bit(CCR2가 32bit 레지스터라 word 단위로 전송), MINC=1, DIR=M2P, TCIE=1
    DMA1_Stream1->CR = (0x3 << 25) | (0x2 << 13) | (0x2 << 11) | (0x1 << 10) | (0x1 << 6) | (0x1 << 4);

    NVIC_ClearPendingIRQ(12);
    NVIC_EnableIRQ(12);
    Macro_Set_Bit(DMA1_Stream1->CR, 0);                                // Stream Enable

    WS2812_DMA_Busy = 1;

    Macro_Set_Bit(TIM2->DIER, 8);                                      // UDE Enable
    Macro_Set_Bit(TIM2->CR1, 0);                                       // Counter Start
}
