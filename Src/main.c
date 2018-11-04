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

#define TIM3_FREQ         1000    /* 定时器的时钟频率设置为1KHz */
#define TIM3_PWM2_PERIOD  500     /* 定时器的计数周期设置位500，即一个周期500ms */
extern void RTC_WKUP_IRQHandler(void);

/* IIC设备接口1 */
i2c_interface_t i2c_int1;
i2c_device_t at24c02_dev;
const char *const String = "XXC Test.\n";

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
    unsigned char i;
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    /* IIC 设备初始化 */
    i2c_int1.SCL.GPIO = GPIOH;
    i2c_int1.SCL.Pin  = GPIO_PIN_4;
    i2c_int1.SDA.GPIO = GPIOH;
    i2c_int1.SDA.Pin  = GPIO_PIN_5;

    HAL_Init();
    SDRAM_Init();
    LCD_Init();
    LCD_Touch_Init();
    LCD_Clear(LCD_BG_COLOR);
    Key_Init();
    LED_Init();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET);
    RTC_ConfigTime(&time, 9, 6, 50, 1);
    RTC_ConfigDate(&date, 18, 4, 25, 3);
    RTC_Init(&time, &date);
    RTC_WeakUp_Enable(WKUP_CLK_PRE_16, 2048); /* 1秒1次唤醒中断 */
    RTC_WKUP_IRQHandler();
    IIC_Interface_Init(&i2c_int1);
    UART_Init();
    printf("UART Initialized successed!\n");
    AT24CXX_Init(&at24c02_dev, &i2c_int1, 0);
    while (!AT24CXX_Check(&at24c02_dev)) {
        printf("24C02 Check Failed!\n");
        printf("Please Check!      \n");
        delayms(500);
    }
    printf("24C02 Ready!\n");
    //TIM3_PWM_Init(TIM3_FREQ, TIM3_PWM2_PERIOD, 0);
    AT24CXX_ReadByte(&at24c02_dev, 0, &i);
    printf("The last set value is %u.\n", i);
    TIM3->CCR2 = 0;

    //IWDG_Init();
}

void exec_input(char *buff, size_t len)
{
    int ret;
    unsigned int index;
    unsigned int read_size;
    static char tab_flag = 0;

    index = 0;
    cli_printf("%s", cli_prompt);
    if (tab_flag) {
        index = strlen(buff);
        cli_printf("%s", buff);
    }
    fflush(stdout);
    tab_flag = 0;
    while (index < len) {
        read_size = USARTReveiveStr(buff + index, 1);
        if (buff[index] == '\r' || buff[index] == '\n') {
            break;
        } else if (buff[index] == '\t' || buff[index] == '?') {
            index++;
            tab_flag = 1;
            break;
        } else if (buff[index] < 127 && buff[index] > 31 && read_size == 1) {
            index++;
        } else if (buff[index] == 127) {
            if (index == 0) {
                continue;
            }
            index--;
            fflush(stdout);
        }
    }

    buff[index] = '\0';
    if ((ret = cli_exec(buff, len)) < 0) {
        cli_printf("unknow command!\n");
    }
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
            cli_printf("%s", pcdb->data_block.command);
            break;
        default:
            cli_printf("WORD");
            break;
        }
        if (flag) {
            cli_printf(": %s", pcdb->desc);
        }
        if (pcdb->exec) {
            cli_printf("*");
        }
        cli_printf("\n");
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
    cli_printf("STM32F767IGT6\n");
}

static void exec_quit(struct command_data_block *pcdb)
{
    int *quit_flag;

    release_cli_tree();
    quit_flag = (int *)pcdb->private_data;
    if (quit_flag) {
        *quit_flag = 1;
    }
    cli_printf("the cli tree is released, and cli is not available\n");
}

static void exec_show_version_detail(struct command_data_block *pcdb)
{
    cli_printf("STM32F767IGT6\n");
    cli_printf("Memory: 512K\n");
    cli_printf("Flash:  1024KB\n");
    cli_printf("UART: 0x%X\n", (unsigned int)STD_UART_CONSOLE_0);
}

static void exec_show_specfial_log(struct command_data_block *pcdb)
{
    cli_printf("%.*s\n", pcdb->data_block.string.size, pcdb->data_block.string.string);
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
        cli_printf("address is invalid\n");
        return;
    }

    AT24CXX_ReadByte(&at24c02_dev, (unsigned short)address, &i);
    cli_printf("%d\n", i);
}

static void exec_at24c02_set_value(struct command_data_block *pcdb)
{
    int ret;
    int address;
    int value;
    char tmp[4];

    if (((struct cli_string_block *)pcdb->private_data)->size > 4) {
        cli_printf("address is invalid\n");
        return;
    }
    memcpy(tmp, ((struct cli_string_block *)pcdb->private_data)->string,
        ((struct cli_string_block *)pcdb->private_data)->size);
    tmp[((struct cli_string_block *)pcdb->private_data)->size] = '\0';
    ret = sscanf(tmp, "%d", &address);
    if (ret != 1 || address < 0 || address > 255) {
        cli_printf("error: wrong format address!\n");
        return;
    }

    if (pcdb->data_block.string.size > sizeof(tmp)) {
        cli_printf("value is invalid\n");
        return;
    }
    memcpy(tmp, pcdb->data_block.string.string, pcdb->data_block.string.size);
    tmp[pcdb->data_block.string.size] = '\0';
    ret = sscanf(tmp, "%d", &value);
    if (ret != 1) {
        cli_printf("error: wrong format value!\n");
        return;
    } else {
        value &= 0xFFU;
        cli_printf("set value to %d\n", value);
    }

    AT24CXX_WriteByte(&at24c02_dev, (unsigned short)address, (unsigned char)value);
}

static void exec_clear(struct command_data_block *pcdb)
{
    cli_printf("\f");
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
        cli_printf("unknow command!\n");
        return;
    }

    cli_str_blk = (struct cli_string_block *)pcdb->private_data;
    if (cli_string_is(cli_str_blk, "green", 1)) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, status);;
    } else if (cli_string_is(cli_str_blk, "red", 1)) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, status);;
    } else {
        cli_printf("unknow command!\n");
        return;
    }
}

static int quit_flag = 0;

static void cli_init(void)
{
    struct command_data_block *pcdb[10];

    pcdb[0] = cli_regist_command("show", "show info", NULL, NULL, NULL);
    pcdb[1] = cli_regist_command("version", "show version", NULL, pcdb[0],
            exec_show_version);
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
    (void)cli_regist_command("quit", "quit cli, be careful to use", (void *)&quit_flag, NULL, exec_quit);
    (void)cli_regist_command("detail", "show cli tree with help", (void *)1, pcdb[7], exec_help);
}

int main(void)
{
    char cmdline[1024];

    main_init();
    cli_init();

    while (!quit_flag) {
        exec_input(cmdline, 1023);
        fflush(stdout);
    }
}

