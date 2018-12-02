/**
  ******************************************************************************
  * @file    main.c
  * @author  Nassura
  * @version V1.0
  * @date    2017-12-9
  * @brief   Default main function.
  ******************************************************************************
*/

#include "main.h"

#define TIM3_FREQ     1000    /* 定时器的时钟频率设置为1KHz */
#define TIM3_PWM2_PERIOD  500     /* 定时器的计数周期设置位500，即一个周期500ms */
extern void RTC_WKUP_IRQHandler(void);

/* IIC设备接口1 */
i2c_interface_t i2c_int1;
i2c_device_t at24c02_dev;
const char *const String = "XXC Test.\r\n";

/*
 * 将字符串全部填为空格符
 */
static void Fill_Str(char *str, int n)
{
    int len = strlen(str);
    while (len < n - 1)
        str[len++] = ' ';
    str[len] = '\0';
}

static void exec_show_lcd(struct command_data_block *pcdb)
{
    char str[29];
    LCD_CharConfigTypeDef LCD_String;

    /* LCD显示字符串 */
    LCD_String.FontConfig.Font = &ASCII_Font[1]; /* 要显示的字体 */
    /* 字体是否需要其他颜色衬托(是否需要背景色) */
    LCD_String.FontConfig.Mode = FONT_DISPLAY_WITH_BACK_COLOR;
    /* 背景色设置为白色 */
    LCD_String.FontConfig.BackColor = LCD_BG_COLOR;
    LCD_String.PointConfig.Color = 0x0000U; /* 字符串的颜色设置为黑色 */
    LCD_String.PointConfig.Layer = LCD_LAYER_1; /* 在第一层显示 */
    LCD_String.PointConfig.Position.X = 0; /* 字符串开始地址X轴 */
    LCD_String.PointConfig.Position.Y = 0; /* 字符串开始地址Y轴 */
    /* 竖屏显示 */
    LCD_String.PointConfig.Direction = LCD_DIRECTION_V;
    sprintf(str, "Current Temperature: NULL");
    Fill_Str(str, 29);
    LCD_String.PointConfig.Position.X = 0;
    LCD_String.PointConfig.Position.Y = 32;
    LCD_Puts(&LCD_String, str);
    AT24CXX_ReadByte(&at24c02_dev, 0, (unsigned char *)str);
    sprintf(str, "Target Temperature: %d", (int)str[0]);
    Fill_Str(str, 29);
    LCD_String.PointConfig.Position.Y = 48;
    LCD_Puts(&LCD_String, str);
    AT24CXX_ReadByte(&at24c02_dev, 1, (unsigned char *)str);
    sprintf(str, "Heating Status:%s", str[0] == 1 ? "heating" : "stopped");
    Fill_Str(str, 29);
    LCD_String.PointConfig.Position.Y = 64;
    LCD_Puts(&LCD_String, str);
}

void EXTI15_10_IRQHandler(void)
{  
}

static void main_init(void)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    /* IIC 设备初始化 */
    i2c_int1.SCL.GPIO = GPIOH;
    i2c_int1.SCL.Pin  = GPIO_PIN_4;
    i2c_int1.SDA.GPIO = GPIOH;
    i2c_int1.SDA.Pin  = GPIO_PIN_5;

    HAL_Init();
    UART_Init();
    printf("UART Initialized success!\r\n");
    printf("Initialize SDRAM...");
    fflush(stdout);
    SDRAM_Init();
    printf("       \033[32m[OK]\033[0m\r\n");
    printf("Initialize LCD...");
    LCD_Init();
    printf("         \033[32m[OK]\033[0m\r\n");
    printf("Initialize LCD Touch...");
    fflush(stdout);
    LCD_Touch_Init();
    printf("   \033[32m[OK]\033[0m\r\n");
    LCD_Clear(LCD_BG_COLOR);
    printf("Initialize Key...");
    fflush(stdout);
    Key_Init();
    printf("         \033[32m[OK]\033[0m\r\n");
    printf("Initialize LED...");
    fflush(stdout);
    LED_Init();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET);
    printf("         \033[32m[OK]\033[0m\r\n");
    printf("Initialize RTC...");
    fflush(stdout);
    RTC_ConfigTime(&time, 9, 6, 50, 1);
    RTC_ConfigDate(&date, 18, 4, 25, 3);
    RTC_Init(&time, &date);
    RTC_WeakUp_Enable(WKUP_CLK_PRE_16, 2048); /* 1秒1次唤醒中断 */
    RTC_WKUP_IRQHandler();
    printf("         \033[32m[OK]\033[0m\r\n");
    printf("Initialize AT24C02...");
    fflush(stdout);
    IIC_Interface_Init(&i2c_int1);
    AT24CXX_Init(&at24c02_dev, &i2c_int1, 0);
    printf("     \033[32m[OK]\033[0m\r\n");
    printf("Initialize CLI...");
    fflush(stdout);
    cli_init();
    printf("         \033[32m[OK]\033[0m\r\n");
    printf("Welcome, Nassura!\r\n");

    /*
    printf("Initialize TIM3 PWM...");
    fflush(stdout);
    TIM3_PWM_Init(TIM3_FREQ, TIM3_PWM2_PERIOD, 0);
    TIM3->CCR2 = 0;
    printf("    [OK]\n");
    */

    //IWDG_Init();
}

