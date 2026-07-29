#ifndef __LED8_H__
#define __LED8_H__

// 보드 내장 8개 LED : SC16IS752 GPIO[7:0]에 물려있음, SPI로 제어
// Active Low + 외부 Pull-up 내장 -> LED8_Write()는 Active High로 뒤집어서 제공

#define LED8_SPI_DIV    (32U)      // SPI Clock = PCLK2/32 = 3MHz (SC16IS752 SPI 최대 4MHz)
#define LED8_COUNT      (8U)

#endif /* __LED8_H__ */
