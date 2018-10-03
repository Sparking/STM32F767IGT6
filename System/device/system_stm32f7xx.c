/**
  ******************************************************************************
  * @file    Templates/system_stm32f7xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M7 Device Peripheral Access Layer System Source File.
  *
  *   This file provides two functions and one global variable to be called from 
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f7xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f7xx_system
  * @{
  */  
  
/** @addtogroup STM32F7xx_System_Private_Includes
  * @{
  */

#include "stm32f7xx.h"
#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)25000000) /*!< Default value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

/**
  * @}
  */

/** @addtogroup STM32F7xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F7xx_System_Private_Defines
  * @{
  */

/************************* Miscellaneous Configuration ************************/
/*!< Uncomment the following line if you need to use external SRAM or SDRAM mounted
     on STMicroelectronics EVAL/Discovery boards as data memory  */
/*!< In case of EVAL/Discovery�s LCD use in application code, the DATA_IN_ExtSDRAM define
     need to be added  in the project preprocessor to avoid SDRAM multiple configuration
     (the LCD uses SDRAM as frame buffer, and its configuration is done by the BSP_SDRAM_Init()) */
/* #define DATA_IN_ExtSRAM */ 
/* #define DATA_IN_ExtSDRAM */

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field. 
                                   This value must be a multiple of 0x200. */
/******************************************************************************/

/**
  * @}
  */

/** @addtogroup STM32F7xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F7xx_System_Private_Variables
  * @{
  */

  /* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency 
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
  uint32_t SystemCoreClock = HCLK;
  const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
  const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

/**
  * @}
  */

/** @addtogroup STM32F7xx_System_Private_FunctionPrototypes
  * @{
  */
#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
  static void SystemInit_ExtMemCtl(void); 
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

/**
  * @}
  */

/** @addtogroup STM32F7xx_System_Private_Functions
  * @{
  */

/**********************************************************************************/
// 外设时钟设置
#ifndef APB1PRE                  // APB1分频系数
#error "Macro APB1PRE is not set."
#elif (APB1PRE == 0 || APB1PRE == 1)
#define apb1_pre        0        // 不分频
#elif (APB1PRE == 2)
#define apb1_pre        4        // 2分频
#elif (APB1PRE == 4)
#define apb1_pre        5        // 4分频
#elif (APB1PRE == 8)
#define apb1_pre        6        // 8分频
#elif (APB1PRE == 16)
#define apb1_pre        7        // 16分频
#else
#error "The value Macro APB1PRE is or invalid."
#endif

#ifndef APB2PRE                 // APB2分频系数
#error "Macro APB2PRE is not set."
#elif (APB2PRE == 0 || APB2PRE == 1)
#define apb2_pre        0        // 不分频
#elif (APB2PRE == 2)
#define apb2_pre        4        // 2分频
#elif (APB2PRE == 4)
#define apb2_pre        5        // 4分频
#elif (APB2PRE == 8)
#define apb2_pre        6        // 8分频
#elif (APB2PRE == 16)
#define apb2_pre        7        // 16分频
#else
#error "The value of Macro APB2PRE is invalid."
#endif

static void RCC_CRClockInit(uint32_t Clk_Mask) {
  RCC->CR |= Clk_Mask;
  Clk_Mask <<= 1;
  if (!RCC_CR_CSSON_Msk)
  while (!(RCC->CR & Clk_Mask))
    continue;
}

