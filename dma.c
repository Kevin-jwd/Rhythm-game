#include "device_driver.h"
#include "stm32f411xe.h"

// CR 
// DMA 동작 설정
// bit 0          EN: 채널 enable disable
// bit 3          TCIE (Transfer COmplete Interrupt Enable)
// bit 4          HTIE (Half Transfer Interrupt enable)
// bit 6,7        DIR[1:0]: P2M (00), M2P (01), M2M (10)
// bit 8          CIRC: Cicular Mode enable -> Auto Reload Mode (M2M 제외)
// bit 9, 10      PINC MINC: peri측, mem측 1 unit size (00: 8b, 01:16b, 10:32b)
// bit 11, 12     PSIZE[1:0]: Peri측 주소 증가, 고정 모드 (0: 고정, 1: 증가)
// bit 13, 14     MSIZE[1:0]: Mem측 주소 증가, 고정 모드
// bit 25, 26, 27 CHSEL[2:0]: Channel Select

// NDTR
// 각 스트림의 Transfer Count (cnt down, 0에 도달하면 DMA가 complete으로 인지) 설정 (최대 65,535)

// MOAR
// 각 스트림의 MEM 측 0번 주소를 저장

// PAR
// 각 스트림의 PERI 측 주소

// LISR, HISR
// 8개 Stream에 대한 상태 FLag 확인 (7개 bit)
// LISR: 스트림 0~3에 대한 인터럽트 flag
// HISR: 스트림 4~7에 대한 인터럽트 flag

// LIFCR, HIFCR
// 8개 Stream에 대한 상태 Flag Clear
// 1 쓰면 clear -> 대입연산

// FCR
// F411은 M2M 모드이면 반드시 FIFO 사용

// RCC->AHB4ENR
// bit 21  DMA1EN
// bit 22  DMA2EN


void DMA2_Stream0_M2M_Start(void * src_addr, void * dst_addr, int num)
{
	Macro_Set_Bit(RCC->AHB1ENR, 22);

	// CR
	// mem 주소 증가 모드 
	// peri 주소 증가 모드
	// mem 1 unit size: 16b
	// peri 1 unit size: 16b
	// DIR: M2M (10)
	// EN: disable
    DMA2_Stream0->CR = (0x0 << 25)|(0x2 << 13)|(0x2 << 11)|(0x1 << 10)|(0x1 << 9)|(0x2 << 6)|(0x0 << 0);

	// FCR
	// DMDIS/Direct Mode Disable(FIFO를 거치지 않고 바로 Mem 접근하는 모드) 
	// (1: disable) -> M2M 모드로 설정하면 무조건 disable
	// FTH/FIFO Threshold Selection 
	// (11: 4/4)
	DMA2_Stream0->FCR = (0x1 << 2)|(0x3 << 0);
	
	// Peri 측 주소 저장
	DMA2_Stream0->PAR  = (unsigned int)src_addr;

	// Mem 측 0번 주소 저장
	DMA2_Stream0->M0AR = (unsigned int)dst_addr;

	// Transfer Count 설정
	DMA2_Stream0->NDTR = num;
	
	// LIFCR/HIFCR
	// 8개 Stream에 대한 flag clear
	// 7bit
	DMA2->LIFCR = 0x3F << 0;

	// TCIE: Transfer Complete Interrupt Enable
	Macro_Set_Bit(DMA2_Stream0->CR, 4);
	NVIC_ClearPendingIRQ(56);
	NVIC_EnableIRQ(56);
	
	// EN: enable
	Macro_Set_Bit(DMA2_Stream0->CR, 0);	
}

void DMA1_Stream6_USART2_TX_Start(void * src_addr, int num)
{
	Macro_Set_Bit(RCC->AHB1ENR, 21);

	// mem 주소 증가 모드
	// peri 주소 고정 모드
	// CHSEL: 100
	// MSIZE: 00 (8b)
	// PSIZE: 00 (8b)
	// MINC : 1  (INC)
	// PINC : 0  (FIX)
	// DIR  : 01 (M2P)
	// EN   : 0  (disable)
    DMA1_Stream6->CR = (0x4 << 25)|(0x0 << 13)|(0x0 << 11)|(0x1 << 10)|(0x0 << 9)|(0x1 << 6)|(0x0 << 0);
	DMA1_Stream6->FCR = 0;

	DMA1_Stream6->PAR  = (unsigned int)&USART2->DR;
	DMA1_Stream6->M0AR = (unsigned int)src_addr;
	DMA1_Stream6->NDTR = num;

#if 0
	DMA1->HIFCR = 0x3F << 16;
	Macro_Set_Bit(DMA1_Stream6->CR, 4);
	NVIC_ClearPendingIRQ(DMA1_Stream6_IRQn);
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
#endif

#if 1
	DMA1->HIFCR = 0x3F << 0;
	Macro_Set_Bit(DMA1_Stream6->CR, 4);
	NVIC_ClearPendingIRQ(15);
	NVIC_EnableIRQ(15);
#endif	
	Macro_Set_Bit(DMA1_Stream6->CR, 0);	
}
