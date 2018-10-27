#ifndef _NASSURA_CLI_H_
#define _NASSURA_CLI_H_

#include <stdio.h>
#include <stdlib.h>

#define cli_printf(...)     printf(__VA_ARGS__)
#define cli_malloc(size)    malloc(size)
#define cli_free(p)         free(p)

#define CLI_EXEC_NO_END     -1
#define CLI_EXEC_UNKNOW     -2
#define CLI_EXEC_UNKNOW_TAB -3
#define CLI_EXEC_FAIL       -4
#define CLI_EXEC_NONE       0
#define CLI_EXEC_READ_TAB   1
#define CLI_EXEC_SUCCESS    2

enum {
    COMMAND_DATA_BLOCK_TYPE_COMMAND = 0,
    COMMAND_DATA_BLOCK_TYPE_INTEGER, 
    COMMAND_DATA_BLOCK_TYPE_STRING,
    COMMAND_DATA_BLOCK_TYPE_FLAGS,
};

struct cli_string_block {
    char  *string;
    size_t size;
};

struct command_data_block {
    int    data_block_type;
    union {
    	char  *command;
        size_t integer;
        size_t flags;
        struct cli_string_block string;
    } data_block;
	char  *desc;
	struct command_data_block *alt;
	struct command_data_block *next;
	void (*exec)(struct command_data_block *);
    void *private_data;
};

extern char *cli_prompt;

extern struct command_data_block *cli_regist_command(
		char *name, char *desc, void *private_data,
        struct command_data_block *super,
        void (*exec)(struct command_data_block *));

extern struct command_data_block *cli_regist_string(
        char *desc, void *private_data,
        struct command_data_block *super,
        void (*exec)(struct command_data_block *));

extern int cli_exec(char *buf, size_t size);
		
#endif /* End of _NASSURA_CLI_H_ */
