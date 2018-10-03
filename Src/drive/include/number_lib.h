#ifndef __NUMBER_LIB_H_
#define __NUMBER_LIB_H_

#include <stdint.h>

extern uint32_t BCD2NUM(uint32_t bcd);
extern uint32_t NUM2BCD(uint32_t number);
extern void Num2Str(uint32_t number, char *s, int len);
extern signed int Str2Num(char *string);

#endif
