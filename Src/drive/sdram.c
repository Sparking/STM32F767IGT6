#include "sdram.h"

void FMC_SDRAM_WriteBuffer(uint8_t *pbuffer, uint32_t write_addr, uint32_t size)
{
        for (uint32_t i = 0; i < size; i ++) {
                *(__IO uint8_t *)(Bank5_SDRAM_ADDR + write_addr + i) = *(pbuffer + i);
        }
}

void FMC_SDRAM_ReadBuffer(uint8_t *pbuffer, uint32_t read_addr, uint32_t size)
{
        for (uint32_t i = 0; i < size; i ++) {
                *(pbuffer + i) = *(__IO uint8_t *)(Bank5_SDRAM_ADDR + read_addr + i);
        }
}

void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
{
        FMC_SDRAM_CommandTypeDef Command;
        Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1; /* 目标SDRAM存储区域 */ 
        Command.CommandMode   = FMC_SDRAM_CMD_CLK_ENABLE;   /* 命令:配置时钟使能 */
        Command.AutoRefreshNumber      = 1; /* 自刷新次数 */
        Command.ModeRegisterDefinition = 0; /* 要写入模式寄存器的值 */
        HAL_SDRAM_SendCommand(hsdram, &Command, 0x1000); /* 发送命令 */
        HAL_Delay(1);   /* 这里至少要延时200us */
        Command.CommandMode = FMC_SDRAM_CMD_PALL; /* 命令:对所有存储区域预充电 */
        HAL_SDRAM_SendCommand(hsdram, &Command, 0x1000);
        Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE; /* 命令:设置自刷新次数 */
        Command.AutoRefreshNumber = 8; /* 自刷新次数为8 */
        HAL_SDRAM_SendCommand(hsdram, &Command, 0x1000);
        Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE; /* 设置加载模式 */
        /*
         *
         */
        Command.ModeRegisterDefinition = 
                SDRAM_MODEREG_BURST_LENGTH_1             |
                SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      |
                SDRAM_MODEREG_CAS_LATENCY_3              |
                SDRAM_MODEREG_OPERATING_MODE_STANDARD    |
                SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED;
        HAL_SDRAM_SendCommand(hsdram, &Command, 0x1000);
}

void SDRAM_Init(void)
{
        SDRAM_HandleTypeDef SDRAM_Handler;
        FMC_SDRAM_TimingTypeDef SDRAM_Timing;
/* FMC的SDRAM配置 */
        SDRAM_Handler.State = HAL_SDRAM_STATE_RESET;
        SDRAM_Handler.Instance                = FMC_SDRAM_DEVICE;            /* SDRAM设备 */
        SDRAM_Handler.Init.SDBank             = FMC_SDRAM_BANK1;             /* SDRAM的BANK1 */
        SDRAM_Handler.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9; /* SDRAM列地址位数 */
        SDRAM_Handler.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_13;   /* SDRAM行地址位数 */
        SDRAM_Handler.Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_16;  /* SDRAM的数据宽度 */
        SDRAM_Handler.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;/* SDRAM的BANK数量 */
        SDRAM_Handler.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;     /* SDRAM的CAS延时周期 */
        SDRAM_Handler.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE; /* 禁止SDRAM的写保护 */
        SDRAM_Handler.Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;    /* SDRAM的时钟的分频系数 */
        SDRAM_Handler.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;     /* 允许对SDRAM进行突发访问 */
        SDRAM_Handler.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_1;     /* 读通道的延时周期 */
/* SDRAM的时序设置 */
        SDRAM_Timing.LoadToActiveDelay    = 2; /* 加载模式寄存器,激活时间延迟2个周期 */
        SDRAM_Timing.ExitSelfRefreshDelay = 8; /* 退出自刷新延迟为8个时钟周期 */
        SDRAM_Timing.SelfRefreshTime      = 6; /* 自刷新时间为6个时钟周期 */
        SDRAM_Timing.RowCycleDelay        = 6; /* 行循环延迟时间为6个时钟周期 */
        SDRAM_Timing.WriteRecoveryTime    = 2; /* 写恢复延迟时间为2个时钟周期 */
        SDRAM_Timing.RPDelay              = 2; /* 行预充电延迟为2个时钟周期 */
        SDRAM_Timing.RCDDelay             = 2; /* 行到列延迟为2个时钟周期 */
        HAL_SDRAM_Init(&SDRAM_Handler, &SDRAM_Timing);
        SDRAM_Initialization_Sequence(&SDRAM_Handler);
        HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler, 683);
}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
        GPIO_InitTypeDef GPIO_FMC_SDRAM_Initure;
/*
 * PC0: FMC_SDNWE
 * PC2: FMC_SDNE0
 * PC3: FMC_SDKE0
 */
        GPIO_FMC_SDRAM_Initure.Pin   = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_FMC_SDRAM_Initure.Mode  = GPIO_MODE_AF_PP; /* AF复用推挽 */
        GPIO_FMC_SDRAM_Initure.Speed = GPIO_SPEED_HIGH; /* 高速输入输出(100MHz) */
        GPIO_FMC_SDRAM_Initure.Pull  = GPIO_NOPULL;     /* 浮空模式 */
        GPIO_FMC_SDRAM_Initure.Alternate =  GPIO_AF12_FMC;
        __HAL_RCC_GPIOC_CLK_ENABLE();
        HAL_GPIO_Init(GPIOC, &GPIO_FMC_SDRAM_Initure);
/*
 * PD0: FMC_D2
 * PD1: FMC_D3
 * PD8: FMC_D13
 * PD9: FMC_D14
 * PD10: FMC_D15
 * PD14: FMC_D0
 * PD15: FMC_D1
 */
        GPIO_FMC_SDRAM_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
            GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
        __HAL_RCC_GPIOD_CLK_ENABLE();
        HAL_GPIO_Init(GPIOD, &GPIO_FMC_SDRAM_Initure);
/*
 * PE7: FMC_D4
 * PE8: FMC_D5
 * PE9: FMC_D6
 * PE10: FMC_D7
 * PE11: FMC_D8
 * PE12: FMC_D9
 * PE13: FMC_D10
 * PE14: FMC_D11
 * PE15: FMC_D12
 */
        GPIO_FMC_SDRAM_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
            GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        __HAL_RCC_GPIOE_CLK_ENABLE();
        HAL_GPIO_Init(GPIOE, &GPIO_FMC_SDRAM_Initure);
/*
 * PF0: FMC_A0
 * PF1: FMC_A1
 * PF2: FMC_A2
 * PF3: FMC_A3
 * PF4: FMC_A4
 * PF5: FMC_A5
 * PF11: FMC_SDNRAS
 * PF12: FMC_A6
 * PF13: FMC_A7
 * PF14: FMC_A8
 * PF15: FMC_A9
 */
        GPIO_FMC_SDRAM_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
            GPIO_PIN_14 | GPIO_PIN_15;
        __HAL_RCC_GPIOF_CLK_ENABLE();
        HAL_GPIO_Init(GPIOF, &GPIO_FMC_SDRAM_Initure);
/*
 * PG0: FMC_A10
 * PG1: FMC_A11
 * PG2: FMC_A12
 * PG4: FMC_BA0
 * PG5: FMC_BA1
 * PG8: FMC_SDCLK
 * PG15: FMC_SDNWE
 */
        GPIO_FMC_SDRAM_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
            GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
        __HAL_RCC_GPIOG_CLK_ENABLE();
        HAL_GPIO_Init(GPIOG, &GPIO_FMC_SDRAM_Initure);
        __HAL_RCC_FMC_CLK_ENABLE();
}
