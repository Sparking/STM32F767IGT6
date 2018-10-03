#include <stdlib.h>
#include "digital_filter.h"

static int _ucompare(const void *a, const void *b)
{
        return *(unsigned int *)a - *(unsigned int *)b;
}

void _filter_a(unsigned int buff[], unsigned int size, unsigned int *i,
        unsigned int *j)
{
        qsort(buff, size, sizeof(int), (void *)_ucompare);
        for (*i = 0; *i < size && buff[*i] == 0; (*i)++)
                continue; /* 滤除0 */
        for (*j = *i; *j < size && 0xFFFFU - buff[*j] >= 5243; (*j)++)
                continue;
}

unsigned int filter_unsigned_median(unsigned int *buff, unsigned int size)
{
        unsigned int i, j;
        _filter_a(buff, size, &i, &j);
        return buff[i + (size - j) / 2];
}

unsigned int filter_unsigned_avr(unsigned int *buff, unsigned int size)
{
        unsigned int i, j, index, sum;

        _filter_a(buff, size, &i, &j);
        index = i;
        for (sum = 0, i = 0; index < j; index++) {
                sum += buff[index++];
                i++;
        }
        return sum /i;
}

unsigned int filter_unsigned_median_with_avr(unsigned int *buff,
                unsigned int size)
{
        unsigned int i, sum;
        qsort(buff, size, sizeof(int), (void *)_ucompare);
        for (i = 10, sum = 0; i < size - 10; i++) {
                sum += buff[i];
        }
        return sum / (i - 10);
}

unsigned int filter_unsigned_limit(unsigned int prev_value,
                unsigned int current_value, unsigned int limit_value)
{
        if (abs(current_value - prev_value) >= limit_value)
                current_value = prev_value;
        return current_value;
}

unsigned int filter_unsigned_elimination(unsigned int *buff, unsigned int size,
                unsigned int prev_result)
{
        static unsigned char count = 0;
        unsigned int current_result = filter_unsigned_median_with_avr(buff, size);
        if (current_result == prev_result)
                count = 0;
        else
                count = (count + 1) % 4;
        if (count != 0)
                current_result = prev_result;
        return current_result;
}
