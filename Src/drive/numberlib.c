#include "number_lib.h"

uint32_t BCD2NUM(uint32_t bcd)
{
        short top = 0;
        uint8_t s[8];
        uint32_t temp;
        while (bcd != 0 && top < 8) {
                temp = bcd & 0x0000000F;
                s[top++] = temp;
                bcd >>= 4;
        }
        temp = 0;
        while (top > 0)
                temp = temp * 10 + s[--top];
        return temp;
}

uint32_t NUM2BCD(uint32_t num)
{
        short top = 0;
        uint8_t s[8];
        uint32_t temp;
        while (num != 0 && top < 8) {
                temp = num % 10;
                s[top++] = temp;
                num /= 10;
        }
        temp = 0;
        while (top > 0)
                temp = (temp << 4) + s[--top];
        return temp;
}

void Num2Str(uint32_t number, char *s, int n)
{
        int index, top = n - 1;
        char temp;
        s[top--] = '\0';
        while (number != 0 && top >= 0) {
                temp = number % 10;
                number /= 10;
                s[top--] = temp + '0';
        }
        if (top != 0) {
                index = 0;
                while (top < n)
                        s[index++] = s[++top];
        }
}

signed int Str2Num(char *string)
{
        signed int num;
        signed int sig;
        unsigned char i;

        i = 1;
        sig = 1;
        num = 0;
        if (string[0] == '-')
                sig = -1;
        else if (string[0] != '+')
                i = 0;
        while (string[i] >= '0' && string[i] <= '9')
                num = num * 10 + string[i++] - '0';
        return num * sig;
}
