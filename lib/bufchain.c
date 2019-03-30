#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bufchain.h"

static unsigned int __bufchain_next_addr_offset(bufchain chain,
        unsigned int counts)
{
    return (chain->offset + counts * chain->count_size) %
        chain->size;
}

void *bufchain_next_addr(bufchain chain, unsigned int counts)
{
    return (void *)(chain->buffer_begin_pos +
            __bufchain_next_addr_offset(chain, counts));
}

static void __bufchain_del_counts(bufchain chain, unsigned int counts)
{
    if (chain->counts < counts) {
        chain->counts = 0;
        chain->offset = 0;
    }
    chain->counts -= counts;
    chain->offset = __bufchain_next_addr_offset(chain, counts);
}

bufchain create_bufchain(bufchain_handleoutfn_t out, unsigned int size,
            unsigned int count)
{
    bufchain chain;

    chain = (bufchain)malloc(sizeof(struct bufchain_t) + size * count);
    if (chain == NULL)
        return NULL;

    chain->total_counts = count;
    chain->count_size = size;
    chain->size = count * size;
    chain->offset = 0;
    chain->counts = 0;
    chain->out = out;

    return chain;
}

void bufchain_prefix(bufchain chain, void **pbuf, unsigned int *pcounts)
{
    *pcounts = bufchain_counts(chain);
    *pbuf = (void *)(chain->buffer_begin_pos + chain->offset);
}

int bufchain_add(bufchain chain, void *buff, unsigned int counts)
{
    char *to_buffer, *from_buffer;
    unsigned int total_counts, offset;
    unsigned int i, j, r;

    to_buffer = (char *)chain->buffer_begin_pos;
    from_buffer = (char *)buff;
    offset = chain->offset + chain->counts * chain->count_size;
    total_counts = chain->total_counts - chain->counts;
    if (counts < total_counts)
        total_counts = counts;
    for (i = 0, r = 0; i < total_counts; ++i) {
        offset %= chain->size;
        for (j = 0; j < chain->count_size; ++j)
            to_buffer[offset++] = from_buffer[r++];
    }

    chain->counts += total_counts;
    return total_counts;
}

static inline int __buf_chain_del(bufchain_handleoutfn_t handler, void *from_buffer,
        unsigned int buffer_size)
{
    return handler(from_buffer, buffer_size);
}

int bufchain_del(bufchain chain, unsigned int counts)
{
    bool repeat_flag;
    unsigned int handled_counts;
    unsigned int end_count_offset;
    unsigned int current_count_offset;
    unsigned int current_handle_counts;

    /* current_count_offset计算出游标的位置,表示当前游标在第n个数据项 */
    current_count_offset = chain->offset / chain->count_size;
    /* counts为实际可以出队的数据项数 */
    counts = counts < chain->counts ? counts : chain->counts;
    if (chain->out != NULL) { /* 使用指定的函数处理出队的数据 */
        repeat_flag = true; /* 回绕标识符 */
        end_count_offset = current_count_offset + counts;
        current_handle_counts = counts;
        /* 判断会不会发生回绕的情况 */
        if (end_count_offset > chain->total_counts) {
            current_handle_counts = chain->total_counts - current_count_offset;
buf_chain_del_repeat_L0:
            while (current_handle_counts > 0) { /* 出队处理 */
                handled_counts = __buf_chain_del(chain->out, chain->buffer_begin_pos
                        + chain->offset, current_handle_counts);
                current_handle_counts -= handled_counts;
                __bufchain_del_counts(chain, handled_counts);
            }
            current_handle_counts = end_count_offset -
                    chain->total_counts;
        }
        if (repeat_flag) { /* 回绕只发生一次 */
            repeat_flag = false;
            goto buf_chain_del_repeat_L0;
        }
    } else {
        __bufchain_del_counts(chain, counts);
    }

    if (chain->counts == 0) /* 优化选项 */
        chain->offset = 0;

    return counts;
}

void bufchain_destory(bufchain chain)
{
    free(chain);
}

