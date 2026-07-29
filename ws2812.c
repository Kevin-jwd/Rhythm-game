#include "device_driver.h"
#include "rgb_led.h"

// 프로토콜 계층 : GRB/MSB first 버퍼 인코딩 + 공개 API
// 실제 전송은 rgb_led.c의 RGB_LED_Transfer(하드웨어 계층)에 위임

static unsigned int ws2812_buf[WS2812_BUF_LEN];

// buf에 24개(GRB, MSB first) code값을 채우고 채운 개수를 반환
static unsigned int WS2812_Fill_Pixel(unsigned int *buf, unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char data[WS2812_BYTES_PER_LED] = { g, r, b };
    unsigned int byte, bit, idx = 0;

    for (byte = 0; byte < WS2812_BYTES_PER_LED; byte++)
    {
        for (bit = 0; bit < WS2812_BITS_PER_BYTE; bit++)
        {
            buf[idx++] = ((data[byte] >> (WS2812_BITS_PER_BYTE - 1 - bit)) & 0x1) ? WS2812_CODE_1 : WS2812_CODE_0;
        }
    }

    return idx;
}

// 4개 LED 전체 동일 색상, non-blocking (이전 전송 중이면 무시)
void RGB_LED_Send_All(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int i, idx = 0;

    if (RGB_LED_Is_Busy())
    {
        return;
    }

    for (i = 0; i < WS2812_NUM_LEDS; i++)
    {
        idx += WS2812_Fill_Pixel(&ws2812_buf[idx], r, g, b);
    }

    for (i = 0; i < WS2812_RESET_CNT; i++)
    {
        ws2812_buf[idx++] = 0;
    }

    RGB_LED_Transfer(ws2812_buf, idx);
}

// index(0~3) 하나만 켜고 나머지는 Off, non-blocking (이전 전송 중이면 무시)
void RGB_LED_Send_One(unsigned int index, unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int i, idx = 0;

    if (RGB_LED_Is_Busy())
    {
        return;
    }

    for (i = 0; i < WS2812_NUM_LEDS; i++)
    {
        if (i == index)
        {
            idx += WS2812_Fill_Pixel(&ws2812_buf[idx], r, g, b);
        }
        else
        {
            idx += WS2812_Fill_Pixel(&ws2812_buf[idx], 0, 0, 0);
        }
    }

    for (i = 0; i < WS2812_RESET_CNT; i++)
    {
        ws2812_buf[idx++] = 0;
    }

    RGB_LED_Transfer(ws2812_buf, idx);
}
