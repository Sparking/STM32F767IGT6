#ifndef _NASSURA_CLI_H_
#define _NASSURA_CLI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define cli_printf(fmt,...) printf(fmt, ##__VA_ARGS__)
#define cli_malloc(size)    malloc(size)
#define cli_free(p)         free(p)

#define CLI_EXEC_NO_END     -1
#define CLI_EXEC_UNKNOW     -2
#define CLI_EXEC_UNKNOW_TAB -3
#define CLI_EXEC_FAIL       -4
#define CLI_EXEC_NONE       0
#define CLI_EXEC_READ_TAB   1
#define CLI_EXEC_SUCCESS    2

/* 关键字的数据类型 */
enum {
    COMMAND_DATA_BLOCK_TYPE_COMMAND = 0,    /* 命令 */
    COMMAND_DATA_BLOCK_TYPE_INTEGER,        /* 整数 */
    COMMAND_DATA_BLOCK_TYPE_STRING,         /* 单词 */
    COMMAND_DATA_BLOCK_TYPE_FLAGS,          /* 标识 */
};

/* 单词的数据格式 */
struct cli_string_block {
    char  *string;                          /* 单词的起始位置，处于命令行输入的字符串中 */
    size_t size;                            /* 单词的长度 */
};

/* 一个关键字的数据格式 */
struct command_data_block {
    int    data_block_type;                 /* 关键字的数据类型 */
    union {
        char  *command;                     /* 命令的名称 */
        size_t integer;                     /* 解析出来的整数 */
        size_t flags;                       /* 解析出来的标识 */
        struct cli_string_block string;     /* 解析出来的单词 */
    } data_block;
    size_t next_name_max_len;
    struct command_data_block *next;        /* 关键字的下一跳 */
    struct command_data_block *alt;         /* 备选关键字 */
    char  *desc;                            /* 关键字的描述信息 */
    /* 执行命令的入口 */
    void (*exec)(struct command_data_block *);
    void *private_data;                     /* 私有数据 */
};

/* 命令提示符 */
extern char *cli_prompt;

/**
 * 注册命令关键字
 *
 * @param:
 *      name            命令关键字的名称
 *      desc            描述信息
 *      private_data    私有数据
 *      super           关键字的继承节点
 *      exec            命令的执行入口
 *
 * @retval:
 *      返回关键字的数据块指针，NULL表示注册失败
 */
extern struct command_data_block *cli_regist_command(
        char *name, char *desc, void *private_data,
        struct command_data_block *super,
        void (*exec)(struct command_data_block *));

extern struct command_data_block *cli_regist_string(
        char *desc, void *private_data,
        struct command_data_block *super,
        void (*exec)(struct command_data_block *));

extern int cli_exec(char *buf, size_t size);

extern void release_cli_tree(void);

extern struct command_data_block *cli_first_command_data_block(void);

static inline bool cli_string_is(struct cli_string_block *str_blk, char *str, int case_flag) {
    size_t len = strlen(str);
    int (*cmp)(const char *, const char *, size_t) = case_flag ? strncmp : strncasecmp;
    return len == str_blk->size && !(*cmp)(str_blk->string, str, len);
}

extern void cli_init(void);

extern void cli_exec_input(char *buff, size_t len, int *tab_flag);

#endif /* End of _NASSURA_CLI_H_ */

