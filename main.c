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

#if 0

extern volatile int TIM4_Expired;

#define NUM_OF_DATA 40

unsigned int src1_dat[NUM_OF_DATA];
unsigned int dst1_dat[NUM_OF_DATA];

void Main(void)
{
	unsigned int i=0;

	Sys_Init(115200);
	printf("Memory Copy Test\n");

	TIM4_Repeat_Interrupt_Enable(1, 1000);

	for(i=0;i<NUM_OF_DATA;i++)
	{
		src1_dat[i] = i + 8;
		dst1_dat[i] = 0;
	}

	for(i=0;i<NUM_OF_DATA;i++)
	{
		dst1_dat[i] = src1_dat[i];
	}

	for(i=0;i<NUM_OF_DATA;i++)
	{
		printf("dst1_dat[%d] = %d\n",i,dst1_dat[i]);
		dst1_dat[i] = 0;
	}

	for(;;)
	{
		if(TIM4_Expired)
		{
			static unsigned int led = 0;
			LED_Display(led ^= 0x1);
			TIM4_Expired = 0;
		}
	}
}

#endif

#if 0

extern volatile int TIM4_Expired;
extern volatile int DMA2_STREAM0_DONE;

#define NUM_OF_DATA     40

unsigned int src1_dat[NUM_OF_DATA];
unsigned int dst1_dat[NUM_OF_DATA];

void Main(void)
{
    unsigned int i = 0;

    Sys_Init(115200);
    printf("DMA Software Trigger Test(STM32F411)\n");

    TIM4_Repeat_Interrupt_Enable(1, 250);

    for(i = 0; i < NUM_OF_DATA; i++)
    {
        src1_dat[i] = i + 8;
        dst1_dat[i] = 0;
    }

    DMA2_Stream0_M2M_Start(src1_dat, dst1_dat, NUM_OF_DATA);

    for(;;)
    {
        if(DMA2_STREAM0_DONE)
        {
            printf("\nDMA2-Stream0 Complete!\n");

            for(i = 0; i < NUM_OF_DATA; i++)
            {
                printf("@dst1_dat[%d] = %d\n", i, dst1_dat[i]);
                dst1_dat[i] = 0;
            }

            DMA2_STREAM0_DONE = 0;
        }

        if(TIM4_Expired)
        {
            static unsigned int led = 0;
            LED_Display(led ^= 0x1);
            TIM4_Expired = 0;
        }
    }
}

#endif

#if 0

extern volatile int TIM4_Expired;

const char * str[] = {
    "[1] The DMA controller performs direct memory transfer by sharing the system bus with the Cortex™-M3 core.\n"
	"[2] The DMA request may stop the CPU access to the system bus for some bus cycles,\n"
	"[3] when the CPU and DMA are targeting the same destination (memory or peripheral).\n"
	"[4] The bus matrix implements round-robin scheduling, thus ensuring at least half of the system bus bandwidth (both to memory and peripheral) for the CPU.\n"
	"[5] After an event, the peripheral sends a request signal to the DMA Controller.\n"
	"[6] The DMA controller serves the request depending on the channel priorities.\n"
	"[7] As soon as the DMA Controller accesses the peripheral, an Acknowledge is sent to the peripheral by the DMA Controller.\n"
	"[8] The peripheral releases its request as soon as it gets the Acknowledge from the DMA Controller.\n"
	"[9] Once the request is deasserted by the peripheral, the DMA Controller release the Acknowledge.\n"
"[1] The DMA controller performs direct memory transfer by sharing the system bus with the Cortex™-M3 core.\n"
	"[2] The DMA request may stop the CPU access to the system bus for some bus cycles,\n"
	"[3] when the CPU and DMA are targeting the same destination (memory or peripheral).\n"
	"[4] The bus matrix implements round-robin scheduling, thus ensuring at least half of the system bus bandwidth (both to memory and peripheral) for the CPU.\n"
	"[5] After an event, the peripheral sends a request signal to the DMA Controller.\n"
	"[6] The DMA controller serves the request depending on the channel priorities.\n"
	"[7] As soon as the DMA Controller accesses the peripheral, an Acknowledge is sent to the peripheral by the DMA Controller.\n"
	"[8] The peripheral releases its request as soon as it gets the Acknowledge from the DMA Controller.\n"
	"[9] Once the request is deasserted by the peripheral, the DMA Controller release the Acknowledge.\n"
    "[1] The DMA controller performs direct memory transfer by sharing the system bus with the Cortex™-M3 core.\n"
	"[2] The DMA request may stop the CPU access to the system bus for some bus cycles,\n"
	"[3] when the CPU and DMA are targeting the same destination (memory or peripheral).\n"
	"[4] The bus matrix implements round-robin scheduling, thus ensuring at least half of the system bus bandwidth (both to memory and peripheral) for the CPU.\n"
	"[5] After an event, the peripheral sends a request signal to the DMA Controller.\n"
	"[6] The DMA controller serves the request depending on the channel priorities.\n"
	"[7] As soon as the DMA Controller accesses the peripheral, an Acknowledge is sent to the peripheral by the DMA Controller.\n"
	"[8] The peripheral releases its request as soon as it gets the Acknowledge from the DMA Controller.\n"
	"[9] Once the request is deasserted by the peripheral, the DMA Controller release the Acknowledge.\n"
"[1] The DMA controller performs direct memory transfer by sharing the system bus with the Cortex™-M3 core.\n"
	"[2] The DMA request may stop the CPU access to the system bus for some bus cycles,\n"
	"[3] when the CPU and DMA are targeting the same destination (memory or peripheral).\n"
	"[4] The bus matrix implements round-robin scheduling, thus ensuring at least half of the system bus bandwidth (both to memory and peripheral) for the CPU.\n"
	"[5] After an event, the peripheral sends a request signal to the DMA Controller.\n"
	"[6] The DMA controller serves the request depending on the channel priorities.\n"
	"[7] As soon as the DMA Controller accesses the peripheral, an Acknowledge is sent to the peripheral by the DMA Controller.\n"
	"[8] The peripheral releases its request as soon as it gets the Acknowledge from the DMA Controller.\n"
	"[9] Once the request is deasserted by the peripheral, the DMA Controller release the Acknowledge.\n"
 };