static void EnablePLLAsSysClock(void)
{
/* 函数名: 设置系统时钟为PLL */
/* 隐含参数:HCLK (HCLK为一个定义在sysinit.h中的宏) */
/* HCLK为欲设置的主PLL频率，单位为Hz */
        RCC->CR  = 0x00000001;
        RCC->CIR = 0x00000000; /* 禁止RCC时钟中断 */

/* 设置系统驱动性能 */
#ifndef HCLK
#error "System frequency(HCLK) should be set."
#endif
#if (HCLK == 180000000)
        PWR->CR1 |= 0x0000C000;   /* 高性能模式,时钟可达到180MHz */
#elif (HCLK > 180000000 && HCLK <= 216000000)
        PWR->CR1 |= 0x0003C000;   /* 使能过驱动,可将频率达到216MHz */
#elif (HCLK > 216000000)
#error "System frequency is much too high to use."
#endif
/* 使能外部高速时钟HSE */
#ifndef HSE_VALUE
#error "Frequency of external crystal is unkown!"
#endif
#ifdef HSE_USE_CRYSTAL          /* HSE使用外部晶振作为时钟发生源 */
        RCC->CR &= 0xFFFBFFFF;  /* 外部晶振不旁路 */
#else                           /* HSE使用外部信号源(非晶振) */
        RCC->CR |= 0x00040000;  /* HSE旁路 */
#endif
/* Enable HSE */
        RCC_CRClockInit(RCC_CR_HSEON_Msk);
/* 将配置后的PLL参数写入 */
        RCC->PLLCFGR = 0x00400000 | PLLM_VALUE | (PLLN_VALUE << 6) | (PLLP_VALUE << 16) | (PLLQ_VALUE << 24);
/* 各个时钟源初始化 */
/* EnableCSS */
        RCC_CRClockInit(RCC_CR_CSSON_Msk);
/* EnablePLL */
        RCC_CRClockInit(RCC_CR_PLLON_Msk);
/* Enable PLLSAI */
#if (PLLSAI_ENABLE == 1U)
        RCC->PLLSAICFGR = (PLLSAIN_VALUE << 6) | (PLLSAIP_VALUE << 16) |
            (PLLSAIQ_VALUE << 24) | (PLLSAIR_VALUE << 28);
#if (RCC_DKCFGR1_TIMPRE == 1U)
        RCC->DCKCFGR1 = 0x01000000 | (SAI2SEL_VALUE << 22) | (SAI1SEL_VALUE << 20) | (PLLSAIDIVR_VALUE << 16) | (PLLSAIDIVQ_VALUE << 8);
#else
        RCC->DCKCFGR1 = (PLLSAIDIVR_VALUE << 16) | (SAI2SEL_VALUE << 22) | (SAI1SEL_VALUE << 20) | (PLLSAIDIVQ_VALUE << 8);
#endif
        RCC_CRClockInit(RCC_CR_PLLSAION_Msk);
#endif
/* Enable PLLI2S */
        RCC_CRClockInit(RCC_CR_PLLI2SON_Msk);
/* 设置FLASH */
/* 指令预取、ART加速器、数据cache使能,(HCLK/30)个CPU等待周期. */
        FLASH->ACR = 0x00000300 | ((HCLK/1000000) / 30);
/* 设置APBx预分频系数(APB2:15-13,APB1:12-10) */
        RCC->CFGR |= (apb1_pre << 10) | (apb2_pre << 13) | 0x00000002;
        while((RCC->CFGR & 0x0000000C) != 0x00000008)
                continue;
        RCC->CR &= 0xFFFFFFFE; /* 关闭HSI */
}

/*********************************************************************************/
/**
  * @brief  Setup the microcontroller system
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemFrequency variable.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  SCB_EnableICache();
  SCB_EnableDCache();
  SCB->CACR |= 0x00000004;
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
/* 使能硬件浮点运算单元 */
  SCB->CPACR |= 0x00F00000; // 使协处理器CP10，CP11能够全访问
#endif
/* 如果GPIO输出速度高于50MHz，应当设置I/O补偿单元，
 * 只有电源电压范围为2.4到3.6V时,才可以使用I/O补偿单元
 */
  SYSCFG->CMPCR = 0x00000001;
/* 使用主PLL作为系统时钟 */
  EnablePLLAsSysClock(); // 将设置系统频率为HCLK,单位MHz
#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
  SystemInit_ExtMemCtl(); 
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

/* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM1_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif

/* 设置对齐访问 */
#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR &= 0x000001FF;// CCR[9]=0:设置栈按照4字节对齐 CCR[9]=1:按照8字节对齐(默认)
  SCB->CCR |= 0x00000008;// CCR[3]=1:不允许非对齐访问
#endif
}

/**
   * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**) 
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *         
  *         (*) HSI_VALUE is a constant defined in stm32f7xx.h file (default value
  *             16 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.   
  *    
  *         (**) HSE_VALUE is a constant defined in stm32f7xx.h file (default value
  *              25 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *                
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *     
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
  
  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock source */

      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
         SYSCLK = PLL_VCO / PLL_P
         */    
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
      
      if (pllsource != 0)
      {
        /* HSE used as PLL clock source */
        pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }
      else
      {
        /* HSI used as PLL clock source */
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
      }

      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
      SystemCoreClock = pllvco/pllp;
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
  /* Compute HCLK frequency --------------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK frequency */
  SystemCoreClock >>= tmp;
}

