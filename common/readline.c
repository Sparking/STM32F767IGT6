#include <string.h>
#include "readline.h"

/**
 * read_first_word 读取字符串中的第一个单词
 *
 * @param:
 *    line: 字符串
 *    size: 单词的大小
 *    delm: 单词分割符
 *
 * @retval:
 *    成功返回单词的地址,失败返回NULL
 */
char *read_first_word(char *__restrict line, int *__restrict size,
    const char *__restrict delm)
{
    int i;
    char *this_word;

    this_word = NULL;
    for (i = 0; line[i] != '\0'; i++) {
        if (strchr(delm, line[i]) == NULL) {
            this_word = line + i;
            while (strchr(delm, line[i]) == NULL && line[i] != '\0'
                && line[i] != '\r' && line[i] != '\n') {
                i++;
            }
            *size = line + i - this_word;
            break;
        }
    }

    return this_word;
}