void Main(void)
{
    unsigned int str_num = 0;

    Sys_Init(115200);
    printf("USART String TX Test\n\n");
    TIM4_Repeat_Interrupt_Enable(1, 100);

    for (;;)
    {
		if(str_num < (sizeof(str)/sizeof(str[0])))
		{
			printf("%s", str[str_num]);
			str_num++;
		}

        if(TIM4_Expired)
        {
            static unsigned int led = 0;
            LED_Display(led ^= 0x1);
            TIM4_Expired = 0;
        }        
    }
}

#endif

#if 0

extern volatile int TIM4_Expired;
extern volatile int DMA1_STREAM6_DONE;

const char * str[] = {
    "[1] The DMA controller performs direct memory transfer by sharing the system bus with the Cortex™-M3 core.\n"
	"[2] The DMA request may stop the CPU access to the system bus for some bus cycles,\n"
	"[3] when the CPU and DMA are targeting the same destination (memory or peripheral).\n"
	"[4] The bus matrix implements round-robin scheduling, thus ensuring at least half of the system bus bandwidth (both to memory and peripheral) for the CPU.\n"
	"[5] After an event, the peripheral sends a request signal to the DMA Controller.\n"
	"[6] The DMA controller serves the request depending on the channel priorities.\n"
	"[7] As soon as the DMA Controller accesses the peripheral, an Acknowledge is sent to the peripheral by the DMA Controller.\n"
	"[8] The peripheral releases its request as soon as it gets the Acknowledge from the DMA Controller.\n"
	"[9] Once the request is deasserted by the peripheral, the DMA Controller release the Acknowledge.\n" };

static void Uart2_Wait_for_TX_Complete(void)
{
    while(!Macro_Check_Bit_Set(USART2->SR, 6));
}

void Main(void)
{
    unsigned int str_num = 0;

    Sys_Init(115200);
    printf("DMA(M2P) Test - H/W (USART) Trigger\n\n");
    TIM4_Repeat_Interrupt_Enable(1, 1000);
    Uart2_Wait_for_TX_Complete();

    for (;;)
    {
        if (DMA1_STREAM6_DONE || (str_num == 0))
        {
            if (str_num < (sizeof(str) / sizeof(str[0])))
            {
                // uart 장치 HW trigger 설정
                // 7 DMAT: DMA enable transmitter
                // This bit is set/reset by software
                // 1: DMA mode is enabled for transmission.
                // 0: DMA mode is disabled for transmission
                Macro_Set_Bit(USART2->CR3, 7);
                DMA1_Stream6_USART2_TX_Start((void * )str[str_num], strlen(str[str_num]));
                str_num++;
                printf("\n\n\n%d\n\n\n", str_num);
                DMA1_STREAM6_DONE = 0;                
            }

            else
            {
                Macro_Clear_Bit(DMA1_Stream6->CR, 0);
                Macro_Clear_Bit(USART2->CR3, 7);
            }
        }

        if(TIM4_Expired)
        {
            static unsigned int led = 0;
            LED_Display(led ^= 0x1);
            TIM4_Expired = 0;
        }
    }
}

#endif

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
