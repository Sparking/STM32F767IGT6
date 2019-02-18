#include "iwdg.h"

void IWDG_Init(void)
{
        __HAL_RCC_LSI_ENABLE();
        while (RCC->CSR & 0x00000002U)
                continue;
        IWDG->KR = 0x0000CCCCU; /* 使能独立看门狗 */
        IWDG->KR = 0x00005555U; /* 使能寄存器访问 */
        IWDG->PR = 0x00000002U; /* 64分频，频率为500Hz */
        IWDG->RLR = 250;
        while (IWDG->SR)
                continue;
}

void IWDG_Feed(void)
{
        IWDG->KR = 0x0000AAAAU; /* 复位看门狗 */
}
