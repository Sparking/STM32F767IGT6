#include <stdlib.h>
#include <string.h>
#include "readline.h"
#include "cli.h"

#include "usart.h"

/* 计算结构体成员在结构体中的偏移量 */
#define cli_offset(p, m) \
    ((char *)(p) - (char *)&((struct command_data_block *)0)->m)
/* 父结构体的入口 */
#define cli_entry(p, m) ((struct command_data_block *)cli_offset(p, m))
#define cli_tree_root   (*cli_entry(&_cli_tree_root, next_name_max_len))

/* 命令提示符 */
char *cli_prompt = "# ";

static struct {
    struct command_data_block *_0;
    size_t _1;
} _cli_tree_root;

void cli_init(void)
{
    cli_tree_root.next = NULL;
    cli_tree_root.next_name_max_len = 0;
}

/* 返回命令树的第一个next节点 */
struct command_data_block *cli_first_command_data_block(void)
{
    return cli_tree_root.next;
}

static void _release_cli_tree_node(struct command_data_block *pcdb)
{
    if (pcdb) {
        _release_cli_tree_node(pcdb->alt);
        _release_cli_tree_node(pcdb->next);
        free(pcdb);
    }
}

/* 释放命令树 */
void release_cli_tree(void)
{
    _release_cli_tree_node(cli_tree_root.next);
    cli_init();
}

/* 查找下一个备选节点 */
static struct command_data_block *cli_find_alt(struct command_data_block *pcdb,
        char *name, int size)
{
    while (pcdb) {
        switch (pcdb->data_block_type) {
        case COMMAND_DATA_BLOCK_TYPE_INTEGER:
            goto out;
        case COMMAND_DATA_BLOCK_TYPE_FLAGS:
            goto out;
        case COMMAND_DATA_BLOCK_TYPE_STRING:
            pcdb->data_block.string.size = size;
            pcdb->data_block.string.string = name;
            goto out;
        case COMMAND_DATA_BLOCK_TYPE_COMMAND: 
        /* Fall Through */
        default:
            if (strlen(pcdb->data_block.command) == size &&
                    !strncasecmp(name, pcdb->data_block.command, size)) {
                goto out;
            }
            break;
        }
        pcdb = pcdb->alt;
    }

out:
    return pcdb;
}


static struct command_data_block *cli_find_first_alt_with_prefix(
        struct command_data_block *pcdb, char *name, int prefix_size)
{
    while (pcdb) {
        if (pcdb->data_block_type != COMMAND_DATA_BLOCK_TYPE_COMMAND) {
            pcdb = NULL;
            break;
        }
        if (!strncasecmp(name, pcdb->data_block.command, prefix_size)) {
            break;
        }
        pcdb = pcdb->alt;
    }

    return pcdb;
}

static void cli_config_command_data_block(struct command_data_block *pcdb,
        int data_block_type, char *desc, void *private_data,
        struct command_data_block *alt, struct command_data_block *next,
        void (*exec)(struct command_data_block *))
{
    pcdb->private_data = private_data;
    pcdb->data_block_type = data_block_type;
    pcdb->desc = desc;
    pcdb->alt = alt;
    pcdb->next = next;
    pcdb->exec = exec;
    pcdb->next_name_max_len = 0;
}

static struct command_data_block *cli_append_command_pcdb_by_sort(
        struct command_data_block *super, struct command_data_block *new)
{
    int len;
    int comp_ret;
    struct command_data_block **prev;

    if (super == NULL) {
        super = &cli_tree_root;
    }
    comp_ret = 1;
    prev = &super->next;
    while (*prev) {
        if ((*prev)->alt == NULL ||
            (*prev)->data_block_type != COMMAND_DATA_BLOCK_TYPE_COMMAND ||
            (comp_ret = strcasecmp((*prev)->data_block.command,
            new->data_block.command)) >= 0) {
            if (comp_ret != 0)
                break;
            cli_printf("keyword %s has already been set!\n", new->data_block.command);
            cli_free(new);
            return NULL;
        }
        prev = &(*prev)->alt;
    }

