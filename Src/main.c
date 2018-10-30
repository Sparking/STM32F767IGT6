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
        UART_Init();
        printf("UART Initialized successed!\n");
        SDRAM_Init();
        LCD_Init();
        LCD_Touch_Init();
        LCD_Clear(LCD_BG_COLOR);
        Key_Init();
        LED_Init();
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_SET);
        RTC_ConfigTime(&time, 9, 6, 50, 1);
        RTC_ConfigDate(&date, 18, 4, 25, 3);
        RTC_Init(&time, &date);
        RTC_WeakUp_Enable(WKUP_CLK_PRE_16, 2048); /* 1秒1次唤醒中断 */
        IIC_Interface_Init(&i2c_int1);
        AT24CXX_Init(&at24c02_dev, &i2c_int1, 0);
        while (!AT24CXX_Check(&at24c02_dev)) {
                printf("24C02 Check Failed!\n");
                printf("Please Check!      \n");
                delayms(500);
        }
        printf("24C02 Ready!\n");
        TIM3_PWM_Init(TIM3_FREQ, TIM3_PWM2_PERIOD, 0);
        AT24CXX_ReadByte(&at24c02_dev, 0, &i);
        printf("The last set value is %u.\n", i);
        TIM3->CCR2 = 0;

//        IWDG_Init();
        RTC_WKUP_IRQHandler();
}

static void exec_input(char *buff, size_t len)
{
    int ret;
    unsigned int index;
    unsigned int read_size;
    static int tab_flag = 0;

    index = 0;
    cli_printf("%s", cli_prompt);
    if (tab_flag) {
        index = strlen(buff);
        cli_printf("%s", buff);
    }
    fflush(stdout);
    tab_flag = 0;
    while (index < len) {
        read_size = USART1ReveiveStr(buff + index, 1);
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

static void trave_pcdb(struct command_data_block *pcdb, int i)
{
    if (pcdb) {
        for (int j = 0; j < i; j++) {
            putchar('-');
            putchar('-');
        }
        switch (pcdb->data_block_type) {
        case COMMAND_DATA_BLOCK_TYPE_COMMAND:
            cli_printf("%s\n", pcdb->data_block.command);
            break;
        default:
            cli_printf("WORD\n");
            break;
        }
        trave_pcdb(pcdb->next, i + 1);
        trave_pcdb(pcdb->alt, i);
    }
}

static void exec_help(struct command_data_block *pcdb)
{
    struct command_data_block *first = cli_first_command_data_block();

    trave_pcdb(first, 0);
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
}

static void exec_show_specfial_log(struct command_data_block *pcdb)
{
    cli_printf("%.*s\n", pcdb->data_block.string.size, pcdb->data_block.string.string);
}

static void exec_at24c02_show(struct command_data_block *pcdb)
{
    unsigned char i;
    AT24CXX_ReadByte(&at24c02_dev, 1, &i);
    cli_printf("%d\n", i);
}

static void exec_at24c02_set_value(struct command_data_block *pcdb)
{
    int ret;
    int i;
    char *tmp;

    tmp = (char *)malloc(pcdb->data_block.string.size + 1);
    if (tmp == NULL) {
        cli_printf("error: has no memeory\n");
        return;
    }

    strncpy(tmp, pcdb->data_block.string.string, pcdb->data_block.string.size + 1);
    ret = sscanf(tmp, "%d", &i);
    free(tmp);
    if (ret != 1) {
        cli_printf("error: wrong format data\n");
        return;
    } else {
        i &= 0xFFU;
        cli_printf("set value to %d\n", i);
    }

    AT24CXX_WriteByte(&at24c02_dev, 1, (char)i);
}

static void exec_clear(struct command_data_block *pcdb)
{
    cli_printf("\f");
}

static int quit_flag = 0;

static void cli_init(void)
{
    struct command_data_block *pcdb[5];

    pcdb[0] = cli_regist_command("show", "show info", NULL, NULL, NULL);
    pcdb[1] = cli_regist_command("version", "show version", NULL, pcdb[0],
            exec_show_version);
    pcdb[2] = cli_regist_command("log", "show log", NULL, pcdb[0], NULL);
    pcdb[3] = cli_regist_command("at24c02", "operations for at24c02", NULL, NULL, NULL);
    pcdb[4] = cli_regist_command("set", "set at24c02 first byte", NULL, pcdb[3], NULL);
    (void)cli_regist_command("detail", "show version detail", NULL, pcdb[1], exec_show_version_detail);
    (void)cli_regist_string("show detail log", NULL, pcdb[2], exec_show_specfial_log);
    (void)cli_regist_command("show_lcd", "show lcd", NULL, NULL, exec_show_lcd);
    (void)cli_regist_command("clear", "clean screen", NULL, NULL, exec_clear);
    (void)cli_regist_command("show", "show at24c02 info", NULL, pcdb[3], exec_at24c02_show);
    (void)cli_regist_string("an integer", NULL, pcdb[4], exec_at24c02_set_value);
    (void)cli_regist_command("quit", "quit cli, be careful to use", (void *)&quit_flag, NULL, exec_quit);
    (void)cli_regist_command("help", "show cli tree", NULL, NULL, exec_help);
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