static void trave_pcdb(struct command_data_block *pcdb, int i, int flag)
{
    if (pcdb) {
        cli_printf("  ");
        for (int j = 0; j < i; j++) {
            cli_printf("  ");
        }
        switch (pcdb->data_block_type) {
        case COMMAND_DATA_BLOCK_TYPE_COMMAND:
            cli_printf("\033[33m%s\033[0m", pcdb->data_block.command);
            break;
        default:
            cli_printf("\033[33mWORD\033[0m");
            break;
        }
        if (flag) {
            cli_printf(": %s", pcdb->desc);
        }
        if (pcdb->exec) {
            cli_printf("\033[31m*\033[0m");
        }
        cli_printf("\r\n");
        trave_pcdb(pcdb->next, i + 1, flag);
        trave_pcdb(pcdb->alt, i, flag);
    }
}

static void exec_help(struct command_data_block *pcdb)
{
    int flag = (int)pcdb->private_data;
    struct command_data_block *first = cli_first_command_data_block();

    trave_pcdb(first, 0, flag);
}

static void exec_show_version(struct command_data_block *pcdb)
{
    cli_printf("\033[7mSTM32F767IGT6\033[0m\r\n");
}

extern void Reset_Handler(void);
static void exec_reload(struct command_data_block *pcdb)
{
    release_cli_tree();
    cli_printf("\033[031mreboot the system...\033[0m\r\n");
    Reset_Handler();
}

static void exec_show_version_detail(struct command_data_block *pcdb)
{
    exec_show_version(NULL);
    cli_printf("\033[7mMemory\033[0m: \033[1m512K\033[0m\r\n");
    cli_printf("\033[7mFlash\033[0m: \033[1m1024KB\033[0m\r\n");
    cli_printf("\033[7mUART\033[0m: \033[1m0x%X\033[0m\r\n", (unsigned int)STD_UART_CONSOLE_0);
}

static void exec_show_specfial_log(struct command_data_block *pcdb)
{
    cli_printf("%.*s\r\n", pcdb->data_block.string.size, pcdb->data_block.string.string);
}

static void exec_at24c02_show(struct command_data_block *pcdb)
{
    unsigned char i;
    int address;
    char tmp[4];

    if (pcdb->data_block.string.size > sizeof(tmp)) {
        goto invalid_address_out;
    }
    memcpy(tmp, pcdb->data_block.string.string, pcdb->data_block.string.size);
    tmp[pcdb->data_block.string.size] = '\0';
    if (sscanf(tmp, "%d", &address) != 1 || (address < 0 || address > 255)) {
invalid_address_out:
        cli_printf("address is invalid\r\n");
        return;
    }

    AT24CXX_ReadByte(&at24c02_dev, (unsigned short)address, &i);
    cli_printf("%d\r\n", i);
}

