#include "device_driver.h"

#define PORTA_2_PIN (2U)

void RGB_LED_Init(void)
{
    Macro_Set_Bit(RCC->AHB1ENR, 0); 
    
    // Alternate function
    Macro_Write_Block(GPIOA->MODER, 0x3, 0x2, PORTA_2_PIN * 2U);

    // AFR 
    // AF01: TIM2_CH3
    // AF02: TIM5_CH3
    // AF03: TIM9_CH1
    Macro_Write_Block(GPIOA->AFR[0], 0xf, 0x1111, 0);

    
    // Push pull
	Macro_Clear_Bit(GPIOA->OTYPER, 5);
    
    // RESET signal above 50us delay
	Macro_Clear_Bit(GPIOA->ODR, 5); 
    // _delay_?

    // 
}

void Send_High(void)
{
    
}

void Send_Low(void)
{

}