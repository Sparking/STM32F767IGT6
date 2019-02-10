#ifndef __BUFCHAIN_H_
#define __BUFCHAIN_H_

#include <stdbool.h>

/*
 * 处理出队数据的函数
 * 该函数被函数bufchan_del回调
 */
typedef int (* bufchain_handleoutfn_t)(void *from_buffer, int counts);

 __attribute__(( aligned(sizeof(void*)) )) struct bufchain_t {
	bufchain_handleoutfn_t out;     /* 回调函数，处理出队的元素 */
	unsigned int count_size;        /* 每项数据占用的空间大小 */
	unsigned int total_counts;      /* 队列可存放的数据项数 */
	unsigned int size;              /* 存储空间的总大小 */
	unsigned int offset;            /* 地址偏移量 */
	unsigned int counts;            /* 队列中的数据项数 */
	char buffer_begin_pos[];        /* 队列的存储空间,紧凑分布 */
};

typedef struct bufchain_t* bufchain;

static inline unsigned int bufchain_counts(bufchain chain) {
	return chain->counts;
}

static inline bool bufchain_is_full(bufchain chain) {
	return chain->counts == chain->total_counts;
}

static inline bool bufchain_is_empty(bufchain chain) {
	return chain->counts == 0;
}

extern bufchain create_bufchain(bufchain_handleoutfn_t out,
		unsigned int size, unsigned int count);
extern void bufchain_prefix(bufchain chain, void **pbuf,
		unsigned int *pcounts);
extern int bufchain_add(bufchain chain, void *buff, unsigned int counts);
extern int bufchain_del(bufchain chain, unsigned int counts);
extern void *bufchain_next_addr(bufchain chain, unsigned int counts);
extern void bufchain_destory(bufchain chain);

#endif