static void exec_at24c02_set_value(struct command_data_block *pcdb)
{
    int ret;
    int address;
    int value;
    char tmp[4];

    if (((struct cli_string_block *)pcdb->private_data)->size > 4) {
        cli_printf("address is invalid\r\n");
        return;
    }
    memcpy(tmp, ((struct cli_string_block *)pcdb->private_data)->string,
    ((struct cli_string_block *)pcdb->private_data)->size);
    tmp[((struct cli_string_block *)pcdb->private_data)->size] = '\0';
    ret = sscanf(tmp, "%d", &address);
    if (ret != 1 || address < 0 || address > 255) {
        cli_printf("error: wrong format address!\r\n");
        return;
    }

    if (pcdb->data_block.string.size > sizeof(tmp)) {
        cli_printf("value is invalid\r\n");
        return;
    }
    memcpy(tmp, pcdb->data_block.string.string, pcdb->data_block.string.size);
    tmp[pcdb->data_block.string.size] = '\0';
    ret = sscanf(tmp, "%d", &value);
    if (ret != 1) {
        cli_printf("error: wrong format value!\r\n");
        return;
    } else {
        value &= 0xFFU;
        cli_printf("set value to %d\r\n", value);
    }

    AT24CXX_WriteByte(&at24c02_dev, (unsigned short)address, (unsigned char)value);
}

static void exec_clear(struct command_data_block *pcdb)
{
    cli_printf("\033[2J\f");
}

static void exec_control_led(struct command_data_block *pcdb)
{
    struct cli_string_block *cli_str_blk;
    GPIO_PinState status;

    if (cli_string_is(&pcdb->data_block.string, "on", 1)) {
        status = GPIO_PIN_RESET;
    } else if (cli_string_is(&pcdb->data_block.string, "off", 1)) {
        status = GPIO_PIN_SET;
    } else {
        goto unknow_out;
    }

    cli_str_blk = (struct cli_string_block *)pcdb->private_data;
    if (cli_string_is(cli_str_blk, "green", 1)) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, status);;
    } else if (cli_string_is(cli_str_blk, "red", 1)) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, status);;
    } else {
unknow_out:
        cli_printf("unknow command!\r\n");
    }
}

static void cli_tree_init(void)
{
    struct command_data_block *pcdb[10];

    pcdb[0] = cli_regist_command("show", "show info", NULL, NULL, NULL);
    pcdb[1] = cli_regist_command("version", "show version", NULL, pcdb[0], exec_show_version);
    pcdb[2] = cli_regist_command("log", "show log", NULL, pcdb[0], NULL);
    pcdb[3] = cli_regist_command("at24c02", "operations for at24c02", NULL, NULL, NULL);
    pcdb[4] = cli_regist_string("address [0-255]", NULL, pcdb[3], exec_at24c02_show);
    pcdb[5] = cli_regist_command("led", "control led on or off", NULL, NULL, NULL);
    pcdb[6] = cli_regist_string("green | red", NULL, pcdb[5], NULL);
    pcdb[7] = cli_regist_command("cli", "show cli tree", NULL, pcdb[0], exec_help);
    pcdb[8] = cli_regist_command("set", "set at24c02 data", &pcdb[4]->data_block.string, pcdb[4], NULL);
    (void)cli_regist_string("on | off", &pcdb[6]->data_block.string, pcdb[6], exec_control_led);
    (void)cli_regist_command("detail", "show version detail", NULL, pcdb[1], exec_show_version_detail);
    (void)cli_regist_string("show detail log", NULL, pcdb[2], exec_show_specfial_log);
    (void)cli_regist_command("show_lcd", "show lcd", NULL, NULL, exec_show_lcd);
    (void)cli_regist_command("clear", "clean screen", NULL, NULL, exec_clear);
    (void)cli_regist_string("value", pcdb[8]->private_data, pcdb[8], exec_at24c02_set_value);
    (void)cli_regist_command("reboot", "reload the system", NULL, NULL, exec_reload);
    (void)cli_regist_command("detail", "show cli tree with help", (void *)1, pcdb[7], exec_help);
}

int main(void)
{
    char cmdline[1024];
    int tab_flag;

    main_init();
    cli_tree_init();

    tab_flag = 0;
    while (1) {
        cli_exec_input(cmdline, 1023, &tab_flag);
        fflush(stdout);
    }
}

