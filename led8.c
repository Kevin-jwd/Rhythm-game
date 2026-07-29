#include "device_driver.h"
#include "led8.h"

// spi.c의 SPI1_SC16IS752_* 위에 얹은 얇은 래퍼, 1회 SPI 전송(16bit)이라 블로킹이어도 충분히 빠름(DMA 불필요)

void LED8_Init(void)
{
    SPI1_SC16IS752_Init(LED8_SPI_DIV);

    SPI1_SC16IS752_Write_Reg(SC16IS752_IOCONTROL, 0x00);   // GPIO[7:0] 전부 순수 GPIO로 강제
    SPI1_SC16IS752_Config_GPIO(0xff);                      // GPIO[7:0] 전부 Output
    SPI1_SC16IS752_Write_GPIO(0xff);                       // Active Low -> 전부 Off
}

// pattern bit=1 -> 해당 LED On (Active Low 반전은 여기서 처리)
void LED8_Write(unsigned char pattern)
{
    SPI1_SC16IS752_Write_GPIO((unsigned char)~pattern);
}
