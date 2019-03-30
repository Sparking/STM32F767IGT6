#ifndef __DRIVE_IWDG_H_
#define __DRIVE_IWDG_H_

#include "stm32f7xx.h"
#include "stm32f7xx_hal_rcc.h"

void IWDG_Init(void);
void IWDG_Feed(void);

#endif /* __DRIVE_IWDG_H_ */
