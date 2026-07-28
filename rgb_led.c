#include "device_driver.h"
#include "rgb_led.h"

// TIM2 CH2 800KHz PWM, 비트마다 CCR2를 0-code/1-code로 갱신
// GRB 순서, MSB first, 필요할 때만 Enable/Disable로 PWM On/Off

void RGB_LED_Init(void)
{
    Macro_Set_Bit(RCC->AHB1ENR, 0);                                    // GPIOA Clock

    Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, WS2812_GPIO_PIN * 2U);   // PA1 Output
    Macro_Clear_Bit(GPIOA->OTYPER, WS2812_GPIO_PIN);                   // Push-Pull
    Macro_Write_Block(GPIOA->PUPDR, 0x3, 0x0, WS2812_GPIO_PIN * 2U);   // No pull
    Macro_Write_Block(GPIOA->OSPEEDR, 0x3, 0x3, WS2812_GPIO_PIN * 2U); // Very High Speed
    Macro_Clear_Bit(GPIOA->ODR, WS2812_GPIO_PIN);                      // Idle Low

    Macro_Write_Block(GPIOA->AFR[0], 0xf, 0x1, WS2812_GPIO_PIN * 4U);  // AF01 (TIM2_CH2)

    Macro_Set_Bit(RCC->APB1ENR, 0);                                    // TIM2 Clock

    TIM2->PSC = WS2812_TIM_PSC;
    TIM2->ARR = WS2812_ARR;

    Macro_Write_Block(TIM2->CCMR1, 0xff, 0x68, 8);                     // CH2 PWM mode1 + preload
    TIM2->CCER = (0 << 5) | (1 << 4);                                  // CH2 Enable, Active High
    TIM2->CCR2 = 0;

    Macro_Set_Bit(TIM2->EGR, 0);
    Macro_Clear_Bit(TIM2->SR, 0);

    Macro_Clear_Bit(TIM2->CR1, 0);                                     // Counter는 미시작
    Macro_Clear_Bit(RCC->APB1ENR, 0);                                  // 평소엔 Clock Off
}

void RGB_LED_Enable(void)
{
    Macro_Set_Bit(RCC->APB1ENR, 0);

    TIM2->CCR2 = 0;
    Macro_Set_Bit(TIM2->EGR, 0);                                       // 카운터 초기화
    Macro_Clear_Bit(TIM2->SR, 0);

    Macro_Write_Block(GPIOA->MODER, 0x3, 0x2, WS2812_GPIO_PIN * 2U);   // PA1 AF (TIM2_CH2)

    Macro_Set_Bit(TIM2->CR1, 0);                                       // Counter Enable
}

void RGB_LED_Disable(void)
{
    Macro_Clear_Bit(TIM2->CR1, 0);

    Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, WS2812_GPIO_PIN * 2U);   // PA1 Output
    Macro_Clear_Bit(GPIOA->ODR, WS2812_GPIO_PIN);                      // Low

    Macro_Clear_Bit(RCC->APB1ENR, 0);                                  // TIM2 Clock Off
}

// RGB_LED_Enable() 상태에서만 호출
static void WS2812_Send_Bit(int bit)
{
    TIM2->CCR2 = bit ? WS2812_CODE_1 : WS2812_CODE_0;

    while (Macro_Check_Bit_Clear(TIM2->SR, 0));    // Update Event 대기
    Macro_Clear_Bit(TIM2->SR, 0);
}

static void WS2812_Send_Byte(unsigned char data)
{
    int i;

    for (i = WS2812_BITS_PER_BYTE - 1; i >= 0; i--)
    {
        WS2812_Send_Bit((data >> i) & 0x1);
    }
}

// RGB_LED_Enable() 상태에서만 호출, Latch(>=50us Low)
void RGB_LED_Reset(void)
{
    unsigned int i;

    TIM2->CCR2 = 0;

    for (i = 0; i < WS2812_RESET_CNT; i++)
    {
        while (Macro_Check_Bit_Clear(TIM2->SR, 0));
        Macro_Clear_Bit(TIM2->SR, 0);
    }
}

// RGB_LED_Enable() 상태에서만 호출, 1개 픽셀(24bit, GRB) 전송
void RGB_LED_Send(unsigned char r, unsigned char g, unsigned char b)
{
    WS2812_Send_Byte(g);
    WS2812_Send_Byte(r);
    WS2812_Send_Byte(b);
}

// 4개 LED 전체 동일 색상 출력 (Enable/Reset/Disable 자동 처리)
void RGB_LED_Send_All(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int i;

    RGB_LED_Enable();

    for (i = 0; i < WS2812_NUM_LEDS; i++)
    {
        RGB_LED_Send(r, g, b);
    }

    RGB_LED_Reset();
    RGB_LED_Disable();
}

// index(0~3) 하나만 켜고 나머지는 Off (Enable/Reset/Disable 자동 처리)
void RGB_LED_Send_One(unsigned int index, unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int i;

    RGB_LED_Enable();

    for (i = 0; i < WS2812_NUM_LEDS; i++)
    {
        if (i == index)
        {
            RGB_LED_Send(r, g, b);
        }
        else
        {
            RGB_LED_Send(0, 0, 0);
        }
    }

    RGB_LED_Reset();
    RGB_LED_Disable();
}