    new->alt = *prev;
    *prev = new;
    len = strlen(new->data_block.command);
    if (len > super->next_name_max_len) {
        super->next_name_max_len = len;
    }

    return new;
}

struct command_data_block *cli_regist_command(
        char *name, char *desc, void *private_data,
        struct command_data_block *super,
        void (*exec)(struct command_data_block *))
{
    struct command_data_block *this_pcdb;

    this_pcdb = (struct command_data_block *)cli_malloc(sizeof(*this_pcdb));
    if (this_pcdb == NULL) {
        return NULL;
    }

    this_pcdb->data_block.command = name;
    cli_config_command_data_block(this_pcdb, COMMAND_DATA_BLOCK_TYPE_COMMAND,
            desc, private_data, NULL, NULL, exec);

    return cli_append_command_pcdb_by_sort(super, this_pcdb);
}

static struct command_data_block *cli_append_other_type_pcdb(
        struct command_data_block *super, struct command_data_block *new)
{
    int len;
    struct command_data_block **prev;

    if (super == NULL || new->alt != NULL) {
        goto out;
    }

    prev = &super->next;
    while (*prev) {
        if ((*prev)->data_block_type != COMMAND_DATA_BLOCK_TYPE_COMMAND) {
out:
            cli_free(new);
            return NULL;
        }
        *prev = (*prev)->alt;
    }
    *prev = new;

    switch (new->data_block_type) {
    case COMMAND_DATA_BLOCK_TYPE_FLAGS:
    case COMMAND_DATA_BLOCK_TYPE_STRING:
        len = 4;
        break;
    case COMMAND_DATA_BLOCK_TYPE_INTEGER:
        len = 5;
        break;
    default:
        break;
    }

    if (len > super->next_name_max_len) {
        super->next_name_max_len = len;
    }

    return new;
}

struct command_data_block *cli_regist_string(char *desc,
        void *private_data, struct command_data_block *super,
        void (*exec)(struct command_data_block *))
{
    struct command_data_block *this_pcdb;

    this_pcdb = (struct command_data_block *)cli_malloc(sizeof(*this_pcdb));
    if (this_pcdb == NULL)
        return NULL;

    this_pcdb->data_block.string.string = NULL;
    this_pcdb->data_block.string.size = 0;
    cli_config_command_data_block(this_pcdb, COMMAND_DATA_BLOCK_TYPE_STRING,
            desc, private_data, NULL, NULL, exec);

    return cli_append_other_type_pcdb(super, this_pcdb);
}

static int cli_command_max_prefix_size(char *string1, char *string2)
{
    int size;

    for (size = 0; string1[size] && string2[size]; size++) {
        if (string1[size] != string2[size])
           break; 
    }

    return size;
}

/**
 * return the max size of common prefix
 */
static int cli_trave_keyword(struct command_data_block *super,
        struct command_data_block *pcdb,
        struct command_data_block **first_alt_for_tab, int *only_one_flag,
        char *prefix, int size, int print_detail_flag)
{
    char *name;
    struct command_data_block *alt;
    struct command_data_block *last_alt_for_tab;

    if (pcdb == NULL) {
        return 0;
    }

    if (pcdb->data_block_type == COMMAND_DATA_BLOCK_TYPE_COMMAND) {
        alt = cli_find_first_alt_with_prefix(pcdb, prefix, size);
        *only_one_flag = 1;
    } else {
        *only_one_flag = 0;
        alt = pcdb;
    }
    *first_alt_for_tab = alt;
    if (alt == NULL) {
        return -1;
    }
    last_alt_for_tab = NULL;
    while (alt) {
        last_alt_for_tab = alt;
        switch (alt->data_block_type) {
        case COMMAND_DATA_BLOCK_TYPE_STRING:
            name = "WORD";
            break;
        case COMMAND_DATA_BLOCK_TYPE_FLAGS:
            name = "FLAG";
            break;
        case COMMAND_DATA_BLOCK_TYPE_INTEGER:
            name = "NUMBER";
            break;
        case COMMAND_DATA_BLOCK_TYPE_COMMAND:
        /* FALL THROUGH */
        default:
            name = alt->data_block.command;
            break;
        }
        if (print_detail_flag) {
            cli_printf("\033[33m%*s\033[0m: %s\n", super->next_name_max_len, name, alt->desc);
        } else {
            cli_printf("%s", name);
        }
        
        if (alt->alt && alt->alt->data_block_type
                == COMMAND_DATA_BLOCK_TYPE_COMMAND) {
            alt = cli_find_first_alt_with_prefix(alt->alt, prefix, size);
        } else {
            alt = alt->alt;
        }

        if (!print_detail_flag) {
            cli_printf(alt ? "\t" : "\n");
        }
    }
    fflush(stdout);

    if (last_alt_for_tab == NULL) {
        return size;
    } else if (last_alt_for_tab->data_block_type
            != COMMAND_DATA_BLOCK_TYPE_COMMAND ||
            (*first_alt_for_tab)->data_block_type
            != COMMAND_DATA_BLOCK_TYPE_COMMAND) {
        *only_one_flag = 0;
        return size;
    }

    if (*first_alt_for_tab != last_alt_for_tab) {
        *only_one_flag = 0;
    }
    return cli_command_max_prefix_size(
            (*first_alt_for_tab)->data_block.command,
            last_alt_for_tab->data_block.command);
}

