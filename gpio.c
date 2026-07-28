#include "device_driver.h"

typedef enum t_PORT
{
    A = 0U,
    B = 1U,
    C = 2U,
    D = 3U,
    E = 4U,
    H = 7U
} PORT;

typedef enum t_GPIO_MODE
{
    INPUT_MODE  = 0x00U,
    OUTPUT_MODE = 0x01U,
    AF_MODE     = 0x10U,
    ANALOG_MODE = 0x11U,
} GPIO_MODE;

typedef enum t_GPIO_TYPE
{
    PUSH_PULL_TYPE  = 0x0U,
    OPEN_DRAIN_TYPE = 0x1U,
} GPIO_TYPE;

void GPIO_Init(PORT port, char pin, char mode, char type)
{
    // CLK enable
    Macro_Set_Bit(RCC->AHB1ENR, port);

    // MODER
    Macro_Write_Block()
    // OTYPER

    // ODR
}