#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
/**
  * @brief  Setup the external memory controller.
  *         Called in startup_stm32f7xx.s before jump to main.
  *         This function configures the external memories (SRAM/SDRAM)
  *         This SRAM/SDRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
  __IO uint32_t tmp = 0;
#if defined (DATA_IN_ExtSDRAM) && defined (DATA_IN_ExtSRAM)
  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register uint32_t index;

  /* Enable GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface 
      clock */
  RCC->AHB1ENR |= 0x000001F8;

  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);

  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x00CCC0CC;
  GPIOD->AFR[1]  = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xAAAA0A8A;

  /* Configure PDx pins speed to 100 MHz */  
  GPIOD->OSPEEDR = 0xFFFF0FCF;  
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */ 
  GPIOD->PUPDR   = 0x55550545;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00CC0CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAA828A;
  /* Configure PEx pins speed to 50 MHz */ 
  GPIOE->OSPEEDR = 0xFFFFC3CF;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */ 
  GPIOE->PUPDR   = 0x55554145;  

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCCC000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAA800AAA;
  /* Configure PFx pins speed to 50 MHz */ 
  GPIOF->OSPEEDR = 0xFF800FFF;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */ 
  GPIOF->PUPDR   = 0x55400555;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CC00CC;
  GPIOG->AFR[1]  = 0xC00000CC;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0x80220AAA;
  /* Configure PGx pins speed to 50 MHz */ 
  GPIOG->OSPEEDR = 0x80320FFF;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */ 
  GPIOG->PUPDR   = 0x40110555;

  /* Connect PHx pins to FMC Alternate function */
  GPIOH->AFR[0]  = 0x00C0CC00;
  GPIOH->AFR[1]  = 0xCCCCCCCC;
  /* Configure PHx pins in Alternate function mode */ 
  GPIOH->MODER   = 0xAAAA08A0;
  /* Configure PHx pins speed to 50 MHz */ 
  GPIOH->OSPEEDR = 0xAAAA08A0;
  /* Configure PHx pins Output type to push-pull */  
  GPIOH->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PHx pins */ 
  GPIOH->PUPDR   = 0x55550450;

  /* Connect PIx pins to FMC Alternate function */
  GPIOI->AFR[0]  = 0xCCCCCCCC;
  GPIOI->AFR[1]  = 0x00000CC0;
  /* Configure PIx pins in Alternate function mode */ 
  GPIOI->MODER   = 0x0028AAAA;
  /* Configure PIx pins speed to 50 MHz */ 
  GPIOI->OSPEEDR = 0x0028AAAA;
  /* Configure PIx pins Output type to push-pull */  
  GPIOI->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PIx pins */ 
  GPIOI->PUPDR   = 0x00145555;

/*-- FMC Configuration ------------------------------------------------------*/
  /* Enable the FMC interface clock */
  RCC->AHB3ENR |= 0x00000001;
  
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1->BTCR[4]  = 0x00001091;
  FMC_Bank1->BTCR[5]  = 0x00110212;
  FMC_Bank1E->BWTR[4] = 0x0FFFFFFF;

  /* Configure and enable SDRAM bank1 */
  FMC_Bank5_6->SDCR[0] = 0x000019E5;
  FMC_Bank5_6->SDTR[0] = 0x01116361;   

  /* SDRAM initialization sequence */
  /* Clock enable command */
  FMC_Bank5_6->SDCMR = 0x00000011; 
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }

  /* Delay */
  for (index = 0; index<1000; index++);
  
  /* PALL command */
  FMC_Bank5_6->SDCMR = 0x00000012;           
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
  
  /* Auto refresh command */
  FMC_Bank5_6->SDCMR = 0x000000F3;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
 
  /* MRD register program */
  FMC_Bank5_6->SDCMR = 0x00046014;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  } 
  
  /* Set refresh count */
  tmpreg = FMC_Bank5_6->SDRTR;
  FMC_Bank5_6->SDRTR = (tmpreg | (0x00000603<<1));
  
  /* Disable write protection */
  tmpreg = FMC_Bank5_6->SDCR[0]; 
  FMC_Bank5_6->SDCR[0] = (tmpreg & 0xFFFFFDFF);  
  