int cli_exec(char *buf, size_t size)
{
    struct command_data_block *super;
    struct command_data_block *pcdb;
    struct command_data_block *next;
    struct command_data_block *first_alt;
    char *line;
    char *keyword;
    int only_one_flag;
    int keyword_size;
    int lastc;
    int ret;

    if (buf == NULL) {
        goto none_out;
    }

    only_one_flag = 0;
    pcdb = NULL;
    first_alt = NULL;
    next = cli_tree_root.next;
    super = &cli_tree_root;
    line = buf;
    while ((keyword = read_first_word(line, &keyword_size, " ")) != NULL) {
        line = keyword + keyword_size;
        lastc = keyword_size - 1;
        if (keyword[lastc] == '\t' || keyword[lastc] == '?') {
            putchar('\n');
            ret = cli_trave_keyword(super, next, &first_alt, &only_one_flag, keyword,
                    lastc, keyword[lastc] - '\t');
            keyword[lastc] = '\0';
            if (ret < 0 || (next == NULL && lastc != 0)) {
                return CLI_EXEC_UNKNOW_TAB;
            }
            if (ret != 0) {
                if (buf + size < keyword + ret) {
                    cli_printf("\033[41mwaring\033[0m: the cli input buffer is full!\n");
                    ret = buf + size - keyword;
                }
                if (first_alt->data_block_type == COMMAND_DATA_BLOCK_TYPE_COMMAND) {
                    memcpy(keyword, first_alt->data_block.command, ret);
                    keyword[ret] = '\0';
                }
                if (buf + size < keyword + ret + 1) {
                    cli_printf("\033[41mwaring\033[0m: this cli input buffer is full!\n");
                    return CLI_EXEC_READ_TAB;
                }
                if (only_one_flag) {
                    keyword[ret++] = ' ';
                    keyword[ret] = '\0';
                }
            }
            return CLI_EXEC_READ_TAB;
        }
        pcdb = cli_find_alt(next, keyword, keyword_size);
        if (pcdb == NULL) {
            cli_printf("\'%s\'\n \033[35m%*s\033[0m\n", buf, (int)(1 + keyword - buf), "^");
            return CLI_EXEC_UNKNOW;
        }
        super = pcdb;
        next = pcdb->next;
    }

    if (next == cli_tree_root.next) {
none_out:
        return CLI_EXEC_NONE;
    }

    if (pcdb->exec == NULL) {
        cli_printf("command %s is not end\n", buf);
        ret = CLI_EXEC_NO_END;
    } else {
        pcdb->exec(pcdb);
        ret = CLI_EXEC_SUCCESS;
    }

    return ret;
}

void cli_exec_input(char *buff, size_t len)
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
            printf("\033[K");
            fflush(stdout);
        }
    }

    buff[index] = '\0';
    if ((ret = cli_exec(buff, len)) < 0) {
        cli_printf("unknow command!\n");
    }
}
