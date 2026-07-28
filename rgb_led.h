#ifndef __RGB_LED_H__
#define __RGB_LED_H__

// WS2812B 4구 보드 : PA1 -> AF01 (TIM2_CH2), TIMXCLK=96MHz, 1tick=10.4166ns
// PA2/PA3는 Uart2_Init()에서 Lock 되어 재설정 불가 -> PA1 사용

#define WS2812_GPIO_PIN      (1U)       // PA1

#define WS2812_TIM_PSC       (0U)

#define WS2812_ARR           (119U)     // 120tick = 1.25us (800KHz)
#define WS2812_CODE_0        (38U)      // T0H 0.4us
#define WS2812_CODE_1        (82U)      // T1H 0.85us

#define WS2812_RESET_CNT     (45U)      // 56.25us Low (Latch, >=50us)

#define WS2812_BITS_PER_BYTE (8U)

#define WS2812_NUM_LEDS      (4U)

#endif /* __RGB_LED_H__ */