#elif defined (DATA_IN_ExtSDRAM)
  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register uint32_t index;

  /* Enable GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface 
      clock */
  RCC->AHB1ENR |= 0x000001F8;
  
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);
  
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x000000CC;
  GPIOD->AFR[1]  = 0xCC000CCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xA02A000A;
  /* Configure PDx pins speed to 50 MHz */  
  GPIOD->OSPEEDR = 0xA02A000A;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */ 
  GPIOD->PUPDR   = 0x50150005;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00000CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAA800A;
  /* Configure PEx pins speed to 50 MHz */ 
  GPIOE->OSPEEDR = 0xAAAA800A;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */ 
  GPIOE->PUPDR   = 0x55554005;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCCC000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAA800AAA;
  /* Configure PFx pins speed to 50 MHz */ 
  GPIOF->OSPEEDR = 0xAA800AAA;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */ 
  GPIOF->PUPDR   = 0x55400555;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CC00CC;
  GPIOG->AFR[1]  = 0xC000000C;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0x80020A0A;
  /* Configure PGx pins speed to 50 MHz */ 
  GPIOG->OSPEEDR = 0x80020A0A;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */ 
  GPIOG->PUPDR   = 0x40010505;
  
  /* Connect PHx pins to FMC Alternate function */
  GPIOH->AFR[0]  = 0x00C0CC00;
  GPIOH->AFR[1]  = 0xCCCCCCCC;
  /* Configure PHx pins in Alternate function mode */ 
  GPIOH->MODER   = 0xAAAA08A0;
  /* Configure PHx pins speed to 50 MHz */ 
  GPIOH->OSPEEDR = 0xAAAA08A0;
  /* Configure PHx pins Output type to push-pull */  
  GPIOH->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PHx pins */ 
  GPIOH->PUPDR   = 0x55550450;
  
  /* Connect PIx pins to FMC Alternate function */
  GPIOI->AFR[0]  = 0xCCCCCCCC;
  GPIOI->AFR[1]  = 0x00000CC0;
  /* Configure PIx pins in Alternate function mode */ 
  GPIOI->MODER   = 0x0028AAAA;
  /* Configure PIx pins speed to 50 MHz */ 
  GPIOI->OSPEEDR = 0x0028AAAA;
  /* Configure PIx pins Output type to push-pull */  
  GPIOI->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PIx pins */ 
  GPIOI->PUPDR   = 0x00145555;
  
/*-- FMC Configuration ------------------------------------------------------*/
  /* Enable the FMC interface clock */
  RCC->AHB3ENR |= 0x00000001;
  
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  /* Configure and enable SDRAM bank1 */
  FMC_Bank5_6->SDCR[0] = 0x000019E5;
  FMC_Bank5_6->SDTR[0] = 0x01116361;      
  
  /* SDRAM initialization sequence */
  /* Clock enable command */
  FMC_Bank5_6->SDCMR = 0x00000011; 
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }

  /* Delay */
  for (index = 0; index<1000; index++);
  
  /* PALL command */
  FMC_Bank5_6->SDCMR = 0x00000012;           
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
  
  /* Auto refresh command */
  FMC_Bank5_6->SDCMR = 0x000000F3;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
 
  /* MRD register program */
  FMC_Bank5_6->SDCMR = 0x00046014;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  } 
  
  /* Set refresh count */
  tmpreg = FMC_Bank5_6->SDRTR;
  FMC_Bank5_6->SDRTR = (tmpreg | (0x00000603<<1));
  
  /* Disable write protection */
  tmpreg = FMC_Bank5_6->SDCR[0]; 
  FMC_Bank5_6->SDCR[0] = (tmpreg & 0xFFFFFDFF);
  
#elif defined(DATA_IN_ExtSRAM)
/*-- GPIOs Configuration -----------------------------------------------------*/
   /* Enable GPIOD, GPIOE, GPIOF and GPIOG interface clock */
  RCC->AHB1ENR   |= 0x00000078;
  
    /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);
  
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x00CCC0CC;
  GPIOD->AFR[1]  = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xAAAA0A8A;
  /* Configure PDx pins speed to 100 MHz */  
  GPIOD->OSPEEDR = 0xFFFF0FCF;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */ 
  GPIOD->PUPDR   = 0x55550545;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00CC0CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAA828A;
  /* Configure PEx pins speed to 100 MHz */ 
  GPIOE->OSPEEDR = 0xFFFFC3CF;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */ 
  GPIOE->PUPDR   = 0x55554145;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCC0000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAA000AAA;
  /* Configure PFx pins speed to 100 MHz */ 
  GPIOF->OSPEEDR = 0xFF000FFF;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */ 
  GPIOF->PUPDR   = 0x55000555;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CCCCCC;
  GPIOG->AFR[1]  = 0x000000C0;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0x00200AAA;
  /* Configure PGx pins speed to 100 MHz */ 
  GPIOG->OSPEEDR = 0x00300FFF;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */ 
  GPIOG->PUPDR   = 0x00100555;
  
/*-- FMC/FSMC Configuration --------------------------------------------------*/                                                                               
  /* Enable the FMC/FSMC interface clock */
  RCC->AHB3ENR         |= 0x00000001;
  
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1->BTCR[4]  = 0x00001091;
  FMC_Bank1->BTCR[5]  = 0x00110212;
  FMC_Bank1E->BWTR[4] = 0x0FFFFFFF;  

#endif /* DATA_IN_ExtSRAM */

  /*
   * Disable the FMC bank1 (enabled after reset).
   * This, prevents CPU speculation access on this bank which blocks the use of FMC during
   * 24us. During this time the others FMC master (such as LTDC) cannot use it!
   */
  FMC_Bank1->BTCR[0] = 0x000030d2;
  
  (void)(tmp);
}
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

/**
  * @}
  */

/**
  * @}
  */
  
/**
  * @}
  */    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
